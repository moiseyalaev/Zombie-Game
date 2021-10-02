#include <iostream>
#include "History.h"
#include "globals.h"
using namespace std;

History::History(int nRows, int nCols) 
{
    m_row = nRows; 
    m_col = nCols;

    for(int r = 1; r <= nRows; r++) {
        for (int c = 1; c <= nCols; c++)
            zGrid[r][c] = '.';
    }

}

bool History::record(int r, int c)
{
    if( (r < 1 || r >= m_row) && (c < 1 || c >= m_col) )
        return false;
    
    if(zGrid[r][c] == '.')
        zGrid[r][c] = 'A';
    else if (zGrid[r][c] != 'Z') 
        zGrid[r][c]++;

    return true;
}

void History::display() const
{
    clearScreen();

    for(int r = 1; r <= m_row; r++) {
        for (int c = 1; c <= m_col; c++)
            cout << zGrid[r][c];

        cout << endl;
    }
    cout << endl;   
}