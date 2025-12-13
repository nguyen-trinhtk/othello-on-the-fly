#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <string>

#include "board.hpp"
using namespace std;


class Board {
    // attributes
    private: 
        int boardSize; // TODO: must be divisible by 2
        std::vector<std::vector<std::string> > boardMatrix; // should it be std::string?
        bool whitesTurn; // alternate
        const std::string NOTHING = " ";
        const std::string BLACK = "●"; // TODO: replace later with Unicode
        const std::string WHITE = "○";

    // method
    public: 
        Board();
        Board(int boardSize);
        void clearBoard();
        void initialSetup();
        void horizontalDivider();
        void firstNumberingRow();
        void renderRows();
        void printBoard();
        void place(int r, int c, string checker); // 1-indexed
        void startGame();
        // TODO: code parser, e.g.: move("3A") -> row 3, column A
        ~Board();
};

#endif