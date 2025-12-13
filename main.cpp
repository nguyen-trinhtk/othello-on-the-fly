#include <iostream>

using namespace std;

void horizontalDivider(int n) {
    for (int i = 0; i < ((n + 1)*4 + 1); i++) {
            cout << "-";
    }
    cout << endl;
}

void firstNumberingRow(int n) {
    cout << "|   ";
    for (int i = 1; i <=  n; i++) {
        cout << "| " << i << " ";
    }
    cout << "|" << endl;
}

void numberedGridRows(int n) {
    for (int i = 1; i <= n; i++) {
        horizontalDivider(n);
        cout << "| " << i << " ";
        for (int i = 1; i <= n; i++) {
            cout << "|   ";
        }
        cout << "|" << endl;
    }
}

void printBoard() {
    int n = 8; // 8x8 board
    horizontalDivider(n);
    firstNumberingRow(n);
    numberedGridRows(n);
    horizontalDivider(n);
}

int main() {
    printBoard();
}