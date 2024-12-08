#ifndef SEQUENCE_GAME_DUCK_H
#define SEQUENCE_GAME_DUCK_H

#define DUCK_TIME_BETWEEN_DAMAGE 1.0f

#define DUCK_SPRITE_WIDTH 32
#define DUCK_SPRITE_HEIGHT 32

#define DUCK_COLLISION_BOX_X1_OFFSET 8
#define DUCK_COLLISION_BOX_Y1_OFFSET 16
#define DUCK_COLLISION_BOX_X2_OFFSET 24
#define DUCK_COLLISION_BOX_Y2_OFFSET 24

#define DUCK_HIT_BOX_X1_OFFSET_FACING_LEFT 8
#define DUCK_HIT_BOX_Y1_OFFSET_FACING_LEFT 10
#define DUCK_HIT_BOX_X2_OFFSET_FACING_LEFT 26
#define DUCK_HIT_BOX_Y2_OFFSET_FACING_LEFT 24

#define DUCK_HIT_BOX_X1_OFFSET_FACING_RIGHT 6
#define DUCK_HIT_BOX_Y1_OFFSET_FACING_RIGHT 10
#define DUCK_HIT_BOX_X2_OFFSET_FACING_RIGHT 24
#define DUCK_HIT_BOX_Y2_OFFSET_FACING_RIGHT 24

#define DUCK_SLAP_BOX_X1_OFFSET_FACING_LEFT 3
#define DUCK_SLAP_BOX_Y1_OFFSET_FACING_LEFT 8
#define DUCK_SLAP_BOX_X2_OFFSET_FACING_LEFT 16
#define DUCK_SLAP_BOX_Y2_OFFSET_FACING_LEFT 20

#define DUCK_SLAP_BOX_X1_OFFSET_FACING_RIGHT 16
#define DUCK_SLAP_BOX_Y1_OFFSET_FACING_RIGHT 8
#define DUCK_SLAP_BOX_X2_OFFSET_FACING_RIGHT 29
#define DUCK_SLAP_BOX_Y2_OFFSET_FACING_RIGHT 20

extern Duck *ducks;

void display_ducks();
Duck *get_duck_by_id(int i);
void initialize_ducks();
void free_ducks();
void update_ducks(float deltatime);

#endif // SEQUENCE_GAME_DUCK_H