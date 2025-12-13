#include <iostream>

using namespace std;

void printBoard() {
    int n = 8; // 8x8 board
    // numbering line
    cout << "   ";
    for (int i = 1; i <=  8; i++) {
        cout << "| " << i << " ";
    }
    cout << "|" << endl;

    for (int i = 1; i <= 8; i++) {
        for (int i = 0; i < 36; i++) {
            cout << "-";
        }
        cout << endl;
        cout << " " << i << " ";
        for (int i = 1; i <= 8; i++) {
            cout << "|   ";
        }
        cout << "|" << endl;
    }

    for (int i = 0; i < 36; i++) {
            cout << "-";
        }
    cout << endl;
}

int main() {
    printBoard();
}