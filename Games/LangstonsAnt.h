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

/**
 * How many ants
 */
#ifndef LANGSTONS_ANT_NUM_ANTS
    #define LANGSTONS_ANT_NUM_ANTS 1
#endif /* LANGSTONS_ANT_NUM_ANTS */

/**
 * Check if all ants are the same
 */
#ifndef LANGSTONS_ANT_CHECK_ALL_ANTS_SAME
    #define LANGSTONS_ANT_CHECK_ALL_ANTS_SAME 0
#endif /* LANGSTONS_ANT_CHECK_ALL_ANTS_SAME */

/*******************************************************************************
 *                                LangstonsAnt                                 *
 *******************************************************************************/

class LangstonsAnt: public Game {
private:
    uint8_t i_ant;
    Ant ** ants;
    uint8_t * state_cell_ant_on;

    #if (LANGSTONS_ANT_CHECK_ALL_ANTS_SAME)
        bool all_ants_same;
    #endif /* LANGSTONS_ANT_CHECK_ALL_ANTS_SAME */

protected:
    virtual void _newRound();
    virtual void _turnAnts();

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
    ants = new Ant * [LANGSTONS_ANT_NUM_ANTS];
    state_cell_ant_on = new uint8_t [LANGSTONS_ANT_NUM_ANTS];

    for(i_ant = 0; i_ant < LANGSTONS_ANT_NUM_ANTS; i_ant++) {
        ants[i_ant] = new Ant();
        state_cell_ant_on[i_ant] = CELL_STATE_DEAD;
    }

    #if (LANGSTONS_ANT_CHECK_ALL_ANTS_SAME)
        all_ants_same = true;
    #endif
}

/**
 * Begin a new round of conway. Reset grid and init colors.
 */
void LangstonsAnt::_newRound() {
    Game::_newRound();

    #if (ANT_RANDOMIZE)
        #if (LANGSTONS_ANT_CHECK_ALL_ANTS_SAME)
        all_ants_same = true;
        while (all_ants_same) {
        #endif /* LANGSTONS_ANT_CHECK_ALL_ANTS_SAME */
            for(i_ant = 0; i_ant < LANGSTONS_ANT_NUM_ANTS; i_ant++) {

                #if (LANGSTONS_ANT_CHECK_ALL_ANTS_SAME)
                    if (LANGSTONS_ANT_NUM_ANTS == 1) all_ants_same = false;
                    else if ((i_ant > 0) && (all_ants_same)) all_ants_same = (ants[i_ant-1]->getX() == ants[i_ant]->getX()) && (ants[i_ant]->getY() == ants[i_ant]->getY());
                #endif /* LANGSTONS_ANT_CHECK_ALL_ANTS_SAME */

                ants[i_ant]->randomize(width, height);
            }


        #if (LANGSTONS_ANT_CHECK_ALL_ANTS_SAME)
        }
        #endif /* LANGSTONS_ANT_CHECK_ALL_ANTS_SAME */
    #else
        #if (LANGSTONS_ANT_CHECK_ALL_ANTS_SAME && GAME_DEBUG)

            if (LANGSTONS_ANT_NUM_ANTS > 1) {
                all_ants_same = true;

                for(i_ant = 0; i_ant < LANGSTONS_ANT_NUM_ANTS; i_ant++) {
                    if ((i_ant > 0) && (all_ants_same)){
                        all_ants_same = (ants[i_ant-1]->getX() == ants[i_ant]->getX()) && (ants[i_ant]->getY() == ants[i_ant]->getY());
                        break;
                    }
                }

                if (all_ants_same) Serial.println("All ants equal");
            }

        #endif /* LANGSTONS_ANT_CHECK_ALL_ANTS_SAME && GAME_DEBUG */
    #endif /* ANT_RANDOMIZE */

    #if (LANGSTONS_ANT_RANDOMIZE_BOARD)
        board->randomize();
    #endif /* LANGSTONS_ANT_RANDOMIZE_BOARD */
}

/**
 * Turn ants
 */
void LangstonsAnt::_turnAnts() {
    for(i_ant = 0; i_ant < LANGSTONS_ANT_NUM_ANTS; i_ant++) {
        if (state_cell_ant_on[i_ant] == CELL_STATE_DEAD) {
            board->setState(
                ants[i_ant]->getX(),
                ants[i_ant]->getY(),
                CELL_STATE_ALIVE
            );

            ants[i_ant]->turn(false);
        }
        else if (state_cell_ant_on[i_ant] == CELL_STATE_ALIVE) {
            board->setState(
                ants[i_ant]->getX(),
                ants[i_ant]->getY(),
                CELL_STATE_DEAD
            );

            ants[i_ant]->turn(true);
        }
        else if (state_cell_ant_on[i_ant] == CELL_STATE_WIPE) {
            Serial.println("Should not be wipe");
        }
        else if (state_cell_ant_on[i_ant] == CELL_STATE_ALIVE_LOW) {
            Serial.println("Should not be alive low");
        }
        else if (state_cell_ant_on[i_ant] == CELL_STATE_ANT) {
            Serial.println("Should not be ant");
        }
    }
}

/**
 * Update game
 */
void LangstonsAnt::update() {
    #if (GAME_DEBUG)
        for(i_ant = 0; i_ant < LANGSTONS_ANT_NUM_ANTS; i_ant++)
            ants[i_ant]->print();
    #endif /* GAME_DEBUG */

    _turnAnts();

    #if (GAME_DEBUG)
        for(i_ant = 0; i_ant < LANGSTONS_ANT_NUM_ANTS; i_ant++)
            ants[i_ant]->print();
    #endif /* GAME_DEBUG */

    /**
     * Move ants
     */
    for(i_ant = 0; i_ant < LANGSTONS_ANT_NUM_ANTS; i_ant++) {
        ants[i_ant]->move(width, height);

        state_cell_ant_on[i_ant] = board->getState(
            ants[i_ant]->getX(),
            ants[i_ant]->getY()
        );

        /**
         * Show moved ant
         */
        board->setState(
            ants[i_ant]->getX(),
            ants[i_ant]->getY(),
            CELL_STATE_ANT
        );
    }
}

#endif /* LangstonsAnt_h */
