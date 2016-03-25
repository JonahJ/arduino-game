#include "Conway.h"

#define SPEED 1000

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
