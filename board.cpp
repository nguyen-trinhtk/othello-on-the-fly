#include <iostream>
#include "board.hpp"
// using namespace std;

void horizontalDivider(int n) {
    for (int i = 0; i < ((n + 1)*4 + 1); i++) {
            std::cout << "-";
    }
    std::cout << std::endl;
}

void firstNumberingRow(int n) {
    std::cout << "|   ";
    for (int i = 1; i <=  n; i++) {
        std::cout << "| " << i << " ";
    }
    std::cout << "|" << std::endl;
}

void numberedGridRows(int n) {
    for (int i = 1; i <= n; i++) {
        horizontalDivider(n);
        std::cout << "| " << i << " ";
        for (int i = 1; i <= n; i++) {
            std::cout << "|   ";
        }
        std::cout << "|" << std::endl;
    }
}

void printBoard(int n) {
    // int n = 8; // 8x8 board
    horizontalDivider(n);
    firstNumberingRow(n);
    numberedGridRows(n);
    horizontalDivider(n);
}