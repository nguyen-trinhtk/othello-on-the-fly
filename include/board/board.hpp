#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include "constants.hpp"

namespace othello
{
    namespace board
    {

        struct Move
        {
            int row;
            int col;
            std::uint64_t flip_mask = 0;

            Move() noexcept
                : row(-1), col(-1) {}
            Move(int r, int c) noexcept
                : row(r), col(c) {}

            [[nodiscard]] std::size_t flip_count() const noexcept
            {
                return static_cast<std::size_t>(__builtin_popcountll(flip_mask));
            }

            [[nodiscard]] bool operator==(const Move &other) const noexcept
            {
                return row == other.row && col == other.col;
            }
        };

        class Board
        {
        private:
            std::uint64_t white_moves;
            std::uint64_t black_moves;
            std::uint64_t hash_value = 0;

            Disc current_turn;

            // Zobrist table for hashing
            static std::uint64_t zobrist_table[8][8][2]; // 8x8 board, 2 players
            static void init_zobrist();
            static void ensure_zobrist_initialized();

            [[nodiscard]] static constexpr bool is_in_bounds(int r, int c) noexcept
            {
                return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
            }

            [[nodiscard]] static constexpr std::uint64_t bit_at(int r, int c) noexcept
            {
                return 1ULL << (8 * r + c);
            }

            [[nodiscard]] std::uint64_t generate_valid_move_mask(Disc player) const;
            [[nodiscard]] std::uint64_t collect_flip_mask(const Move &move, Disc player) const;
            void rebuild_hash();
            void xor_square_hash(int r, int c, Disc disc) noexcept;
            void xor_mask_hash(std::uint64_t mask, Disc disc) noexcept;
            void transfer_mask_hash(std::uint64_t mask, Disc from, Disc to) noexcept;

        public:
            Board();

            // Board operations
            [[nodiscard]] inline Disc get_square(int r, int c) const
            {
                if (!is_in_bounds(r, c))
                {
                    return EMPTY;
                }
                const bool black = (black_moves >> (8 * r + c)) & 1;
                const bool white = (white_moves >> (8 * r + c)) & 1;
                if (black)
                {
                    return BLACK;
                }
                else if (white)
                {
                    return WHITE;
                }
                else
                {
                    return EMPTY;
                }
            }

            int set_square(int r, int c, Disc disc);

            // Disc differential utility: positive if more discs for 'player', negative if fewer
            [[nodiscard]] inline int disc_diff(Disc player) const
            {
                // Count bits for each player
                const int black_count = __builtin_popcountll(black_moves);
                const int white_count = __builtin_popcountll(white_moves);
                return player == BLACK ? (black_count - white_count) : (white_count - black_count);
            }

            // Getters for disc counts
            [[nodiscard]] inline int get_black_count() const
            {
                return __builtin_popcountll(black_moves);
            }
            [[nodiscard]] inline int get_white_count() const
            {
                return __builtin_popcountll(white_moves);
            }

            // Moves
            // Returns true if neither player has valid moves
            [[nodiscard]] inline bool is_game_over() const
            {
                return !has_valid_moves(BLACK) && !has_valid_moves(WHITE);
            }

            [[nodiscard]] inline std::uint64_t get_valid_move_mask(Disc player) const
            {
                return generate_valid_move_mask(player);
            }
            [[nodiscard]] inline std::uint64_t get_valid_move_mask() const
            {
                return generate_valid_move_mask(current_turn);
            }
            [[nodiscard]] inline std::uint64_t get_player_bits(Disc player) const
            {
                return player == BLACK ? black_moves : white_moves;
            }
            [[nodiscard]] inline std::uint64_t get_empty_bits() const
            {
                return ~(black_moves | white_moves);
            }
            [[nodiscard]] inline Disc get_current_player() const
            {
                return current_turn;
            }

            void set_current_player(Disc player);

            [[nodiscard]] bool has_valid_moves(Disc player) const;
            int compute_valid_moves() const;
            [[nodiscard]] bool is_valid_move(int r, int c, Disc player) const;
            [[nodiscard]] int process_move(const Move &move, Disc player);
            [[nodiscard]] int undo_move(const Move &move, Disc player);
            [[nodiscard]] std::vector<Move> get_moves_for_current_state() const;

            // Zobrist hashing
            [[nodiscard]] std::uint64_t get_hash() const;
        };

    } // namespace board
} // namespace othello

#endif
