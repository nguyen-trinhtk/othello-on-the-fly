#include <chrono>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "board/board.hpp"
#include "engine/alphabeta.hpp"

namespace
{
    struct BenchmarkConfig
    {
        int depth = ALPHABETA_DEPTH;
        int repeats = 3;
        std::string scenario = "midgame";
        std::optional<std::uint64_t> node_limit{};
        std::optional<std::chrono::milliseconds> time_limit{};
        bool parallel_root = false;
        int parallel_root_min_depth = 5;
        int parallel_root_min_moves = 5;
        int parallel_root_max_workers = 0;
    };
    void print_usage(const char *program_name)
    {
        std::cout
            << "Usage: " << program_name << " [--depth N] [--repeats N] [--scenario opening|midgame|endgame]\n"
            << "       [--node-limit N] [--time-ms N] [--parallel-root]\n"
            << "       [--parallel-root-min-depth N] [--parallel-root-min-moves N]\n"
            << "       [--parallel-root-max-workers N]\n";
    }

    [[nodiscard]] int parse_int_arg(const std::string &value, const char *flag_name)
    {
        try
        {
            return std::stoi(value);
        }
        catch (const std::exception &)
        {
            std::cerr << "Invalid value for " << flag_name << ": " << value << '\n';
            std::exit(1);
        }
    }

    void set_board_state(
        othello::board::Board &board,
        const std::vector<std::vector<Disc>> &state,
        Disc current_player)
    {
        for (int row = 0; row < BOARD_SIZE; ++row)
        {
            for (int col = 0; col < BOARD_SIZE; ++col)
            {
                board.set_square(row, col, state[row][col]);
            }
        }
        board.set_current_player(current_player);
    }

    [[nodiscard]] othello::board::Board make_board_for_scenario(const std::string &scenario)
    {
        othello::board::Board board;
        if (scenario == "opening")
        {
            board.set_current_player(BLACK);
            return board;
        }

        if (scenario == "midgame")
        {
            const std::vector<std::vector<Disc>> state = {
                {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
                {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
                {EMPTY, BLACK, BLACK, WHITE, WHITE, WHITE, EMPTY, EMPTY},
                {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
                {EMPTY, EMPTY, WHITE, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
                {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
            };
            set_board_state(board, state, BLACK);
            return board;
        }

        if (scenario == "endgame")
        {
            const std::vector<std::vector<Disc>> state = {
                {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
                {BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK},
                {BLACK, WHITE, BLACK, BLACK, BLACK, BLACK, WHITE, BLACK},
                {BLACK, WHITE, BLACK, WHITE, WHITE, BLACK, WHITE, BLACK},
                {BLACK, WHITE, BLACK, WHITE, EMPTY, BLACK, WHITE, BLACK},
                {BLACK, WHITE, BLACK, BLACK, BLACK, EMPTY, WHITE, BLACK},
                {BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK},
                {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, EMPTY, EMPTY},
            };
            set_board_state(board, state, BLACK);
            return board;
        }

        std::cerr << "Unknown scenario: " << scenario << '\n';
        std::exit(1);
    }

    [[nodiscard]] std::string format_move(const std::optional<othello::board::Move> &move)
    {
        if (!move.has_value())
        {
            return "pass";
        }

        return std::string(1, static_cast<char>('A' + move->col)) + std::to_string(move->row + 1);
    }

    [[nodiscard]] BenchmarkConfig parse_args(int argc, char **argv)
    {
        BenchmarkConfig config{};

        for (int index = 1; index < argc; ++index)
        {
            const std::string arg = argv[index];
            if (arg == "--help")
            {
                print_usage(argv[0]);
                std::exit(0);
            }

            if (arg == "--parallel-root")
            {
                config.parallel_root = true;
                continue;
            }

            if (index + 1 >= argc)
            {
                std::cerr << "Missing value for " << arg << '\n';
                std::exit(1);
            }

            const std::string value = argv[++index];
            if (arg == "--depth")
            {
                config.depth = parse_int_arg(value, "--depth");
            }
            else if (arg == "--repeats")
            {
                config.repeats = parse_int_arg(value, "--repeats");
            }
            else if (arg == "--scenario")
            {
                config.scenario = value;
            }
            else if (arg == "--node-limit")
            {
                config.node_limit = static_cast<std::uint64_t>(parse_int_arg(value, "--node-limit"));
            }
            else if (arg == "--time-ms")
            {
                config.time_limit = std::chrono::milliseconds(parse_int_arg(value, "--time-ms"));
            }
            else if (arg == "--parallel-root-min-depth")
            {
                config.parallel_root_min_depth = parse_int_arg(value, "--parallel-root-min-depth");
            }
            else if (arg == "--parallel-root-min-moves")
            {
                config.parallel_root_min_moves = parse_int_arg(value, "--parallel-root-min-moves");
            }
            else if (arg == "--parallel-root-max-workers")
            {
                config.parallel_root_max_workers = parse_int_arg(value, "--parallel-root-max-workers");
            }

            else
            {
                std::cerr << "Unknown argument: " << arg << '\n';
                print_usage(argv[0]);
                std::exit(1);
            }
        }

        return config;
    }
} // namespace

int main(int argc, char **argv)
{
    const BenchmarkConfig config = parse_args(argc, argv);
    othello::engine::SearchOptions options{};
    options.max_depth = config.depth;
    options.node_limit = config.node_limit;
    options.time_limit = config.time_limit;
    options.parallel_root = config.parallel_root;
    options.parallel_root_min_depth = config.parallel_root_min_depth;
    options.parallel_root_min_moves = config.parallel_root_min_moves;
    options.parallel_root_max_workers = config.parallel_root_max_workers;

    std::uint64_t total_nodes = 0;
    std::uint64_t total_tt_hits = 0;
    std::uint64_t total_cutoffs = 0;
    std::chrono::microseconds total_elapsed{0};
    othello::engine::SearchResult last_result{};

    for (int repeat = 0; repeat < config.repeats; ++repeat)
    {
        const othello::board::Board board = make_board_for_scenario(config.scenario);
        last_result = othello::engine::find_best_move(board, options);
        total_nodes += last_result.stats.nodes_searched;
        total_tt_hits += last_result.stats.tt_hits;
        total_cutoffs += last_result.stats.beta_cutoffs;
        total_elapsed += last_result.stats.elapsed;
    }

    const double average_ms = static_cast<double>(total_elapsed.count()) /
                              static_cast<double>(config.repeats) /
                              1000.0;
    const double nodes_per_second =
        total_elapsed.count() == 0
            ? 0.0
            : (static_cast<double>(total_nodes) * 1'000'000.0) /
                  static_cast<double>(total_elapsed.count());

    std::cout << "Scenario: " << config.scenario << '\n';
    std::cout << "Depth: " << config.depth << '\n';
    std::cout << "Repeats: " << config.repeats << '\n';
    std::cout << "Parallel root: " << (config.parallel_root ? "yes" : "no") << '\n';
    if (config.parallel_root)
    {
        std::cout << "Parallel root min depth: " << config.parallel_root_min_depth << '\n';
        std::cout << "Parallel root min moves: " << config.parallel_root_min_moves << '\n';
        std::cout << "Parallel root max workers: ";
        if (config.parallel_root_max_workers == 0)
        {
            std::cout << "auto\n";
        }
        else
        {
            std::cout << config.parallel_root_max_workers << '\n';
        }
    }

    if (config.node_limit.has_value())
    {
        std::cout << "Node limit: " << *config.node_limit << '\n';
    }
    if (config.time_limit.has_value())
    {
        std::cout << "Time limit (ms): " << config.time_limit->count() << '\n';
    }
    std::cout << "Completed requested depth: " << (last_result.completed ? "yes" : "no") << '\n';
    std::cout << "Last completed depth: " << last_result.stats.completed_depth << '\n';
    std::cout << "Best move: " << format_move(last_result.best_move) << '\n';
    std::cout << "Score: " << last_result.score << '\n';
    std::cout << "Average elapsed (ms): " << average_ms << '\n';
    std::cout << "Average nodes: " << (config.repeats == 0 ? 0 : total_nodes / static_cast<std::uint64_t>(config.repeats)) << '\n';
    std::cout << "Average TT hits: " << (config.repeats == 0 ? 0 : total_tt_hits / static_cast<std::uint64_t>(config.repeats)) << '\n';
    std::cout << "Average beta cutoffs: " << (config.repeats == 0 ? 0 : total_cutoffs / static_cast<std::uint64_t>(config.repeats)) << '\n';
    std::cout << "Nodes per second: " << nodes_per_second << '\n';

    return 0;
}
