// train_ntuple – two-phase (10+90)-ES training.
//
//  Phase 1  ES vs SWH    — stops at --target vs SWH or --es-games budget (0 = unlimited)
//  Phase 2  ES vs self   — evolves against a frozen snapshot of phase-1 best
//                          stops at --target vs SWH or --self-games budget (0 = unlimited)
//
// Prints win rate and saves a checkpoint every 100 eval games.
//
// Usage: train_ntuple [--out PATH] [--seed N] [--target F]
//                    [--es-games N] [--self-games N]

#include "ntuple/ntuple.h"
#include "eval/swh_eval.h"
#include "othello/board.h"
#include "othello/game_result.h"
#include "othello/rules.h"
#include "othello/types.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

namespace {

using EvalFn = std::function<float(const Board&, Player)>;

constexpr float kEps        = 0.10f;
constexpr int   kMu         = 10;
constexpr int   kLambda     = 90;
constexpr int   kEsEval     = 100;                  // eval games per individual (paper value)
constexpr int   kPrintEvery = kLambda * kEsEval;    // one print per generation
constexpr float kDefaultTarget = 0.99f;             // default win-rate vs SWH to stop early
constexpr int   kSwhEval    = 20;                   // quick vs-SWH check in phase 2

// ── Primitives ────────────────────────────────────────────────────────────────

Move pick_move(const Board& board, Player player, const EvalFn& eval,
               float eps, std::mt19937& rng) {
    const auto moves = Rules::get_all_valid_moves(board, player);
    if (moves.empty()) return Move{{-1, -1}};
    if (eps > 0.f && std::bernoulli_distribution{eps}(rng))
        return moves[std::uniform_int_distribution<int>{0, (int)moves.size() - 1}(rng)];
    Move best = moves[0];
    float best_val = -std::numeric_limits<float>::infinity();
    for (const auto& m : moves) {
        Board next = board.clone();
        Rules::apply_move(next, player, m);
        const float v = eval(next, player);
        if (v > best_val) { best_val = v; best = m; }
    }
    return best;
}

int play_game(const EvalFn& hero, const EvalFn& villain, Player hero_color,
              float eps, std::mt19937& rng) {
    Board board = Board::standard_start();
    Player player = Player::BLACK;
    for (;;) {
        if (!Rules::has_valid_move(board, player)) {
            if (!Rules::has_valid_move(board, opponent(player))) break;
            player = opponent(player); continue;
        }
        const EvalFn& eval = (player == hero_color) ? hero : villain;
        Rules::apply_move(board, player, pick_move(board, player, eval, eps, rng));
        player = opponent(player);
    }
    const auto out = outcome_from_board(board);
    if (!out.winner) return 0;
    return *out.winner == hero_color ? 1 : -1;
}

// net vs opponent (SWH when opponent_net==nullptr, else that net), alternating colors
float win_rate(const NTuple::Net& net, const NTuple::Net* opponent_net,
               int games, std::mt19937& rng) {
    SwhEvaluator swh;
    const EvalFn net_fn  = [&net](const Board& b, Player p) { return net.evaluate(b, p); };
    const EvalFn opp_fn  = opponent_net
        ? EvalFn([opponent_net](const Board& b, Player p) { return opponent_net->evaluate(b, p); })
        : EvalFn([&swh](const Board& b, Player p) { return static_cast<float>(swh.evaluate(b, p)); });

    float score = 0.f;
    for (int i = 0; i < games; ++i) {
        const Player color = (i % 2 == 0) ? Player::BLACK : Player::WHITE;
        const int result = play_game(net_fn, opp_fn, color, kEps, rng);
        score += result > 0 ? 1.f : result == 0 ? 0.5f : 0.f;
    }
    return score / games;
}

// ── ES with game budget ───────────────────────────────────────────────────────
//
// opponent_net == nullptr  →  Phase 1: evolve vs SWH
//                              printed metric = ES fitness (IS win rate vs SWH)
// opponent_net != nullptr  →  Phase 2: evolve vs frozen self snapshot
//                              printed metric = win rate vs SWH (quick separate eval)
//
// Prints and checkpoints to out_path every kPrintEvery total eval games.
// Returns early if win rate vs SWH >= target.
// game_budget <= 0 means unlimited (run until target).

NTuple::Net run_es(int game_budget, float target, const NTuple::Net* opponent_net,
                    const std::string& tag, const std::string& out_path,
                    std::mt19937& rng) {
    constexpr int MU = kMu, LAMBDA = kLambda;

    auto make_random = [&] {
        NTuple::Net n;
        std::uniform_real_distribution<float> d{-0.1f, 0.1f};
        for (auto& lut : n.w) for (auto& w : lut) w = d(rng);
        return n;
    };
    auto mutate = [&](NTuple::Net n) {
        std::normal_distribution<float> d{0.f, 1.f};
        for (auto& lut : n.w) for (auto& w : lut) w += d(rng);
        return n;
    };
    auto eval_individual = [&](const NTuple::Net& ind) {
        return win_rate(ind, opponent_net, kEsEval, rng);
    };
    auto vs_swh_quick = [&](const NTuple::Net& ind) {
        std::mt19937 r{42};
        return (opponent_net == nullptr) ? 0.f   // already known: fitness = vs_swh
             : win_rate(ind, nullptr, kSwhEval, r);
    };

    std::vector<NTuple::Net> pop(MU);
    std::vector<float>       fit(MU);
    for (auto& ind : pop) ind = make_random();

    int total = 0;
    int next_print = kPrintEvery;

    // Evaluate initial population
    for (int i = 0; i < MU; ++i) {
        fit[i] = eval_individual(pop[i]);
        total += kEsEval;
    }

    auto best_idx = [&] {
        return static_cast<int>(
            std::max_element(fit.begin(), fit.end()) - fit.begin());
    };

    // Print + checkpoint helper; returns true if target reached
    auto report = [&]() -> bool {
        if (total < next_print) return false;
        next_print = ((total / kPrintEvery) + 1) * kPrintEvery;

        const int   bi    = best_idx();
        const float bf    = fit[bi];
        const float vs_sw = (opponent_net == nullptr)
                            ? bf
                            : vs_swh_quick(pop[bi]);
        std::cout << "[" << tag << "] games=" << total
                  << " fitness=" << bf
                  << " vs_swh=" << vs_sw << '\n' << std::flush;

        pop[bi].save(out_path);

        if (vs_sw >= target) {
            std::cout << "[" << tag << "] target " << target << " reached\n";
            return true;
        }
        return false;
    };

    if (report()) return pop[best_idx()];

    for (;;) {
        if (game_budget > 0 && total >= game_budget) break;

        // Generate and evaluate offspring
        std::vector<NTuple::Net> offspring(LAMBDA);
        std::uniform_int_distribution<int> pick{0, MU - 1};
        for (auto& off : offspring) off = mutate(pop[pick(rng)]);

        std::vector<float> off_fit(LAMBDA);
        for (int i = 0; i < LAMBDA; ++i) {
            off_fit[i] = eval_individual(offspring[i]);
            total += kEsEval;
            if (report()) {
                // find best across current pop + evaluated offspring so far
                float best_fit = *std::max_element(fit.begin(), fit.end());
                int   best_i   = best_idx();
                for (int j = 0; j <= i; ++j)
                    if (off_fit[j] > best_fit) { best_fit = off_fit[j]; best_i = MU + j; }
                return best_i < MU ? pop[best_i] : offspring[best_i - MU];
            }
            if (game_budget > 0 && total >= game_budget) {
                // Budget hit mid-generation: flush & return best so far
                // (partial generation still contributes to selection below)
                goto selection;
            }
        }

    selection:
        // (μ+λ) selection
        using Ranked = std::pair<float, int>;
        std::vector<Ranked> pool;
        pool.reserve(MU + LAMBDA);
        for (int i = 0; i < MU; ++i)     pool.push_back({fit[i],     i});
        for (int i = 0; i < LAMBDA; ++i) pool.push_back({off_fit[i], MU + i});
        std::sort(pool.begin(), pool.end(),
                  [](const Ranked& a, const Ranked& b) { return a.first > b.first; });

        for (int i = 0; i < MU; ++i) {
            const int idx = pool[i].second;
            pop[i] = idx < MU ? pop[idx] : offspring[idx - MU];
            fit[i] = pool[i].first;
        }
    }

    const int bi = best_idx();
    if (game_budget > 0)
        std::cout << "[" << tag << "] budget exhausted games=" << total
                  << " best_fitness=" << fit[bi] << '\n';
    return pop[bi];
}

// ── CLI ───────────────────────────────────────────────────────────────────────

struct Options {
    std::string out        = "ntuple.bin";
    unsigned    seed       = std::random_device{}();
    float       target     = kDefaultTarget;
    int         es_games   = 0;   // 0 = unlimited until --target
    int         self_games = 0;   // 0 = unlimited until --target
};

Options parse(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        const char* nxt = (i + 1 < argc) ? argv[i + 1] : "";
        if      (arg == "--out"        && *nxt) { opt.out        = nxt;                                    ++i; }
        else if (arg == "--seed"       && *nxt) { opt.seed       = static_cast<unsigned>(std::atoi(nxt)); ++i; }
        else if (arg == "--target"     && *nxt) { opt.target     = std::atof(nxt);                        ++i; }
        else if (arg == "--es-games"   && *nxt) { opt.es_games   = std::atoi(nxt);                        ++i; }
        else if (arg == "--self-games" && *nxt) { opt.self_games = std::atoi(nxt);                        ++i; }
        else if (arg == "-h" || arg == "--help") {
            std::cout
                << "Usage: train_ntuple [--out PATH] [--seed N] [--target F]\n"
                << "                   [--es-games N] [--self-games N]\n"
                << "  --target     win-rate vs SWH to stop each phase (default 0.80)\n"
                << "  --es-games   eval-game budget for ES vs SWH   (default 0 = unlimited)\n"
                << "  --self-games eval-game budget for ES vs self  (default 0 = unlimited)\n"
                << "Each phase stops when vs-SWH win rate >= --target, or when its budget is reached.\n"
                << "Budget 0 = unlimited until --target. Set --self-games -1 to skip phase 2.\n";
            std::exit(0);
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            std::exit(1);
        }
    }
    return opt;
}

} // namespace

int main(int argc, char** argv) {
    const Options opt = parse(argc, argv);
    std::mt19937 rng{opt.seed};
    std::cout << "seed=" << opt.seed << '\n';

    const auto budget_str = [](int b) {
        return b > 0 ? std::to_string(b) : std::string("unlimited");
    };

    // ── Phase 1: ES vs SWH ───────────────────────────────────────────────────
    std::cout << "\n=== Phase 1: ES vs SWH"
              << "  budget=" << budget_str(opt.es_games)
              << "  target=" << opt.target << " ===\n\n";

    NTuple::Net best = run_es(opt.es_games, opt.target, nullptr, "ES", opt.out, rng);

    // ── Phase 2: ES vs self snapshot ─────────────────────────────────────────
    if (opt.self_games >= 0) {
        std::cout << "\n=== Phase 2: ES vs self"
                  << "  budget=" << budget_str(opt.self_games)
                  << "  target=" << opt.target << " ===\n\n";

        const NTuple::Net snapshot = best;   // frozen — doesn't update during phase 2
        best = run_es(opt.self_games, opt.target, &snapshot, "SELF", opt.out, rng);
    }

    if (best.save(opt.out))
        std::cout << "\nfinal weights saved → " << opt.out << '\n';
    else
        std::cerr << "failed to save " << opt.out << '\n';

    return 0;
}
