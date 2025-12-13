#include "board.hpp"

Board::Board() : boardSize(8) { 
    std::cout << "8x8 Othello board successfully created!" << std::endl;
}

Board::Board(int boardSize) : boardSize(boardSize) {
    std::cout << boardSize << "x" << boardSize << " Othello board successfully created!" << std::endl;
}

void Board::horizontalDivider() {
    for (int i = 0; i < ((boardSize + 1)*4 + 1); i++) {
        std::cout << "-";
    }
    std::cout << std::endl;
}

void Board::firstNumberingRow() {
    std::cout << "|   ";
    for (int i = 1; i <= boardSize; i++) {
        std::cout << "| " << char(i + 64) << " ";
    }
    std::cout << "|" << std::endl;
}

void Board::numberedGridRows() {
    for (int i = 1; i <= boardSize; i++) {
        Board::horizontalDivider();
        std::cout << "| " << i << " ";
        for (int i = 1; i <= boardSize; i++) {
            std::cout << "|   ";
        }
        std::cout << "|" << std::endl;
    }
}

void Board::printBoard() {
    Board::horizontalDivider();
    Board::firstNumberingRow();
    Board::numberedGridRows();
    Board::horizontalDivider();
}

Board::~Board() {
    std::cout << "Board discarded!" << std::endl;
}
