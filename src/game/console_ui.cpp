#include "game/console_ui.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    using othello::board::Board;
    using othello::board::Move;
    using othello::game_ui::CommandType;
    using othello::game_ui::ParsedCommand;

    [[nodiscard]] std::string trim(std::string_view input)
    {
        const auto start = input.find_first_not_of(" \t\r\n");
        if (start == std::string_view::npos)
        {
            return {};
        }

        const auto end = input.find_last_not_of(" \t\r\n");
        return std::string(input.substr(start, end - start + 1));
    }

    [[nodiscard]] std::string collapse_spaces(std::string_view input)
    {
        std::string normalized;
        normalized.reserve(input.size());
        for (const char ch : input)
        {
            if (!std::isspace(static_cast<unsigned char>(ch)))
            {
                normalized.push_back(ch);
            }
        }
        return normalized;
    }

    [[nodiscard]] std::string to_lower_copy(std::string value)
    {
        std::transform(
            value.begin(),
            value.end(),
            value.begin(),
            [](unsigned char ch)
            { return static_cast<char>(std::tolower(ch)); });
        return value;
    }

    [[nodiscard]] std::string to_upper_copy(std::string value)
    {
        std::transform(
            value.begin(),
            value.end(),
            value.begin(),
            [](unsigned char ch)
            { return static_cast<char>(std::toupper(ch)); });
        return value;
    }

    [[nodiscard]] std::string_view player_name(Disc player) noexcept
    {
        return player == BLACK ? "Black" : "White";
    }

    [[nodiscard]] std::string_view disc_symbol(Disc disc) noexcept
    {
        switch (disc)
        {
        case BLACK:
            return BLACK_DISC;
        case WHITE:
            return WHITE_DISC;
        case EMPTY:
            return EMPTY_SQUARE;
        default:
            return INVALID_SQUARE;
        }
    }

    [[nodiscard]] std::string format_move(const Move &move)
    {
        std::string formatted;
        formatted.push_back(static_cast<char>('A' + move.col));
        formatted.append(std::to_string(move.row + 1));
        return formatted;
    }

    [[nodiscard]] std::vector<Move> sorted_valid_moves(const Board &board)
    {
        std::vector<Move> moves(board.get_valid_moves().begin(), board.get_valid_moves().end());
        std::sort(
            moves.begin(),
            moves.end(),
            [](const Move &lhs, const Move &rhs)
            {
                if (lhs.row != rhs.row)
                {
                    return lhs.row < rhs.row;
                }
                return lhs.col < rhs.col;
            });
        return moves;
    }
} // namespace

namespace othello
{
    namespace game_ui
    {

        ParsedCommand parse_command(std::string_view input)
        {
            const std::string trimmed = trim(input);
            if (trimmed.empty())
            {
                return {CommandType::invalid, std::nullopt, "Enter a move like D3 or a command like help."};
            }

            const std::string lower = to_lower_copy(trimmed);
            if (lower == "help" || lower == "h")
            {
                return {CommandType::help, std::nullopt, {}};
            }
            if (lower == "board" || lower == "b")
            {
                return {CommandType::board, std::nullopt, {}};
            }
            if (lower == "moves" || lower == "m")
            {
                return {CommandType::moves, std::nullopt, {}};
            }
            if (lower == "score" || lower == "s")
            {
                return {CommandType::score, std::nullopt, {}};
            }
            if (lower == "quit" || lower == "q" || lower == "exit")
            {
                return {CommandType::quit, std::nullopt, {}};
            }

            const std::string compact = to_upper_copy(collapse_spaces(trimmed));
            if (compact.length() != 2)
            {
                return {CommandType::invalid, std::nullopt, MSG_ERR_INVALID_MOVE_FORMAT};
            }
            if (!std::isalpha(static_cast<unsigned char>(compact[0])) ||
                !std::isdigit(static_cast<unsigned char>(compact[1])))
            {
                return {CommandType::invalid, std::nullopt, MSG_ERR_INVALID_MOVE_FORMAT};
            }
            if ((compact[0] < 'A' || compact[0] > 'H') || (compact[1] < '1' || compact[1] > '8'))
            {
                return {CommandType::invalid, std::nullopt, MSG_ERR_OUT_OF_BOUNDS};
            }

            return {
                CommandType::move,
                Move(compact[1] - '1', compact[0] - 'A'),
                {},
            };
        }

        std::string render_board(const Board &board, bool show_valid_moves)
        {
            std::ostringstream out;
            out << "\n"
                << "Turn: " << player_name(board.get_current_player()) << "\n"
                << "Score  Black " << board.get_black_count() << "  White " << board.get_white_count() << "\n\n"
                << "     A   B   C   D   E   F   G   H\n"
                << "   +---+---+---+---+---+---+---+---+\n";

            for (int row = 0; row < BOARD_SIZE; ++row)
            {
                out << ' ' << (row + 1) << " |";
                for (int col = 0; col < BOARD_SIZE; ++col)
                {
                    std::string_view cell = disc_symbol(board.get_square(row, col));
                    if (show_valid_moves &&
                        board.get_square(row, col) == EMPTY &&
                        board.is_valid_move(row, col, board.get_current_player()))
                    {
                        cell = "·";
                    }
                    out << ' ' << cell << " |";
                }
                out << " " << (row + 1) << "\n"
                    << "   +---+---+---+---+---+---+---+---+\n";
            }
            out << "     A   B   C   D   E   F   G   H\n";
            if (show_valid_moves)
            {
                out << "Hint: " << "·" << " marks valid moves for " << player_name(board.get_current_player()) << ".\n";
            }
            return out.str();
        }

        std::string render_score_summary(const Board &board)
        {
            std::ostringstream out;
            const int black_count = board.get_black_count();
            const int white_count = board.get_white_count();
            out << "Final Score: Black = " << black_count << ", White = " << white_count << "\n";
            if (black_count > white_count)
            {
                out << "Black wins!\n";
            }
            else if (white_count > black_count)
            {
                out << "White wins!\n";
            }
            else
            {
                out << "It's a draw!\n";
            }
            return out.str();
        }

        std::string render_valid_moves(const Board &board)
        {
            std::ostringstream out;
            const auto moves = sorted_valid_moves(board);
            out << player_name(board.get_current_player()) << " valid moves:";
            if (moves.empty())
            {
                out << " none\n";
                return out.str();
            }

            for (const auto &move : moves)
            {
                out << ' ' << format_move(move);
            }
            out << '\n';
            return out.str();
        }

        std::string render_help()
        {
            std::ostringstream out;
            out << "Commands:\n"
                << "  D3      Play a move\n"
                << "  moves   Show legal moves\n"
                << "  board   Reprint the board\n"
                << "  score   Show the current score\n"
                << "  help    Show this help\n"
                << "  quit    Exit the current game\n";
            return out.str();
        }

        ConsoleUi::ConsoleUi(std::istream &in, std::ostream &out, std::ostream &err)
            : in_(in), out_(out), err_(err)
        {
        }

        void ConsoleUi::show_board(const Board &board, bool show_valid_moves) const
        {
            // Clear the terminal and move cursor to top-left
            out_ << "\033[2J\033[H";
            out_ << render_board(board, show_valid_moves);
        }

        void ConsoleUi::show_score_summary(const Board &board) const
        {
            out_ << render_score_summary(board);
        }

        void ConsoleUi::show_valid_moves(const Board &board) const
        {
            out_ << render_valid_moves(board);
        }

        void ConsoleUi::show_help() const
        {
            out_ << render_help();
        }

        void ConsoleUi::show_message(std::string_view message) const
        {
            out_ << message << '\n';
        }

        std::optional<board::Move> ConsoleUi::prompt_for_move(const Board &board) const
        {
            show_message(std::string(player_name(board.get_current_player())) + " to move. Enter a square or command:");
            while (true)
            {
                out_ << "> " << std::flush;

                std::string line;
                if (!std::getline(in_, line))
                {
                    return std::nullopt;
                }

                const ParsedCommand command = parse_command(line);
                switch (command.type)
                {
                case CommandType::help:
                    show_help();
                    break;
                case CommandType::board:
                    show_board(board);
                    break;
                case CommandType::moves:
                    show_valid_moves(board);
                    break;
                case CommandType::score:
                    show_message(
                        "Score  Black " + std::to_string(board.get_black_count()) +
                        "  White " + std::to_string(board.get_white_count()));
                    break;
                case CommandType::quit:
                    show_message("Ending the current game.");
                    return std::nullopt;
                case CommandType::invalid:
                    err_ << command.error_message << '\n';
                    break;
                case CommandType::move:
                {
                    auto move = *command.move;
                    if (!board.is_valid_move(move.row, move.col, board.get_current_player()))
                    {
                        if (board.get_square(move.row, move.col) != EMPTY)
                        {
                            err_ << MSG_ERR_SQUARE_OCCUPIED << '\n';
                        }
                        else
                        {
                            err_ << MSG_ERR_INVALID_MOVE << '\n';
                        }
                        break;
                    }

                    show_message(
                        std::string(player_name(board.get_current_player())) +
                        " played " + format_move(move) + ".");
                    return move;
                }
                }
            }
        }

    } // namespace game_ui
} // namespace othello
