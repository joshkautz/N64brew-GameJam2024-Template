#ifndef SEQUENCE_GAME_H
#define SEQUENCE_GAME_H

#include <libdragon.h>

#define BLACK RGBA32(0, 0, 0, 255)
#define RED RGBA32(255, 0, 0, 255)

#define GAME_FADE_IN_DURATION 1.0f
#define GAME_EXIT_DURATION 2.0f

extern bool sequence_game_finished;

void sequence_game(float deltatime);

#endif // SEQUENCE_GAME_H