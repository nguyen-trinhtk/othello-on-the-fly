#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <utility>

#include "engine_internal.hpp"

namespace othello
{
    namespace engine
    {
        namespace detail
        {
            [[nodiscard]] std::size_t parallel_root_worker_count(
                const SearchOptions &options,
                std::size_t remaining_root_moves) noexcept
            {
                if (remaining_root_moves == 0)
                {
                    return 0;
                }

                const unsigned int hardware_threads = std::thread::hardware_concurrency();
                const std::size_t hardware_limit =
                    hardware_threads == 0 ? 1U : static_cast<std::size_t>(hardware_threads);
                const std::size_t configured_limit =
                    options.parallel_root_max_workers <= 0
                        ? hardware_limit
                        : static_cast<std::size_t>(options.parallel_root_max_workers);

                return std::min(remaining_root_moves, std::min(hardware_limit, configured_limit));
            }

            [[nodiscard]] std::size_t parallel_root_batch_size(
                const SearchOptions &options,
                std::size_t remaining_root_moves) noexcept
            {
                const std::size_t worker_count =
                    parallel_root_worker_count(options, remaining_root_moves);
                if (worker_count == 0)
                {
                    return 0;
                }

                const std::size_t batch_scale =
                    options.parallel_root_batch_scale <= 1
                        ? 1U
                        : static_cast<std::size_t>(options.parallel_root_batch_scale);

                return std::min(remaining_root_moves, worker_count * batch_scale);
            }

            namespace
            {
                [[nodiscard]] SearchOptions make_parallel_root_worker_options(const SearchOptions &options)
                {
                    SearchOptions worker_options = options;
                    worker_options.iterative_deepening = false;
                    worker_options.parallel_root = false;
                    worker_options.time_limit.reset();
                    worker_options.node_limit.reset();
                    return worker_options;
                }

                struct ParallelRootWorkerConfig
                {
                    std::size_t transposition_table_size = 0;
                };

                [[nodiscard]] ParallelRootWorkerConfig make_parallel_root_worker_config(
                    const SearchOptions &options) noexcept
                {
                    return {
                        options.transposition_table_size,
                    };
                }

                struct ParallelRootWorkerState
                {
                    explicit ParallelRootWorkerState(const SearchOptions &search_options)
                        : options(make_parallel_root_worker_options(search_options)),
                          context(options)
                    {
                    }

                    SearchOptions options;
                    SearchContext context;
                    othello::board::Board board{};
                };

                void reset_parallel_root_worker_state(
                    ParallelRootWorkerState &worker,
                    const othello::board::Board &root_board)
                {
                    worker.board = root_board;
                    worker.context.stats = {};
                    worker.context.start_time = Clock::now();
                    worker.context.aborted = false;
                    worker.context.killer_moves = {};
                    worker.context.history_scores = {};
                    worker.context.transposition_table.new_search();
                }

                [[nodiscard]] RootMoveResult evaluate_root_move_with_worker(
                    ParallelRootWorkerState &worker,
                    const othello::board::Board &root_board,
                    const othello::board::Move &candidate,
                    Disc perspective_player,
                    int depth,
                    int alpha,
                    int beta,
                    std::size_t move_order)
                {
                    reset_parallel_root_worker_state(worker, root_board);

                    RootMoveResult result{};
                    result.move = candidate;
                    result.move_order = move_order;

                    if (worker.board.process_move(candidate, perspective_player) != OK)
                    {
                        result.score = worker.board.evaluate(perspective_player);
                        result.completed = false;
                        return result;
                    }

                    worker.board.set_current_player(opponent(perspective_player));
                    result.score = search_subtree(
                        worker.board,
                        std::max(0, depth - 1),
                        alpha,
                        beta,
                        perspective_player,
                        worker.context,
                        1);

                    result.stats = worker.context.stats;
                    result.completed = !worker.context.aborted;
                    return result;
                }

                class ParallelRootThreadPool
                {
                public:
                    ParallelRootThreadPool()
                    {
                        const unsigned int hardware_threads = std::thread::hardware_concurrency();
                        pool_size_ =
                            hardware_threads == 0 ? 1U : static_cast<std::size_t>(hardware_threads);

                        workers_.reserve(pool_size_);
                        for (std::size_t worker_index = 0; worker_index < pool_size_; ++worker_index)
                        {
                            workers_.emplace_back([this, worker_index]()
                                                  { worker_loop(worker_index); });
                        }
                    }

                    ~ParallelRootThreadPool()
                    {
                        {
                            std::lock_guard<std::mutex> lock(mutex_);
                            stopping_ = true;
                        }
                        work_available_.notify_all();

                        for (auto &worker : workers_)
                        {
                            worker.join();
                        }
                    }

                    void run_batch(
                        std::vector<RootMoveResult> &results,
                        const othello::board::Board &root_board,
                        const std::vector<othello::board::Move> &moves,
                        Disc perspective_player,
                        int depth,
                        int alpha,
                        int beta,
                        const SearchOptions &options,
                        std::size_t first_parallel_move,
                        std::size_t batch_size,
                        std::size_t worker_count)
                    {
                        if (worker_count == 0)
                        {
                            return;
                        }

                        std::unique_lock<std::mutex> lock(mutex_);
                        batch_finished_.wait(lock, [this]()
                                             { return !batch_in_flight_; });

                        ensure_worker_states(options, worker_count);

                        batch_results_ = &results;
                        batch_root_board_ = &root_board;
                        batch_moves_ = &moves;
                        batch_perspective_player_ = perspective_player;
                        batch_depth_ = depth;
                        batch_alpha_ = alpha;
                        batch_beta_ = beta;
                        batch_first_parallel_move_ = first_parallel_move;
                        batch_remaining_root_moves_ = batch_size;
                        batch_active_workers_ = worker_count;
                        batch_finished_workers_ = 0;
                        batch_in_flight_ = true;
                        next_work_item_.store(0, std::memory_order_relaxed);
                        ++batch_generation_;

                        lock.unlock();
                        work_available_.notify_all();

                        lock.lock();
                        batch_finished_.wait(lock, [this]()
                                             { return !batch_in_flight_; });
                    }

                private:
                    void ensure_worker_states(
                        const SearchOptions &options,
                        std::size_t worker_count)
                    {
                        const ParallelRootWorkerConfig worker_config =
                            make_parallel_root_worker_config(options);
                        if (!worker_states_initialized_ ||
                            worker_state_config_.transposition_table_size !=
                                worker_config.transposition_table_size)
                        {
                            worker_states_.clear();
                            worker_state_config_ = worker_config;
                            worker_states_initialized_ = true;
                        }

                        if (worker_states_.size() >= worker_count)
                        {
                            return;
                        }

                        worker_states_.reserve(worker_count);
                        for (std::size_t worker = worker_states_.size();
                             worker < worker_count;
                             ++worker)
                        {
                            worker_states_.emplace_back(options);
                        }
                    }

                    void worker_loop(std::size_t worker_index)
                    {
                        std::size_t observed_generation = 0;

                        while (true)
                        {
                            std::size_t generation = 0;
                            {
                                std::unique_lock<std::mutex> lock(mutex_);
                                work_available_.wait(lock, [this, observed_generation]()
                                                     { return stopping_ || batch_generation_ != observed_generation; });

                                if (stopping_)
                                {
                                    return;
                                }

                                generation = batch_generation_;
                                if (worker_index >= batch_active_workers_)
                                {
                                    observed_generation = generation;
                                    continue;
                                }
                            }

                            while (true)
                            {
                                const std::size_t work_item =
                                    next_work_item_.fetch_add(1, std::memory_order_relaxed);
                                if (work_item >= batch_remaining_root_moves_)
                                {
                                    break;
                                }

                                const std::size_t move_order = batch_first_parallel_move_ + work_item;
                                (*batch_results_)[work_item] = evaluate_root_move_with_worker(
                                    worker_states_[worker_index],
                                    *batch_root_board_,
                                    (*batch_moves_)[move_order],
                                    batch_perspective_player_,
                                    batch_depth_,
                                    batch_alpha_,
                                    batch_beta_,
                                    move_order);
                            }

                            {
                                std::lock_guard<std::mutex> lock(mutex_);
                                observed_generation = generation;
                                ++batch_finished_workers_;
                                if (batch_finished_workers_ == batch_active_workers_)
                                {
                                    batch_in_flight_ = false;
                                    batch_finished_.notify_one();
                                }
                            }
                        }
                    }

                    std::mutex mutex_;
                    std::condition_variable work_available_;
                    std::condition_variable batch_finished_;
                    bool stopping_ = false;

                    std::vector<RootMoveResult> *batch_results_ = nullptr;
                    const othello::board::Board *batch_root_board_ = nullptr;
                    const std::vector<othello::board::Move> *batch_moves_ = nullptr;
                    Disc batch_perspective_player_ = EMPTY;
                    int batch_depth_ = 0;
                    int batch_alpha_ = ALPHABETA_MIN;
                    int batch_beta_ = ALPHABETA_MAX;
                    std::size_t batch_first_parallel_move_ = 0;
                    std::size_t batch_remaining_root_moves_ = 0;
                    std::size_t batch_active_workers_ = 0;
                    std::size_t batch_finished_workers_ = 0;
                    std::size_t batch_generation_ = 0;
                    bool batch_in_flight_ = false;
                    std::atomic<std::size_t> next_work_item_{0};

                    std::size_t pool_size_ = 0;
                    std::vector<ParallelRootWorkerState> worker_states_{};
                    ParallelRootWorkerConfig worker_state_config_{};
                    bool worker_states_initialized_ = false;
                    std::vector<std::thread> workers_;
                };

                ParallelRootThreadPool &parallel_root_thread_pool()
                {
                    static ParallelRootThreadPool pool;
                    return pool;
                }
            } // namespace

            void evaluate_parallel_root_batch(
                std::vector<RootMoveResult> &results,
                const othello::board::Board &root_board,
                const std::vector<othello::board::Move> &moves,
                Disc perspective_player,
                int depth,
                int alpha,
                int beta,
                const SearchOptions &options,
                std::size_t first_parallel_move,
                std::size_t batch_size)
            {
                const std::size_t worker_count = parallel_root_worker_count(options, batch_size);
                if (worker_count == 0 || batch_size == 0)
                {
                    return;
                }

                parallel_root_thread_pool().run_batch(
                    results,
                    root_board,
                    moves,
                    perspective_player,
                    depth,
                    alpha,
                    beta,
                    options,
                    first_parallel_move,
                    batch_size,
                    worker_count);
            }
        } // namespace detail
    } // namespace engine
} // namespace othello
