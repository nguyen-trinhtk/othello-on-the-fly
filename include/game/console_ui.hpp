#ifndef CONSOLE_UI_HPP
#define CONSOLE_UI_HPP

#include <istream>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

#include "board/board.hpp"

namespace othello
{
    namespace game_ui
    {

        enum class CommandType
        {
            move,
            help,
            board,
            moves,
            score,
            quit,
            invalid,
        };

        struct ParsedCommand
        {
            CommandType type = CommandType::invalid;
            std::optional<board::Move> move;
            std::string error_message;
        };

        [[nodiscard]] ParsedCommand parse_command(std::string_view input);
        [[nodiscard]] std::string render_board(const board::Board &board, bool show_valid_moves = true);
        [[nodiscard]] std::string render_score_summary(const board::Board &board);
        [[nodiscard]] std::string render_valid_moves(const board::Board &board);
        [[nodiscard]] std::string render_help();

        class ConsoleUi
        {
        public:
            ConsoleUi(std::istream &in, std::ostream &out, std::ostream &err, bool enable_screen_clear = false);

            void show_board(const board::Board &board, bool show_valid_moves = true) const;
            void show_score_summary(const board::Board &board) const;
            void show_valid_moves(const board::Board &board) const;
            void show_help() const;
            void show_message(std::string_view message) const;
            [[nodiscard]] std::optional<board::Move> prompt_for_move(const board::Board &board) const;

        private:
            std::istream &in_;
            std::ostream &out_;
            std::ostream &err_;
            bool enable_screen_clear_;
        };

    } // namespace game_ui
} // namespace othello

#endif
