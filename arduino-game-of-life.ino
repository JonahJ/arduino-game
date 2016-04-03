#include "Conway.h"

#ifndef SPEED
  #define SPEED 100
#endif /* SPEED */

Conway game_of_life = Conway(8, 8, 2, 2, 6);

void setup() {
    Serial.begin(9600);

    game_of_life.init();
}

void loop() {
    game_of_life.draw();
    delay(SPEED);
    game_of_life.update();
}
