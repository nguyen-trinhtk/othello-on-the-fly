#include "trans_table.h"

TranspositionTable::TranspositionTable() : slots_(kSize) {}

void TranspositionTable::clear()
{
    ++gen_; // Increment generation counter
    if (gen_ == 0) {
        // Reset generation counter
        gen_ = 1;
        for (Entry &e : slots_)
            e.gen = 0;
    }
}

bool TranspositionTable::probe(std::uint64_t key, int rem_depth, int alpha, int beta,
                               int &out_score) const {
    const Entry &e = slots_[static_cast<std::size_t>(key & kMask)];
    if (e.gen != gen_ || e.key != key)
        // Stale
        return false;
    if (static_cast<int>(e.rem_depth) < rem_depth)
        return false; // Too shallow

    switch (e.bound) {
        // Bound types
        case TTBound::Exact:
            out_score = static_cast<int>(e.score);
            return true;
        case TTBound::Lower:
            if (e.score >= beta)
            {
                out_score = static_cast<int>(e.score);
                return true;
            }
            return false;
        case TTBound::Upper:
            if (e.score <= alpha)
            {
                out_score = static_cast<int>(e.score);
                return true;
            }
            return false;
    }
    return false;
}

void TranspositionTable::store(std::uint64_t key, int rem_depth, int score, int alpha_orig, int beta)
{
    // Store TT entry
    TTBound bound = TTBound::Exact;
    if (score >= beta)
        bound = TTBound::Lower;
    else if (score <= alpha_orig)
        bound = TTBound::Upper;

    Entry &e = slots_[static_cast<std::size_t>(key & kMask)];
    e.key = key;
    e.score = static_cast<std::int32_t>(score);
    e.rem_depth = static_cast<std::uint8_t>(rem_depth > 255 ? 255 : rem_depth);
    e.bound = bound;
    e.gen = gen_;
}
