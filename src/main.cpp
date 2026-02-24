#include <iostream>
#include "board.hpp"
#include "constants.hpp"

using namespace std;

int main() {
    Board board = Board();
    board.start_game();
    // board.autoplay();
    return OK;
}