#pragma once

#include <cstdint>
#include <vector>

enum class TTBound : std::uint8_t {
    Exact,
    Lower,
    Upper,
};

class TranspositionTable {
    // Single-threaded for now
    // TODO: lock free
public:
    TranspositionTable();
    void clear();

    // Rem depth: remaining depth
    bool probe(std::uint64_t key, int rem_depth, int alpha, int beta, int &out_score) const;
    void store(std::uint64_t key, int rem_depth, int score, int alpha_orig, int beta);

private:
    struct Entry { // TT entry
        std::uint64_t key = 0; // Full Zobrist key
        std::int32_t score = 0;
        std::uint8_t rem_depth = 0;
        TTBound bound = TTBound::Exact;
        std::uint64_t gen = 0;
    };

    static constexpr int kLogSize = 20; // 2^20 = 1M entries
    static constexpr std::size_t kSize = std::size_t{1} << kLogSize;
    static constexpr std::uint64_t kMask = kSize - 1; // Mask for indexing

    std::vector<Entry> slots_; // TT slots
    std::uint64_t gen_ = 1; // Generation counter
};
