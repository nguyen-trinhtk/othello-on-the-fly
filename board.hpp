#ifndef BOARD_H
#define BOARD_H

#include <iostream>

#include "board.hpp"


class Board {
    // attributes
    private: 
        int boardSize;
        // const char black = 'b'; // TODO: replace later with Unicode
        // const char white = 'w';

    // method
    public: 
        Board();
        Board(int boardSize);
        void horizontalDivider();
        void firstNumberingRow();
        void numberedGridRows();
        void printBoard();
        ~Board();
};

#endif