#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace othello
{
    namespace board
    {
        struct Move;
    }

    namespace engine
    {

        enum class TTEntryType
        {
            EXACT,
            LOWERBOUND,
            UPPERBOUND
        };

        struct TTMove
        {
            std::int8_t row = -1;
            std::int8_t col = -1;

            [[nodiscard]] bool is_valid() const noexcept
            {
                return row >= 0 && col >= 0;
            }
        };

        struct TTEntry
        {
            std::uint64_t hash = 0;
            int value = 0;
            int depth = 0;
            TTEntryType type = TTEntryType::EXACT;
            TTMove best_move{};

            [[nodiscard]] bool has_best_move() const noexcept
            {
                return best_move.is_valid();
            }
        };

        class TranspositionTable
        {
        public:
            explicit TranspositionTable(std::size_t size = 1u << 20);
            void new_search() noexcept;
            void store(
                std::uint64_t hash,
                int value,
                int depth,
                TTEntryType type,
                const othello::board::Move *best_move = nullptr);
            [[nodiscard]] bool lookup(std::uint64_t hash, TTEntry &entry) const;
            void clear() noexcept;
            [[nodiscard]] std::size_t size() const noexcept;

        private:
            struct Slot
            {
                TTEntry entry{};
                std::uint32_t generation = 0;
                bool occupied = false;
            };

            std::vector<Slot> table_;
            std::size_t entry_count_ = 0;
            std::uint32_t current_generation_ = 1;
        };

    } // namespace engine
} // namespace othello
