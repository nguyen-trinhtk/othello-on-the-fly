#include <iostream>
#include "board.hpp"

using namespace std;

int main() {
    // WARNINGS: padding not handled, n < 10 only!
    int n;
    cin >> n;
    printBoard(n);
}