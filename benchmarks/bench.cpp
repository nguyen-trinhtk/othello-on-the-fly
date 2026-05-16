#include "othello/ai.h"
#include "othello/board.h"
#include "othello/types.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace
{
    void usage(const char *argv0)
    {
        std::cerr << "Usage: " << (argv0 ? argv0 : "bench") << " [depth] [runs]\n"
                  << "  depth  search depth (default 8)\n"
                  << "  runs   repetitions after warm-up (default 10)\n";
    }

    int parse_pos_int(const char *s, const char *name, int fallback)
    {
        if (!s || !*s)
            return fallback;
        char *end = nullptr;
        const long v = std::strtol(s, &end, 10);
        if (end == s || *end != '\0' || v <= 0)
        {
            std::cerr << "Invalid " << name << "; using default.\n";
            return fallback;
        }
        return static_cast<int>(v);
    }
} // namespace

int main(int argc, char **argv)
{
    if (argc >= 2 && argv[1] &&
        (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0))
    {
        usage(argv[0]);
        return 0;
    }

    const int depth = argc >= 2 ? parse_pos_int(argv[1], "depth", 8) : 8;
    const int runs = argc >= 3 ? parse_pos_int(argv[2], "runs", 10) : 10;

    const Board board = Board::standard_start();
    AIEngine ai(depth);

    (void)ai.best_move(board, Player::BLACK);

    auto total = std::chrono::nanoseconds{0};
    for (int i = 0; i < runs; ++i)
    {
        const auto t0 = std::chrono::steady_clock::now();
        (void)ai.best_move(board, Player::BLACK);
        const auto t1 = std::chrono::steady_clock::now();
        total += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
    }

    const double total_ms =
        std::chrono::duration<double, std::milli>{total}.count();
    const double avg_ms = total_ms / static_cast<double>(runs);

    std::cout << "depth=" << depth << " runs=" << runs << " total_ms=" << total_ms
              << " avg_ms=" << avg_ms << '\n';

    return 0;
}
