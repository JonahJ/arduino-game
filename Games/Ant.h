
/*******************************************************************************
 *                                  Settings                                   *
 *******************************************************************************/

/**
 * Randomize ant location or not. If false ant starts at (0, 0)
 */
#ifndef ANT_RANDOMIZE
    #define ANT_RANDOMIZE 1
#endif /* ANT_RANDOMIZE */

/**
 * Wrap board or not. If this is false then the ant can become stuck in a corner
 */
#ifndef ANT_WRAP_BOARD
    #define ANT_WRAP_BOARD 1
#endif ANT_WRAP_BOARD

/*******************************************************************************
 *                                Directions                                   *
 *******************************************************************************/

#ifndef ANT_DIRECTION_UP
    #define ANT_DIRECTION_UP 0
#endif /* ANT_DIRECTION_UP */

#ifndef ANT_DIRECTION_RIGHT
    #define ANT_DIRECTION_RIGHT ANT_DIRECTION_UP + 1
#endif /* ANT_DIRECTION_RIGHT */

#ifndef ANT_DIRECTION_DOWN
    #define ANT_DIRECTION_DOWN ANT_DIRECTION_RIGHT + 1
#endif /* ANT_DIRECTION_DOWN */

#ifndef ANT_DIRECTION_LEFT
    #define ANT_DIRECTION_LEFT ANT_DIRECTION_DOWN + 1
#endif /* ANT_DIRECTION_LEFT */

/*******************************************************************************
 *                                   Class                                     *
 *******************************************************************************/

class Ant {
private:
    int8_t x;
    int8_t y;

    int8_t direction;

public:
    Ant(
        int8_t _x = 0,
        int8_t _y = 0,
        int8_t _direction = ANT_DIRECTION_UP
    );

    #if (GAME_DEBUG)
        void print() const;
    #endif /* GAME_DEBUG */

    #if (ANT_RANDOMIZE)
        void randomize(uint8_t width = 8, uint8_t height = 8);
    #endif /* ANT_RANDOMIZE */

    int8_t getX() const;
    int8_t getY() const;

    void move(uint8_t width = 8, uint8_t height = 8);
    void turn(bool clockwise = true);

};

/**
 * Init Ant
 *
 * @param {int8_t} x
 * @param {int8_t} y
 * @param {int8_t} direction
 */
Ant::Ant(
    int8_t _x = 0,
    int8_t _y = 0,
    int8_t _direction = ANT_DIRECTION_UP
) {
    x = _x;
    y = _y;

    direction = _direction;
};

/**
 * Print Ant to Serial
 */
#if (GAME_DEBUG)
    void Ant::print() const {
        Serial.println("Ant");

        Serial.println(" | Coordinate: (" + String(x) + ", " + String(y) + ")");

        Serial.print(" | Direction: ");
        if (direction == ANT_DIRECTION_UP) Serial.print("Up");
        else if (direction == ANT_DIRECTION_RIGHT) Serial.print("Right");
        else if (direction == ANT_DIRECTION_DOWN) Serial.print("Down");
        else if (direction == ANT_DIRECTION_LEFT) Serial.print("Left");
        else Serial.print("Unknown");
        Serial.print("\n");
    }
#endif /* GAME_DEBUG */

/**
 * Randomize ant location and direction
 *
 * @param  {uint8_t} width
 * @param  {uint8_t} height
 */
#if (ANT_RANDOMIZE)
    void Ant::randomize(uint8_t width = 8, uint8_t height = 8 ) {
        // x = 5;
        // y = 5;
        // // direction = ANT_DIRECTION_UP;
        // direction = ANT_DIRECTION_RIGHT;
        // // direction = ANT_DIRECTION_DOWN;
        // // direction = ANT_DIRECTION_LEFT;
        // return;

        randomSeed(analogRead(0));
        randomSeed(analogRead(random(0, 5)));

        x = random(0, width);
        randomSeed(analogRead(random(0, 5)));
        randomSeed(random(0, width));

        y = random(0, height);
        randomSeed(analogRead(random(0, 5)));
        randomSeed(random(0, height));

        direction = random(ANT_DIRECTION_UP, ANT_DIRECTION_LEFT + 1);
    }
#endif /* ANT_RANDOMIZE */

/**
 * Gets X coordinate
 *
 * @return {int8_t} x
 */
int8_t Ant::getX() const { return x; }

/**
 * Gets Y coordinate
 *
 * @return {int8_t} y
 */
int8_t Ant::getY() const { return y; }

/**
 * Move in proper direction
 *
 * @param  {uint8_t} width
 * @param  {uint8_t} height
 */
void Ant::move(uint8_t width = 8, uint8_t height = 8) {
    if (direction == ANT_DIRECTION_UP) y--;
    else if (direction == ANT_DIRECTION_RIGHT) x++;
    else if (direction == ANT_DIRECTION_DOWN) y++;
    else if (direction == ANT_DIRECTION_LEFT) x--;

    /**
     * Check if beyond boundary
     */
    #if (ANT_WRAP_BOARD)
        if (y == width) y = 0;
        else if (y < 0) y = --width;

        if (x == height) x = 0;
        else if (x < 0) x = --height;
    #else
        if (y == width) y--;
        else if (y < 0) y++;

        if (x == height) x--;
        else if (x < 0) x++;
    #endif /* ANT_WRAP_BOARD */
}

/**
 * Turn the Ant
 *
 * @param  {bool} clockwise turn if true
 */
void Ant::turn(bool clockwise = true) {
    if (clockwise) direction++;
    else direction--;

    if (direction > ANT_DIRECTION_LEFT) direction = ANT_DIRECTION_UP;
    else if (direction < ANT_DIRECTION_UP) direction = ANT_DIRECTION_LEFT;
}
