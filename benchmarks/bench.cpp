#include "othello/ai.h"
#include "othello/board.h"
#include "othello/types.h"
#include "eval/eval_registry.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace
{
    std::string eval_choices()
    {
        std::ostringstream out;
        const auto& entries = eval_registry();
        for (std::size_t i = 0; i < entries.size(); ++i) {
            if (i > 0)
                out << " | ";
            out << entries[i].name;
        }
        return out.str();
    }

    void usage(const char *argv0)
    {
        std::cerr << "Usage: " << (argv0 ? argv0 : "bench") << " [depth] [runs] [eval] [parallel]\n"
                  << "  depth     search depth (default 10)\n"
                  << "  runs      repetitions after warm-up (default 20)\n"
                  << "  eval      " << eval_choices() << " (default component)\n"
                  << "  parallel  none | thread-pool (default thread-pool)\n";
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

    const int depth = argc >= 2 ? parse_pos_int(argv[1], "depth", 10) : 10;
    const int runs = argc >= 3 ? parse_pos_int(argv[2], "runs", 20) : 20;

    const char *eval_name = (argc >= 4 && argv[3]) ? argv[3] : "component";
    const auto eval_index = find_eval(eval_name);
    if (!eval_index)
    {
        std::cerr << "Unknown eval \"" << eval_name << "\"; use " << eval_choices() << ".\n";
        return 1;
    }
    const EvalEntry& eval_entry = eval_registry()[*eval_index];
    std::unique_ptr<IEvaluator> evaluator = eval_entry.make();

    const char *parallel_name = (argc >= 5 && argv[4]) ? argv[4] : "thread-pool";
    bool parallel = true;
    if (std::strcmp(parallel_name, "none") == 0)
        parallel = false;
    else if (std::strcmp(parallel_name, "thread-pool") == 0)
        parallel = true;
    else
    {
        std::cerr << "Unknown parallel \"" << parallel_name << "\"; use none or thread-pool.\n";
        return 1;
    }

    const Board board = Board::standard_start();
    AIEngine ai(depth, std::move(evaluator));
    ai.set_parallel(parallel);

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

    std::cout << "eval=" << eval_entry.name << " eval_label=" << eval_entry.label
              << " parallel=" << parallel_name << " depth=" << depth << " runs=" << runs
              << " total_ms=" << total_ms << " avg_ms=" << avg_ms << '\n';

    return 0;
}
