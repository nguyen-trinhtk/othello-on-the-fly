#include "othello/ai.h"
#include "cli.h"
#include "eval_registry.h"
#include "positions.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

namespace
{
    struct TimeResult {
        double avg_ms{};
        double nodes_per_sec{};
    };

    TimeResult bench_position(AIEngine& ai, const bench::PositionCase& pos, int runs)
    {
        (void)ai.best_move(pos.board, pos.side);

        double total_ms = 0.0;
        std::uint64_t total_nodes = 0;

        for (int i = 0; i < runs; ++i) {
            ai.reset_search_stats();
            const auto t0 = std::chrono::steady_clock::now();
            (void)ai.best_move(pos.board, pos.side);
            const auto t1 = std::chrono::steady_clock::now();

            const double ms =
                std::chrono::duration<double, std::milli>(t1 - t0).count();
            total_ms += ms;
            total_nodes += ai.last_search_nodes();
        }

        TimeResult result;
        result.avg_ms = total_ms / runs;
        if (total_ms > 0.0)
            result.nodes_per_sec = static_cast<double>(total_nodes) / (total_ms / 1000.0);
        return result;
    }

    void print_header()
    {
        std::cout << std::setw(14) << std::left << "position"
                  << std::setw(12) << "avg ms"
                  << "nodes/s\n";
        std::cout << std::string(36, '-') << '\n';
    }

    void usage(const char* argv0)
    {
        std::cerr
            << "Usage: " << (argv0 ? argv0 : "bench-time")
            << " [strategy] [depth] [runs]\n"
            << "  strategy   " << bench::eval_choices() << " (default mask)\n"
            << "  depth      search depth (default 5)\n"
            << "  runs       repetitions per position (default 20)\n"
            << "\nBenches both none (sequential) and thread-pool parallelism.\n";
    }
} // namespace

int main(int argc, char** argv)
{
    if (argc >= 2 && argv[1] &&
        (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)) {
        usage(argv[0]);
        return 0;
    }

    const char* strategy = (argc >= 2 && argv[1]) ? argv[1] : "mask";
    const int depth = argc >= 3 ? bench::parse_pos_int(argv[2], "depth", 10) : 10;
    const int runs = argc >= 4 ? bench::parse_pos_int(argv[3], "runs", 10) : 10;

    const auto eval_index = find_eval(strategy);
    if (!eval_index) {
        std::cerr << "Unknown strategy \"" << strategy << "\"; use "
                  << bench::eval_choices() << ".\n";
        return 1;
    }

    const EvalEntry& entry = eval_registry()[*eval_index];

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "bench-time\n"
              << "  strategy : " << entry.name << '\n'
              << "  depth    : " << depth << '\n'
              << "  runs     : " << runs << '\n';

    const struct { const char* name; bool parallel; } modes[] = {
        {"none", false},
        {"thread-pool", true},
    };

    for (const auto& mode : modes) {
        AIEngine ai(depth, entry.make());
        ai.set_parallel(mode.parallel);

        std::cout << "\n  parallel : " << mode.name << "\n\n";
        print_header();

        for (const auto& pos : bench::all_positions()) {
            const TimeResult result = bench_position(ai, pos, runs);
            std::cout << std::setw(14) << std::left << pos.name
                      << std::setw(12) << result.avg_ms
                      << static_cast<std::uint64_t>(result.nodes_per_sec) << '\n';
        }
    }

    return 0;
}
