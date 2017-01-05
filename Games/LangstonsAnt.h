#ifndef LangstonsAnt_h
#define LangstonsAnt_h

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include "Game.h"


/*******************************************************************************
 *                               Local Includes                                *
 *******************************************************************************/

#include "Boards/Board.h"


/*******************************************************************************
 *                                LangstonsAnt                                 *
 *******************************************************************************/

class LangstonsAnt: public Game {
private:

public:
    LangstonsAnt(
        uint8_t num_pixels_width,
        uint8_t num_pixels_height,
        uint8_t num_boards_x,
        uint8_t num_boards_y,
        uint8_t pin
    );

    // void init();
    // void update();
    // void draw();
};

/**
 * Constructor
 *
 * @param  {uint8_t} num_pixels_width   Width of individual NeoMatrix
 * @param  {uint8_t} num_pixels_height  Height of individual NeoMatrix
 * @param  {uint8_t} num_boards_x       Number of boards in X direction
 * @param  {uint8_t} num_boards_y       Number of boards in Y direction
 * @param  {uint8_t} pin                Pin out on Arduino
 */
LangstonsAnt::LangstonsAnt(
    uint8_t num_pixels_width,
    uint8_t num_pixels_height,
    uint8_t num_boards_x,
    uint8_t num_boards_y,
    uint8_t pin
): Game(
    num_pixels_width,
    num_pixels_height,
    num_boards_x,
    num_boards_y,
    pin
) {

}



#endif /* LangstonsAnt_h */
