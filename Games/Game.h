#ifndef Game_h
#define Game_h

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
 *                                Game Settings                                *
 *******************************************************************************/

/**
 * Debug mode enables certain verbose messaging, including printing out the
 * current state. This may slow down the board speed if board is large
 */
#ifndef GAME_DEBUG
    #define GAME_DEBUG false
#endif /* GAME_DEBUG */

/**
 * Set to above 0 to have a max number moves before automatic reset. May be used
 * as a quick and dirty way to not get stuck in a repeated state
 */
#ifndef GAME_MAX_MOVES
    #define GAME_MAX_MOVES -1
#endif /* GAME_MAX_MOVES */

/**
 * Count moves
 */
#ifndef GAME_COUNT_MOVES
    #define GAME_COUNT_MOVES true
#endif /* GAME_COUNT_MOVES */

/**
 * GAME_BRIGHTNESS for NeoMatrix
 */
#ifndef GAME_BRIGHTNESS
    #define GAME_BRIGHTNESS (uint8_t)10
#endif /* GAME_BRIGHTNESS */

/**
 * Clear board on redraw
 */
#ifndef GAME_DRAW_CLEAR_ON_REDRAW
    #define GAME_DRAW_CLEAR_ON_REDRAW false
#endif GAME_DRAW_CLEAR_ON_REDRAW /* GAME_DRAW_CLEAR_ON_REDRAW */

/**
 * Simple draw mode, rows, then columns
 */
#ifndef GAME_DRAW_MODE_DEFAULT
    #define GAME_DRAW_MODE_DEFAULT 0
#endif /* GAME_DRAW_MODE_DEFAULT */

/**
 * Draw board row wise
 */
#ifndef GAME_DRAW_MODE_ROW_WISE
    #define GAME_DRAW_MODE_ROW_WISE GAME_DRAW_MODE_DEFAULT + 1
#endif GAME_DRAW_MODE_ROW_WISE /* GAME_DRAW_MODE_ROW_WISE */

/**
 * Draw board spiral style. Assumes board is equal in width and height. Uses
 * a bit more memory than typical col -> row draw
 */
#ifndef GAME_DRAW_MODE_SPIRAL
    #define GAME_DRAW_MODE_SPIRAL GAME_DRAW_MODE_ROW_WISE + 1
#endif GAME_DRAW_MODE_SPIRAL /* GAME_DRAW_MODE_SPIRAL */

/**
 * Draw board spiral style. Assumes board is equal in width and height. Uses
 * a bit more memory than typical col -> row draw
 */
#ifndef GAME_DRAW_MODE
    #define GAME_DRAW_MODE GAME_DRAW_MODE_DEFAULT
    // #define GAME_DRAW_MODE GAME_DRAW_MODE_ROW_WISE
    // #define GAME_DRAW_MODE GAME_DRAW_MODE_SPIRAL
#endif GAME_DRAW_MODE /* GAME_DRAW_MODE */

/**
 * Draw board one row at a time.
 */
#ifndef GAME_DRAW_EFFECT_WIPE
    #define GAME_DRAW_EFFECT_WIPE false
#endif GAME_DRAW_EFFECT_WIPE /* GAME_DRAW_EFFECT_WIPE */

/**
 * Additional delay between pixel draws. This is in addition to the delay
 * imposed by redrawing by the NeoMatrix library
 */
#ifndef GAME_DRAW_EFFECT_WIPE_DELAY
    #define GAME_DRAW_EFFECT_WIPE_DELAY 0
#endif GAME_DRAW_EFFECT_WIPE_DELAY /* GAME_DRAW_EFFECT_WIPE_DELAY */

/**
 * Redraws board one cell at a time. This will make the effect quite slow.
 */
#ifndef GAME_DRAW_EFFECT_PER_CELL
    #define GAME_DRAW_EFFECT_PER_CELL false
#endif GAME_DRAW_EFFECT_PER_CELL /* GAME_DRAW_EFFECT_PER_CELL */

/**
 * Shows a marker where it is drawing
 */
#ifndef GAME_DRAW_EFFECT_DRAW_MARKER
    #define GAME_DRAW_EFFECT_DRAW_MARKER true
#endif GAME_DRAW_EFFECT_DRAW_MARKER /* GAME_DRAW_EFFECT_DRAW_MARKER */


/*******************************************************************************
 *                                Cell States                                  *
 *******************************************************************************/

#ifndef CELL_STATE_DEAD
    #define CELL_STATE_DEAD 0
#endif /* CELL_STATE_DEAD */

#ifndef CELL_STATE_ALIVE
    #define CELL_STATE_ALIVE 1
#endif /* CELL_STATE_ALIVE */

#ifndef CELL_STATE_WIPE
    #define CELL_STATE_WIPE CELL_STATE_ALIVE + 1
#endif /* CELL_STATE_WIPE */

#ifndef CELL_STATE_ALIVE_LOW
    #define CELL_STATE_ALIVE_LOW  CELL_STATE_WIPE + 1
#endif /* CELL_STATE_ALIVE_LOW */

#ifndef CELL_STATE_ALIVE_HIGH
    #define CELL_STATE_ALIVE_HIGH CELL_STATE_ALIVE_LOW + 1
#endif /* CELL_STATE_ALIVE_HIGH */

#ifndef CELL_STATE_MAX
    #define CELL_STATE_MAX CELL_STATE_ALIVE_HIGH
#endif /* CELL_STATE_MAX */

/*******************************************************************************
 *                               Local Includes                                *
 *******************************************************************************/

#include "Boards/Board.h"


/*******************************************************************************
 *                                    Game                                     *
 *******************************************************************************/


class Game {
private:
    #if (GAME_DRAW_MODE_SPIRAL)
        uint8_t spiral_spins;
        uint8_t i_spiral_spin;
        uint8_t spiral_width;
        uint8_t spiral_height;

        void _drawSpiralCell(uint8_t x, uint8_t y);
    #endif /* GAME_DRAW_MODE_SPIRAL */

protected:
    Adafruit_NeoMatrix * led_matrix;

    Board * board;

    #if (GAME_COUNT_MOVES > 0)
        uint16_t number_of_rounds_running;
    #endif /* GAME_COUNT_MOVES */

    uint8_t i_col;
    uint8_t i_row;
    uint8_t width;
    uint8_t height;

    uint16_t * colors;

    virtual void _newRound();

public:
    Game(
        uint8_t num_pixels_width,
        uint8_t num_pixels_height,
        uint8_t num_boards_x,
        uint8_t num_boards_y,
        uint8_t pin
    );

    virtual void init();
    virtual void update();
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
Game::Game(
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

    #if (GAME_COUNT_MOVES > 0)
        number_of_rounds_running = 0;
    #endif /* GAME_COUNT_MOVES */

    board = new Board(width, height);
    board->reset();

    colors = new uint16_t [CELL_STATE_MAX + 1];
    colors[CELL_STATE_DEAD]     = led_matrix->Color(0, 0, 0);
    colors[CELL_STATE_ALIVE]    = led_matrix->Color(0, 255, 255);

    #if (CELL_STATE_MAX >= CELL_STATE_WIPE)
        colors[CELL_STATE_WIPE] = led_matrix->Color(0, 0, 255);
    #endif /* CELL_STATE_MAX >= CELL_STATE_WIPE */

    #if (CELL_STATE_MAX >= CELL_STATE_ALIVE_LOW)
        colors[CELL_STATE_ALIVE_LOW] = led_matrix->Color(0, 255, 0);
    #endif /* CELL_STATE_MAX > CELL_STATE_ALIVE_LOW */

    #if (CELL_STATE_MAX >= CELL_STATE_ALIVE_HIGH)
        colors[CELL_STATE_ALIVE_HIGH] = led_matrix->Color(255, 0, 0);
    #endif /* CELL_STATE_MAX > CELL_STATE_ALIVE_HIGH */
}

/**
 * Init game
 */
void Game::init() {
    led_matrix->setBrightness(GAME_BRIGHTNESS);
    led_matrix->begin();
    led_matrix->fillScreen(colors[CELL_STATE_DEAD]);
    led_matrix->show();

    // i_col = i_row = 3;
    // if (width == height) {
    //     i_col = width / 2;
    //     i_row = height / 2;
    // }
    // board->setAlive(i_col    , i_row - 1);
    // board->setAlive(i_col - 1, i_row    );
    // board->setAlive(i_col    , i_row    );
    // board->setAlive(i_col + 1, i_row    );
    // board->setAlive(i_col - 1, i_row + 1);
    // board->setAlive(i_col + 1, i_row + 1);
    // board->setAlive(i_col    , i_row + 2);

    _newRound();
}

/**
 * Update game
 */
void Game::update() { }

/**
 * Set new round
 */
void Game::_newRound() {
    board->reset();

    #if (GAME_COUNT_MOVES)
        number_of_rounds_running = 0;
    #endif /* GAME_COUNT_MOVES */
}

#if (GAME_DRAW_MODE_SPIRAL)
    /**
     * Draw an individual cell in the grid for spiral. Should only be called if
     * GAME_DRAW_EFFECT_PER_CELL is true
     *
     * @param  {uint8_t} x  x coordiante
     * @param  {uint8_t} y  y coordiante
     */
    void Game::_drawSpiralCell(uint8_t x, uint8_t y) {
        #if (GAME_DRAW_EFFECT_WIPE && GAME_DRAW_EFFECT_PER_CELL)
            #if (GAME_DRAW_EFFECT_DRAW_MARKER)
                led_matrix->drawPixel(x, y, colors[CELL_STATE_WIPE]);
                led_matrix->show();

                #if (GAME_DRAW_EFFECT_WIPE_DELAY > 0)
                    delay(GAME_DRAW_EFFECT_WIPE_DELAY);
                #endif /* GAME_DRAW_EFFECT_WIPE_DELAY > 0 */
            #endif /* GAME_DRAW_EFFECT_DRAW_MARKER */

            led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);

            #if (!GAME_DRAW_EFFECT_DRAW_MARKER)
                led_matrix->show();
            #endif /* !GAME_DRAW_EFFECT_DRAW_MARKER */
        #elif (GAME_DRAW_EFFECT_WIPE)
            led_matrix->drawPixel(x, y, colors[CELL_STATE_WIPE]);

            led_matrix->show();

            #if (GAME_DRAW_EFFECT_WIPE_DELAY > 0)
                delay(GAME_DRAW_EFFECT_WIPE_DELAY);
            #endif /* GAME_DRAW_EFFECT_WIPE_DELAY > 0 */
        #else
            led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);
        #endif /* GAME_DRAW_EFFECT_WIPE && GAME_DRAW_EFFECT_PER_CELL*/
    }
#endif

/**
 * Draw board
 */
void Game::draw() {
    #if (GAME_DEBUG)
        Serial.println("\nMOVE " + String(number_of_rounds_running));

        board->print();
    #endif /* GAME_DEBUG */

    #if (GAME_COUNT_MOVES)
        number_of_rounds_running++;

        #if (GAME_MAX_MOVES > 0)
            if(number_of_rounds_running > GAME_MAX_MOVES) {
                #if (GAME_DEBUG)
                    Serial.println("Maximum Number of Moves Ran, resetting board");
                #endif /* GAME_DEBUG */

                _newRound();
            }
        #endif /* GAME_MAX_MOVES */
    #endif /* GAME_COUNT_MOVES */


    #if (GAME_DRAW_CLEAR_ON_REDRAW)
        for (i_col = 0; i_col < width; i_col++) {
            for (i_row = 0; i_row < height; i_row++) {
                led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_DEAD]);
            }
        }

        led_matrix->show();
    #endif /* GAME_DRAW_CLEAR_ON_REDRAW */

    #if (GAME_DRAW_MODE == GAME_DRAW_MODE_SPIRAL)
        spiral_spins = (width / 2) + 1;
        spiral_width = width;
        spiral_height = height;

        i_col = 0;
        i_row = 0;

        for (i_spiral_spin = 0; i_spiral_spin < spiral_spins; i_spiral_spin++) {
            for (i_col = max(width - spiral_width - 1, 0); i_col < spiral_width; i_col++) {

                /**
                 * Up
                 */
                if (i_spiral_spin != 0) _drawSpiralCell(i_col, i_row);

                /**
                 * Right
                 */
                if (i_col == spiral_width - 1) {
                    for (i_row = height - spiral_height; i_row < spiral_height; i_row++) {
                        _drawSpiralCell(i_col, i_row);
                    }
                }
            }

            i_row--;
            i_col--;

            /**
             * Down
             */
            for (; i_col > width - spiral_width; i_col--) {
                _drawSpiralCell(i_col, i_row);
            }

            /**
             * Left
             */
            for (; i_row > height - spiral_height; i_row--) {
                _drawSpiralCell(i_col, i_row);
            }

            spiral_width--;
            spiral_height--;
        }

        #if (GAME_DRAW_EFFECT_WIPE && !GAME_DRAW_EFFECT_PER_CELL)
            for (i_col = 0; i_col < width; i_col++) {
                for (i_row = 0; i_row < height; i_row++) {
                    led_matrix->drawPixel(i_col, i_row, colors[board->getState(i_col, i_row)]);
                }
            }
        #endif /* GAME_DRAW_EFFECT_WIPE && !GAME_DRAW_EFFECT_PER_CELL */
    #else

        #if (GAME_DRAW_MODE == GAME_DRAW_MODE_ROW_WISE)
        for (i_row = 0; i_row < height; i_row++) {
        #else
        for (i_col = 0; i_col < width; i_col++) {
        #endif /* GAME_DRAW_MODE_ROW_WISE */
            /**
             * If we want to wipe, set all in row to visible
             */
            #if (GAME_DRAW_EFFECT_WIPE)
                #if (GAME_DRAW_MODE == GAME_DRAW_MODE_ROW_WISE)
                for (i_col = 0; i_col < width; i_col++) {
                #else
                for (i_row = 0; i_row < height; i_row++) {
                #endif /* GAME_DRAW_MODE_ROW_WISE */

                    #if (GAME_DRAW_EFFECT_PER_CELL)
                        #if (GAME_DRAW_EFFECT_DRAW_MARKER)
                            led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_WIPE]);
                        #endif /* GAME_DRAW_EFFECT_DRAW_MARKER */

                        #if (GAME_DRAW_MODE == GAME_DRAW_MODE_ROW_WISE)
                        if (i_col > 0) led_matrix->drawPixel(i_col - 1, i_row, colors[board->getState(i_col, i_row)]);
                        #else
                        if (i_row > 0) led_matrix->drawPixel(i_col, i_row - 1, colors[board->getState(i_col, i_row)]);
                        #endif /* GAME_DRAW_MODE_ROW_WISE */

                        led_matrix->show();

                        #if (GAME_DRAW_EFFECT_WIPE_DELAY > 0)
                            delay(GAME_DRAW_EFFECT_WIPE_DELAY);
                        #endif /* GAME_DRAW_EFFECT_WIPE_DELAY > 0 */

                    #elif (GAME_DRAW_EFFECT_DRAW_MARKER)
                        led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_WIPE]);
                    #endif /* GAME_DRAW_EFFECT_PER_CELL */
                }

                /**
                 * Don't want to delay twice for last cell
                 */
                #if (!GAME_DRAW_EFFECT_PER_CELL)
                    led_matrix->show();

                    #if (GAME_DRAW_EFFECT_WIPE_DELAY > 0)
                        delay(GAME_DRAW_EFFECT_WIPE_DELAY);
                    #endif /* GAME_DRAW_EFFECT_WIPE_DELAY > 0 */
                #endif /* !GAME_DRAW_EFFECT_PER_CELL */

            #endif /* GAME_DRAW_EFFECT_WIPE */

            /**
             * Draw cell
             */

            #if (GAME_DRAW_MODE == GAME_DRAW_MODE_ROW_WISE)
            for (i_col = 0; i_col < width; i_col++) {
            #else
            for (i_row = 0; i_row < height; i_row++) {
            #endif /* GAME_DRAW_MODE_ROW_WISE */
                led_matrix->drawPixel(i_col, i_row, colors[board->getState(i_col, i_row)]);
            }
        }

    #endif /* GAME_DRAW_MODE */

    led_matrix->show();
}

#endif /* Game_h */
