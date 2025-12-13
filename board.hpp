#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <string>

#include "board.hpp"


class Board {
    // attributes
    private: 
        int boardSize;
        std::vector<std::vector<char> > boardMatrix; // should it be char?
        const char blank = ' ';
        const char black = 'b'; // TODO: replace later with Unicode
        const char white = 'w';

    // method
    public: 
        Board();
        Board(int boardSize);
        void clearBoard();
        void horizontalDivider();
        void firstNumberingRow();
        void renderRows();
        void printBoard();
        ~Board();
};

#endif