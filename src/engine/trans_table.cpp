#include "engine/trans_table.hpp"

namespace othello
{
    namespace engine
    {

        TranspositionTable::TranspositionTable(size_t size)
            : max_size_(size) {}

        void TranspositionTable::store(uint64_t hash, int value, int depth, TTEntryType type /*, int move_x, int move_y*/)
        {
            TTEntry entry{hash, value, depth, type};
            // Optionally set move_x, move_y
            if (table_.size() >= max_size_)
            {
                // Simple replacement: erase a random element (could be improved)
                table_.erase(table_.begin());
            }
            table_[hash] = entry;
        }

        bool TranspositionTable::lookup(uint64_t hash, TTEntry &entry) const
        {
            auto it = table_.find(hash);
            if (it != table_.end())
            {
                entry = it->second;
                return true;
            }
            return false;
        }

        void TranspositionTable::clear()
        {
            table_.clear();
        }

        size_t TranspositionTable::size() const
        {
            return table_.size();
        }

    } // namespace engine
} // namespace othello
