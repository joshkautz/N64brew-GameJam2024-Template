#ifndef SEQUENCE_GAME_SNOWMAN_H
#define SEQUENCE_GAME_SNOWMAN_H

extern sprite_t *sequence_game_snowman_idle_sprite;
extern sprite_t *sequence_game_snowman_jump_sprite;

Snowman *add_snowman(Snowman *snowmen);
void free_snowmen(Snowman *snowmen);
void increase_snowmen_time_and_frames(Snowman *snowmen, float deltatime);
void list_snowmen(Snowman *snowmen);
int count_snowmen(Snowman *snowmen);

#endif // SEQUENCE_GAME_SNOWMAN_H