#include "board.hpp"

Board::Board() : boardSize(8) { 
    clearBoard();
    std::cout << "8x8 Othello board successfully created!" << std::endl;
}

Board::Board(int boardSize) : boardSize(boardSize) {
    clearBoard();
    std::cout << boardSize << "x" << boardSize << " Othello board successfully created!" << std::endl;
}

void Board::clearBoard() {
    for (int i = 0; i < boardSize; i++) {
        std::vector<char> row;
        for (int j = 0; j < boardSize; j++) {
            row.push_back(' ');
        }
        boardMatrix.push_back(row);
    }
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

void Board::renderRows() {
    for (int i = 1; i <= boardSize; i++) {
        Board::horizontalDivider();
        std::cout << "| " << i << " ";
        for (int j = 1; j <= boardSize; j++) {
            std::cout << "| " << boardMatrix[i - 1][j - 1] << " ";
            // std::cout << "|   ";
        }
        std::cout << "|" << std::endl;
    }
}

void Board::printBoard() {
    Board::horizontalDivider();
    Board::firstNumberingRow();
    Board::renderRows();
    Board::horizontalDivider();
}

Board::~Board() {
    std::cout << "Board discarded!" << std::endl;
}
