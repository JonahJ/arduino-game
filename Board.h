//
//  Board.h
//  char
//
//  Created by Jonah Joselow on 3/23/16.
//  Copyright © 2016 Jonah Joselow. All rights reserved.
//

unsigned char i_col = 0;
unsigned char i_row = 0;

#ifndef Board_h
#define Board_h

#ifndef CELL_STATE_ALIVE
    #define CELL_STATE_ALIVE 1
#endif /* CELL_STATE_ALIVE */

#ifndef CELL_STATE_DEAD
    #define CELL_STATE_DEAD 0
#endif /* CELL_STATE_DEAD */

#ifndef CELLS_PER_BYTE
    #define CELLS_PER_BYTE 8
#endif /* CELLS_PER_BYTE */

#ifndef COLUMN_WIDTH
    #define COLUMN_WIDTH 8 / CELLS_PER_BYTE
#endif /* COLUMN_WIDTH */

class Board {
private:
    unsigned char ** board;

    // unsigned char width;
    // unsigned char height;
    unsigned char num_columns;

    unsigned char i_col_count;
    // unsigned char i_col;
    // unsigned char i_row;


public:
    Board(unsigned char _width, unsigned char _height);

    int getState(unsigned char x, unsigned char y);

    void setState(unsigned char x, unsigned char y, int state);
    void setAlive(unsigned char x, unsigned char y);
    void setDead(unsigned char x, unsigned char y);

    void reset();
};

Board::Board(unsigned char _width, unsigned char _height){
    // width = _width;
    // height = _height;

    num_columns = width / CELLS_PER_BYTE;


    board = new unsigned char * [num_columns];

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        board[i_col_count] = new unsigned char [height];
        for(i_row = 0; i_row < height; i_row++){
            board[i_col_count][i_row] = NULL;
            board[i_col_count][i_row] = CELL_STATE_DEAD;
        }
    }

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for (i_col = 0; i_col < width; i_col++) {
            for(i_row = 0; i_row < height; i_row++) {
                // board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));

                if(i_col_count == 0 && i_col == 1 && i_row == 1){
                    board[i_col_count][i_row] = 3;

                    // board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
                }

            }
        }
    }
}

int Board::getState(unsigned char x, unsigned char y){
    i_col_count = x / CELLS_PER_BYTE;
    i_col = x - i_col_count * CELLS_PER_BYTE;
    i_row = y;

    int over = 1;

    // if(COLUMN_WIDTH > 1){
    //     over += COLUMN_WIDTH;
    // }
   // cout
   // << "( " << static_cast<unsigned>(x)
   // << " , " << static_cast<unsigned>(y)
   // << " ) => ("
   // << static_cast<unsigned>(i_col_count) << " + " << static_cast<unsigned>(i_col)
   // << " , "
   // << static_cast<unsigned>(i_row)
   // << " )"
   // << endl;

    // return ((board[i_col_count][i_row] >> i_col) & COLUMN_WIDTH);

    // if(COLUMN_WIDTH == 1) return ((board[i_col_count][i_row] >> i_col) & (1 << (COLUMN_WIDTH)) - 1);

    return ((board[i_col_count][i_row] >> i_col) & (1 << (over)) - 1);
    // return ((board[i_col_count][i_row] >> i_col) & (1 << (COLUMN_WIDTH + 1)) - 1);
    // ((board[i_col_count][i_row] >> i_col) & COLUMN_WIDTH);
}

void Board::setState(unsigned char x, unsigned char y, int state){
    i_col_count = x / CELLS_PER_BYTE;
    i_col = x - i_col_count * CELLS_PER_BYTE;
    i_row = y;

    board[i_col_count][i_row] = (board[i_col_count][i_row] | (state << i_col));
}

void Board::setAlive(unsigned char x, unsigned char y){
    setState(x, y, CELL_STATE_ALIVE);
}
void Board::setDead(unsigned char x, unsigned char y){
    setState(x, y, CELL_STATE_DEAD);
}

void Board::reset(){
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_row = 0; i_row < height; i_row++) {
            board[i_col_count][i_row] = NULL;
        }
    }

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for (i_col = 0; i_col < width; i_col++) {
            for(i_row = 0; i_row < height; i_row++) {
                board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_DEAD << i_col));
            }
        }
    }
}



#endif /* Board_h */
