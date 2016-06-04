#ifndef DELAY
  #define DELAY 100
#endif /* DELAY */

#include "Conway.h"

//Conway game_of_life = Conway(8, 8, 1, 1, 6);
Conway game_of_life = Conway(8, 8, 2, 2, 6);

void setup() {
    Serial.begin(9600);

    game_of_life.init();
}

void loop() {
    game_of_life.draw();
    delay(DELAY);
    game_of_life.update();
}
