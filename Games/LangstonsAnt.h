#ifndef LangstonsAnt_h
#define LangstonsAnt_h

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include "Game.h"
#include "Ant.h"

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

#ifndef CELL_STATE_ANT
    #define CELL_STATE_ANT CELL_STATE_ALIVE_HIGH
#endif /* CELL_STATE_ANT */

#ifndef CELL_STATE_MAX
    #define CELL_STATE_MAX CELL_STATE_ANT
#endif /* CELL_STATE_MAX */

/*******************************************************************************
 *                                  Settings                                   *
 *******************************************************************************/

/**
 * Randomize board or not
 */
#ifndef LANGSTONS_ANT_RANDOMIZE_BOARD
    #define LANGSTONS_ANT_RANDOMIZE_BOARD 0
#endif /* LANGSTONS_ANT_RANDOMIZE_BOARD */


/*******************************************************************************
 *                                LangstonsAnt                                 *
 *******************************************************************************/

class LangstonsAnt: public Game {
private:
    Ant * ant;

    uint8_t state_cell_ant_on;

protected:
    virtual void _newRound();

public:
    LangstonsAnt(
        uint8_t num_pixels_width,
        uint8_t num_pixels_height,
        uint8_t num_boards_x,
        uint8_t num_boards_y,
        uint8_t pin
    );

    void update();
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

    ant = new Ant();

    state_cell_ant_on = CELL_STATE_DEAD;
}

/**
 * Begin a new round of conway. Reset grid and init colors.
 */
void LangstonsAnt::_newRound() {
    Game::_newRound();

    #if (ANT_RANDOMIZE)
        ant->randomize(width, height);
    #endif /* ANT_RANDOMIZE */

    #if (LANGSTONS_ANT_RANDOMIZE_BOARD)
        board->randomize();
    #endif /* LANGSTONS_ANT_RANDOMIZE_BOARD */

    // _initColors();
}

// void LangstonsAnt::_randomize() {
// }

/**
 * Update game
 */
void LangstonsAnt::update() {
    #if (GAME_DEBUG)
        ant->print();
    #endif /* GAME_DEBUG */

    /**
     * Set current ant cell
     */
    if (state_cell_ant_on == CELL_STATE_DEAD) {
        board->setState(
            ant->getX(),
            ant->getY(),
            CELL_STATE_ALIVE
        );

        ant->turn(false);
    }
    else if (state_cell_ant_on == CELL_STATE_ALIVE) {
        board->setState(
            ant->getX(),
            ant->getY(),
            CELL_STATE_DEAD
        );

        ant->turn(true);
    }
    else if (state_cell_ant_on == CELL_STATE_WIPE) {
        Serial.println("Should not be wipe");
    }
    else if (state_cell_ant_on == CELL_STATE_ALIVE_LOW) {
        Serial.println("Should not be alive low");
    }
    else if (state_cell_ant_on == CELL_STATE_ANT) {
        Serial.println("Should not be ant");
    }

    #if (GAME_DEBUG)
        ant->print();
    #endif /* GAME_DEBUG */

    /**
     * Move ant
     */
    ant->move(width, height);

    state_cell_ant_on = board->getState(ant->getX(), ant->getY());

    /**
     * Show moved ant
     */
    board->setState(
        ant->getX(),
        ant->getY(),
        CELL_STATE_ANT
    );
}

#endif /* LangstonsAnt_h */
