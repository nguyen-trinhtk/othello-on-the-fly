#include <iostream>
#include "board.hpp"
#include "constants.hpp"

using namespace std;

int main() {
    Board board = Board();
    board.autoplay();
    return OK;
}