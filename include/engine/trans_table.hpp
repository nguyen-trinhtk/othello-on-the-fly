#pragma once

#include <unordered_map>
#include <cstdint>

namespace othello
{
    namespace engine
    {

        enum class TTEntryType
        {
            EXACT,
            LOWERBOUND,
            UPPERBOUND
        };

        struct TTEntry
        {
            uint64_t hash;
            int value;
            int depth;
            TTEntryType type;
            // Optional: best move representation
            // int move_x, move_y; // or use your move type
        };

        class TranspositionTable
        {
        public:
            TranspositionTable(size_t size = 1 << 20);
            void store(uint64_t hash, int value, int depth, TTEntryType type /*, int move_x, int move_y*/);
            bool lookup(uint64_t hash, TTEntry &entry) const;
            void clear();
            size_t size() const;

        private:
            std::unordered_map<uint64_t, TTEntry> table_;
            size_t max_size_;
        };

    } // namespace engine
} // namespace othello
