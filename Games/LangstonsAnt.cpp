#ifndef LangstonsAnt_h
#define LangstonsAnt_h

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

/**
 * Make Arduino Due happy
 */
#ifndef PSTR
    #define PSTR
#endif /* PSTR */


/*******************************************************************************
 *                               Local Includes                                *
 *******************************************************************************/

#include "Boards/Board.h"


/*******************************************************************************
 *                                LangstonsAnt                                 *
 *******************************************************************************/

class LangstonsAnt {
private:

public:
    LangstonsAnt(
        uint8_t num_pixels_width,
        uint8_t num_pixels_height,
        uint8_t num_boards_x,
        uint8_t num_boards_y,
        uint8_t pin
    );

    void init();
    void update();
    void draw();
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
) {
    led_matrix = new Adafruit_NeoMatrix(
        num_pixels_width,
        num_pixels_height,
        num_boards_x,
        num_boards_y,
        pin,
        NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS    + NEO_TILE_PROGRESSIVE +
        NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_COLUMNS + NEO_TILE_ZIGZAG,
        NEO_GRB + NEO_KHZ800
    );

    width = led_matrix->width();
    height = led_matrix->height();

    #if (CONWAY_COUNT_MOVES > 0)
        number_of_rounds_running = 0;
    #endif /* CONWAY_COUNT_MOVES */

    #if (CONWAY_CYCLE_DETECTED_BUFFER > 0)
        number_of_moves_since_cycle_detected = 0;
    #endif /* CONWAY_CYCLE_DETECTED_BUFFER */

    /**
     * Detect largest width of % 8
     */
    // if (width % 8 != 0) {
    //     return;
    // }

    any_cells_alive = false;
    board = new Board(width, height);

    // row_wise_annex = !(width <= height);
    // if (row_wise_annex) board_next = new BoardAnnex(2, height);
    // else

    #if (CONWAY_CHECKING_BOARD_MINIMIZE)
        #if (CONWAY_CHECKING_BOARD_USE_STACK)
            board_next = new BoardAnnexStack(width, 2);
        #else
            board_next = new BoardAnnex(width, 2);
        #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
    #else
        board_next = new Board(width, height);
    #endif /* CONWAY_CHECKING_BOARD_MINIMIZE */

    #if (CONWAY_CHECK_IF_IN_CYCLE)
        board_two_ago =  new Board(width, height);
    #endif /* CONWAY_CHECK_IF_IN_CYCLE */

    // delete colors;

    colors = new uint16_t [CELL_STATE_MAX - 1];
    colors[CELL_STATE_DEAD] = led_matrix->Color(0, 0, 0);
    colors[CELL_STATE_WIPE] = led_matrix->Color(255, 255, 255);

}



#endif /* LangstonsAnt_h */
