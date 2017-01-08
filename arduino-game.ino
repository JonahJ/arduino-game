/**
 * Delay in ms between rounds
 */
#ifndef DELAY
    #define DELAY 50
#endif /* DELAY */

#include "Games/Game.h"
#include "Games/Conway.h"
#include "Games/LangstonsAnt.h"


// Game game = Game(8, 8, 2, 2, 6);

// Conway game = Conway(8, 8, 1, 1, 6);
// Conway game = Conway(8, 8, 2, 2, 6);

LangstonsAnt game = LangstonsAnt(8, 8, 2, 2, 6);


void setup() {

    #if (GAME_DEBUG)
        Serial.begin(9600);
    #endif /* GAME_DEBUG */

    game.init();
}

void loop() {
    game.draw();

    #if (DELAY > 0)
        delay(DELAY);
    #endif /* DELAY */

    game.update();
}
