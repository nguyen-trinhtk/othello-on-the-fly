#include "board.hpp"

string Board::validateMoveFromInput() {
    string moveCode, moveCodeInput;
    int r, c;
    while (moveCode.empty()) {
            cout << "Enter move code" << endl;
            cin >> moveCodeInput; // TODO: normalize moveCodeInput
            if (moveCodeInput.length() != 2) {
                cerr << "Invalid move: wrong code format. Try again!" << endl;
                continue;
            }

            r = char(moveCodeInput[0]) - '1' + 1;
            c = char(moveCodeInput[1]) - 'A' + 1;

            if (r < 1 || r > 8) {
                cerr << "Invalid move: row index out of bound. Try again!" << endl;
                continue;
            }

            if (c < 1 || c > 8) {
                cerr << "Invalid move: column index out of bound. Try again!" << endl;
                continue;
            }

            if (boardMatrix[r - 1][c - 1] != NOTHING) {
                cerr << "Invalid move to occupied grid. Try again!";
                continue;
            }

            moveCode = moveCodeInput;
    }
    return moveCode;
}

vector<int> Board::parseMove(string validMoveCode) {
    int r, c; 
    r = char(validMoveCode[0]) - '1' + 1;
    c = char(validMoveCode[1]) - 'A' + 1;
    vector<int> coordinate;
    coordinate.push_back(r);
    coordinate.push_back(c);
    return coordinate;
}

Board::Board() : boardSize(8)
{
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
        string moveCode = validateMoveFromInput();
        vector<int> coordinate = parseMove(moveCode);
        int r = coordinate[0];
        int c = coordinate[1];

        // cout << "Moving to " << r << c << endl; // for debugging

        place(r, c, checker);
        // output
        cout << "Board after move: " << endl;
        printBoard();
        whitesTurn = !whitesTurn;
        rounds++;
    }
    cout << "Game over!" << endl;
}

Board::~Board() {
    cout << "Board discarded!" << endl;
}
