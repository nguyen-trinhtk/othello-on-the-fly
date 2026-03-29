#include "board/board.hpp"
#include "engine/trans_table.hpp"

namespace othello
{
    namespace engine
    {
        namespace
        {
            [[nodiscard]] TTMove make_tt_move(const othello::board::Move &move) noexcept
            {
                return {
                    static_cast<std::int8_t>(move.row),
                    static_cast<std::int8_t>(move.col),
                };
            }
        } // namespace

        TranspositionTable::TranspositionTable(std::size_t size)
            : table_(size) {}

        void TranspositionTable::new_search() noexcept
        {
            ++current_generation_;
            if (current_generation_ == 0)
            {
                clear();
            }
        }

        void TranspositionTable::store(
            std::uint64_t hash,
            int value,
            int depth,
            TTEntryType type,
            const othello::board::Move *best_move)
        {
            if (table_.empty())
            {
                return;
            }

            TTEntry entry{};
            entry.hash = hash;
            entry.value = value;
            entry.depth = depth;
            entry.type = type;
            if (best_move != nullptr)
            {
                entry.best_move = make_tt_move(*best_move);
            }

            Slot &slot = table_[hash % table_.size()];
            if (!slot.occupied)
            {
                slot.entry = entry;
                slot.generation = current_generation_;
                slot.occupied = true;
                ++entry_count_;
                return;
            }

            if (slot.entry.hash == hash)
            {
                if (depth >= slot.entry.depth)
                {
                    slot.entry = entry;
                }
                slot.generation = current_generation_;
                return;
            }

            if (slot.generation != current_generation_ || depth >= slot.entry.depth)
            {
                slot.entry = entry;
                slot.generation = current_generation_;
            }
        }

        bool TranspositionTable::lookup(std::uint64_t hash, TTEntry &entry) const
        {
            if (table_.empty())
            {
                return false;
            }

            const Slot &slot = table_[hash % table_.size()];
            if (!slot.occupied || slot.entry.hash != hash)
            {
                return false;
            }

            entry = slot.entry;
            return true;
        }

        void TranspositionTable::clear() noexcept
        {
            for (auto &slot : table_)
            {
                slot = Slot{};
            }
            entry_count_ = 0;
            current_generation_ = 1;
        }

        std::size_t TranspositionTable::size() const noexcept
        {
            return entry_count_;
        }

    } // namespace engine
} // namespace othello
