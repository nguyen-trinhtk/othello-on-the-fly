#include <iostream>
#include <vector>
#include <set>
#include <string>
using namespace std;

vector<vector<int> > makeAMat(){
    // nxn mat
    vector<vector<int> > mat;
    int n;
    cout << "Enter matrix size: ";
    cin >> n;
    for (int i = 0; i < n; i++) {
        vector<int> row;
        cout << "Row index " << n << endl;
        for (int j = 0; j < n; j++) {
            int entry;
            cout << "Enter value at entry (" << i << ", " << j << ")" << endl;
            cin >> entry;
            row.push_back(entry);
        }
        mat.push_back(row);
    }
    return mat;
}

void printMat(vector<vector<int> > mat) {
    int n = mat.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

// TODO: take in whose turn
bool isLegalMove(vector<vector<int> > mat, int r, int c) {
    // example: x's turn, y opponent
    if (mat[r][c] != 0) {return false;}
    
    /*
    General algorithm: 
    - move in the direction vector
    - if next is not occupied or is x's checker, skip to next check
    - otherwise, keep extending in direction until:
        + x met: then it is indeed a legal move
        + out of bound: nah, it is not a legal move unfortunately // TODO: check this again, i forgot how to play reversi!
    */

    // TODO
    // horizontal
    // left
    // right
    // vertical
    // up
    // down
    // diagonal
    // upright
    // downleft
    // cross-diagonal
    // upleft
    // downright
    
    return false;
}

vector<vector<int> > legalMoveMat(vector<vector<int> > mat) {
    vector<vector<int> > boolMat;
    int n = mat.size();
    for (int i = 0; i < n; i++) {
        vector<int> row;
        for (int j = 0; j < n; j++) {
            if (isLegalMove(mat, i, j)) {
                row.push_back(1);
            }
            else row.push_back(0);
        }
        boolMat.push_back(row);
    }
    return boolMat;
}

int main() {
    vector<vector<int> > mat = makeAMat(); // 0: free, 1: white, 2: black
    printMat(mat);
    cout << endl << endl;
    printMat(legalMoveMat(mat));
    return 0;
}