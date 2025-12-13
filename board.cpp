#include "board.hpp"

Board::Board() : boardSize(8) { 
    whitesTurn = true;
    clearBoard();
    initialSetup();
    cout << "8x8 Othello board successfully created!" << endl;
}

Board::Board(int boardSize) : boardSize(boardSize) {
    whitesTurn = true;
    clearBoard();
    initialSetup();
    cout << boardSize << "x" << boardSize << " Othello board successfully created!" << endl;
}

void Board::clearBoard() {
    for (int i = 0; i < boardSize; i++) {
        vector<string> row;
        for (int j = 0; j < boardSize; j++) {
            row.push_back(NOTHING);
        }
        boardMatrix.push_back(row);
    }
}

void Board::initialSetup() {
    int centers[] = {boardSize/2 - 1, boardSize/2};
    for (int i : centers) {
        for (int j : centers) {
            if (i == j) {
                boardMatrix[i][j] = BLACK;
            }
            else {
                boardMatrix[i][j] = WHITE;
            }
        }
    }
}

void Board::horizontalDivider() {
    for (int i = 0; i < ((boardSize + 1)*4 + 1); i++) {
        cout << "-";
    }
    cout << endl;
}

void Board::firstNumberingRow() {
    cout << "|   ";
    for (int i = 1; i <= boardSize; i++) {
        cout << "| " << char(i + 64) << " ";
    }
    cout << "|" << endl;
}

void Board::renderRows() {
    for (int i = 1; i <= boardSize; i++) {
        Board::horizontalDivider();
        cout << "| " << i << " ";
        for (int j = 1; j <= boardSize; j++) {
            cout << "| " << boardMatrix[i - 1][j - 1] << " ";
            // cout << "|   ";
        }
        cout << "|" << endl;
    }
}

void Board::printBoard() {
    Board::horizontalDivider();
    Board::firstNumberingRow();
    Board::renderRows();
    Board::horizontalDivider();
}

void Board::place(int r, int c, string checker) {
    boardMatrix[r - 1][c - 1] = checker;
    cout << "Checker placed at " << r << char(c + int('A') - 1) << endl;
    printBoard();
    // TODO: legal move check
    // TODO: flipping

}

void Board::startGame() {
    cout << "Game started! Initial board:" << endl;
    printBoard();

    // TODO: loop limit fix
    int rounds = 0;
    while (rounds < (boardSize*boardSize - 4)) {
        string checker;
        if (whitesTurn) {
            cout << "White's turn" << endl;
            checker = WHITE;
        }
        else {
            cout << "Black's turn" << endl;
            checker = BLACK;
        }

        // move code parser
        string moveCode; 
        cout << "Enter move code" << endl;
        cin >> moveCode; // TODO: normalize moveCode
        int r = char(moveCode[0]) - '1' + 1;
        int c = char(moveCode[1]) - 'A' + 1;
        cout << "Moving to " << r << c << endl;
        // TODO: invalid moveCode or occupied slot: try again

        place(r, c, checker);
        // output
        cout << "Matrix after move: " << endl;
        printBoard();
        whitesTurn = !whitesTurn;
        rounds++;
    }
}

Board::~Board() {
    cout << "Board discarded!" << endl;
}
