#include "engine/evaluation.hpp"

namespace othello
{
    namespace engine
    {
        namespace
        {
            [[nodiscard]] std::uint64_t shift_mask(
                std::uint64_t bits,
                int direction) noexcept
            {
                if (dirs[direction].shift > 0)
                {
                    return (bits & dirs[direction].mask) << dirs[direction].shift;
                }

                return (bits & dirs[direction].mask) >> -dirs[direction].shift;
            }

            [[nodiscard]] std::uint64_t adjacent_mask(std::uint64_t bits) noexcept
            {
                std::uint64_t adjacent = 0;
                for (int direction = 0; direction < 8; ++direction)
                {
                    adjacent |= shift_mask(bits, direction);
                }

                return adjacent;
            }

            [[nodiscard]] int norm(int a, int b) noexcept
            {
                const int total = a + b;
                if (total == 0)
                {
                    return 0;
                }

                return (100 * (a - b)) / total;
            }

            [[nodiscard]] GamePhase detect_phase(
                const othello::board::Board &board) noexcept
            {
                const int empty_squares = __builtin_popcountll(board.get_empty_bits());
                if (empty_squares >= 44)
                {
                    return GamePhase::Early;
                }
                if (empty_squares >= 10)
                {
                    return GamePhase::Mid;
                }

                return GamePhase::End;
            }

            [[nodiscard]] PhaseWeights phase_weights(GamePhase phase) noexcept
            {
                switch (phase)
                {
                case GamePhase::Early:
                    return {35, 20, 25, 15, 5, 0};
                case GamePhase::Mid:
                    return {30, 15, 30, 10, 15, 0};
                case GamePhase::End:
                    return {10, 5, 20, 10, 55, 0};
                }

                return {};
            }

            [[nodiscard]] int actual_mobility_score(
                const othello::board::Board &board,
                Disc player) noexcept
            {
                const Disc opp = opponent(player);
                const int my_moves = __builtin_popcountll(board.get_valid_move_mask(player));
                const int opp_moves = __builtin_popcountll(board.get_valid_move_mask(opp));
                return norm(my_moves, opp_moves);
            }

            [[nodiscard]] int corner_count(
                const othello::board::Board &board,
                Disc player) noexcept
            {
                int count = 0;
                count += board.get_square(0, 0) == player;
                count += board.get_square(0, BOARD_SIZE - 1) == player;
                count += board.get_square(BOARD_SIZE - 1, 0) == player;
                count += board.get_square(BOARD_SIZE - 1, BOARD_SIZE - 1) == player;
                return count;
            }

            [[nodiscard]] int corner_score(
                const othello::board::Board &board,
                Disc player) noexcept
            {
                return norm(
                    corner_count(board, player),
                    corner_count(board, opponent(player)));
            }

            [[nodiscard]] int parity_score(
                const othello::board::Board &board,
                Disc player) noexcept
            {
                return norm(
                    __builtin_popcountll(board.get_player_bits(player)),
                    __builtin_popcountll(board.get_player_bits(opponent(player))));
            }

            [[nodiscard]] int frontier_score(
                const othello::board::Board &board,
                Disc player) noexcept
            {
                const std::uint64_t empty_bits = board.get_empty_bits();
                const std::uint64_t empty_neighbors = adjacent_mask(empty_bits);
                const int my_frontier = __builtin_popcountll(
                    board.get_player_bits(player) & empty_neighbors);
                const int opp_frontier = __builtin_popcountll(
                    board.get_player_bits(opponent(player)) & empty_neighbors);

                return norm(opp_frontier, my_frontier);
            }

            [[nodiscard]] int potential_mobility_score(
                const othello::board::Board &board,
                Disc player) noexcept
            {
                const std::uint64_t empty_bits = board.get_empty_bits();
                const int my_potential = __builtin_popcountll(
                    empty_bits & adjacent_mask(board.get_player_bits(opponent(player))));
                const int opp_potential = __builtin_popcountll(
                    empty_bits & adjacent_mask(board.get_player_bits(player)));

                return norm(my_potential, opp_potential);
            }

        } // namespace

        int evaluate_position(
            const othello::board::Board &board,
            Disc perspective_player)
        {
            const GamePhase phase = detect_phase(board);
            const PhaseWeights weights = phase_weights(phase);

            const int mobility = actual_mobility_score(board, perspective_player);
            const int potential = potential_mobility_score(board, perspective_player);
            const int corners = corner_score(board, perspective_player);
            const int frontier = frontier_score(board, perspective_player);
            const int parity = parity_score(board, perspective_player);

            return (weights.actual_mobility * mobility +
                    weights.potential_mobility * potential +
                    weights.corners * corners +
                    weights.frontier * frontier +
                    weights.parity * parity) /
                   100;
        }

    } // namespace engine
} // namespace othello
