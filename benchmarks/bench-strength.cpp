#include "othello/ai.h"
#include "othello/board.h"
#include "othello/game_result.h"
#include "othello/rules.h"
#include "othello/types.h"
#include "cli.h"
#include "eval_registry.h"

#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

namespace
{
    // ε-Othello: each player makes a uniformly random move with probability ε.
    // This breaks determinism so win rates become continuous across many games.
    constexpr float kEps = 0.10f;

    int play_game(AIEngine& row_ai, AIEngine& col_ai, Player row_color,
                  std::mt19937& rng)
    {
        Board board = Board::standard_start();
        Player cur = Player::BLACK;

        std::bernoulli_distribution random_move{kEps};
        std::uniform_int_distribution<int> rand_idx;

        for (;;) {
            if (!Rules::has_valid_move(board, cur)) {
                if (!Rules::has_valid_move(board, opponent(cur)))
                    break;
                cur = opponent(cur);
                continue;
            }

            std::optional<Move> move;
            if (random_move(rng)) {
                const auto moves = Rules::get_all_valid_moves(board, cur);
                rand_idx.param(std::uniform_int_distribution<int>::param_type{
                    0, static_cast<int>(moves.size()) - 1});
                move = moves[rand_idx(rng)];
            } else {
                AIEngine& ai = (cur == row_color) ? row_ai : col_ai;
                move = ai.best_move(board, cur);
            }

            if (!move) { cur = opponent(cur); continue; }
            Rules::apply_move(board, cur, *move);
            cur = opponent(cur);
        }

        const GameOutcome out = outcome_from_board(board);
        if (!out.winner) return 0;
        return *out.winner == row_color ? 1 : -1;
    }

    double win_rate_pct(int row_wins, int draws, int games)
    {
        return 100.0 * (static_cast<double>(row_wins) +
                        0.5 * static_cast<double>(draws)) /
               static_cast<double>(games);
    }

    void usage(const char* argv0)
    {
        std::cerr << "Usage: " << (argv0 ? argv0 : "bench-strength")
                  << " [depth] [matches]\n"
                  << "  depth    search depth (default 3)\n"
                  << "  matches  games per pairing  (default 200)\n"
                  << "  Each move has " << kEps * 100 << "% chance of being random\n"
                  << "  (epsilon-Othello, as in Jaskowski 2014)\n";
    }
} // namespace

int main(int argc, char** argv)
{
    if (argc >= 2 && argv[1] &&
        (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)) {
        usage(argv[0]);
        return 0;
    }

    const int depth   = argc >= 2 ? bench::parse_pos_int(argv[1], "depth",   3)   : 3;
    const int matches = argc >= 3 ? bench::parse_pos_int(argv[2], "matches", 200) : 200;

    std::mt19937 rng{std::random_device{}()};

    const auto& entries = eval_registry();
    const std::size_t n = entries.size();
    std::vector<std::vector<double>> rates(n, std::vector<double>(n, 0.0));

    for (std::size_t row = 0; row < n; ++row) {
        for (std::size_t col = 0; col < n; ++col) {
            if (row == col) continue;

            AIEngine row_ai(depth, entries[row].make());
            AIEngine col_ai(depth, entries[col].make());
            row_ai.set_parallel(false);
            col_ai.set_parallel(false);

            int row_wins = 0, draws = 0;
            for (int g = 0; g < matches; ++g) {
                const Player row_color = (g < matches / 2) ? Player::BLACK : Player::WHITE;
                const int result = play_game(row_ai, col_ai, row_color, rng);
                if (result > 0) ++row_wins;
                else if (result == 0) ++draws;
            }
            rates[row][col] = win_rate_pct(row_wins, draws, matches);
        }
    }

    std::size_t name_width = 5;
    for (const auto& e : entries)
        name_width = std::max(name_width, e.name.size());

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "bench-strength  (ε=" << kEps << " random moves)\n"
              << "  depth   : " << depth << '\n'
              << "  matches : " << matches << " (half as black, half as white)\n\n";
    std::cout << "win_rate %  row = player, col = opponent\n\n";

    std::cout << std::setw(static_cast<int>(name_width)) << ' ';
    for (const auto& e : entries)
        std::cout << ' ' << std::setw(static_cast<int>(name_width)) << e.name;
    std::cout << '\n';

    for (std::size_t row = 0; row < n; ++row) {
        std::cout << std::setw(static_cast<int>(name_width)) << entries[row].name;
        for (std::size_t col = 0; col < n; ++col) {
            if (row == col)
                std::cout << ' ' << std::setw(static_cast<int>(name_width)) << '-';
            else
                std::cout << ' ' << std::setw(static_cast<int>(name_width)) << rates[row][col];
        }
        std::cout << '\n';
    }

    return 0;
}
