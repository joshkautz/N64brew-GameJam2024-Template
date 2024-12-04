#include <libdragon.h>
#include "../../../core.h"
#include "../../../minigame.h"
#include "../mallard.h"
#include "sequence_game.h"
#include "sequence_game_graphics.h"

#include "sequence_game_initialize.h"

// These player boxes are just used for visualizing the boxes and spawn points while developing.
#define PLAYER_1_BOX_X1 16
#define PLAYER_1_BOX_Y1 135
#define PLAYER_1_BOX_X2 168
#define PLAYER_1_BOX_Y2 220

#define PLAYER_2_BOX_X1 168
#define PLAYER_2_BOX_Y1 320
#define PLAYER_2_BOX_X2 288
#define PLAYER_2_BOX_Y2 220

#define PLAYER_3_BOX_X1 16
#define PLAYER_3_BOX_Y1 50
#define PLAYER_3_BOX_X2 168
#define PLAYER_3_BOX_Y2 135

#define PLAYER_4_BOX_X1 168
#define PLAYER_4_BOX_Y1 50
#define PLAYER_4_BOX_X2 320
#define PLAYER_4_BOX_Y2 135

float sequence_game_fade_in_elapsed = 0.0f;
float sequence_game_start_held_elapsed = 0.0f;
int sequence_game_player_holding_start = -1;

sprite_t *get_sprite_from_duck(Duck *duck)
{
    switch (duck->action)
    {
    case DUCK_BASE:
        return duck->base_sprite;
    case DUCK_IDLE:
        return duck->idle_sprite;
    case DUCK_SLAP:
        return duck->slap_sprite;
    case DUCK_WALK:
        return duck->walk_sprite;
    case DUCK_RUN:
        return duck->run_sprite;
    default:
        return duck->base_sprite;
    }
}

int get_frame_from_duck(Duck *duck)
{
    switch (duck->action)
    {
    case DUCK_BASE:
        return 0;
    case DUCK_IDLE:
        return (duck->frames >> 3) % SEQUENCE_GAME_MALLARD_IDLE_FRAMES; // Update every 8 frames
    case DUCK_WALK:
        return (duck->frames >> 2) % SEQUENCE_GAME_MALLARD_WALK_FRAMES; // Update every 4 frames
    case DUCK_SLAP:
        return (duck->frames >> 2) % SEQUENCE_GAME_MALLARD_SLAP_FRAMES; // Update every 4 frames
    case DUCK_RUN:
        return (duck->frames >> 2) % SEQUENCE_GAME_MALLARD_RUN_FRAMES; // Update every 4 frames
    default:
        return 0;
    }
}

void sequence_game_render_ducks()
{
    for (size_t i = 0; i < 4; i++)
    {
        Duck *duck = get_duck_by_id(i);

        rdpq_blitparms_t blitparms = {
            .s0 = get_frame_from_duck(duck) * 32,
            .t0 = 0,
            .width = 32,
            .height = 32,
            .flip_x = duck->direction == RIGHT ? true : false,
        };

        rdpq_mode_push();
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(get_sprite_from_duck(duck),
                         duck->x,
                         duck->y,
                         &blitparms);
        rdpq_mode_pop();

        rdpq_mode_push();
        rdpq_set_mode_standard();
        rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_set_prim_color(RGBA32(255, 0, 0, 64));
        rdpq_fill_rectangle(duck->collision_box_x1, duck->collision_box_y1, duck->collision_box_x2, duck->collision_box_y2);
        rdpq_mode_pop();
    }
}

void sequence_game_render_map()
{
    rdpq_mode_push();
    rdpq_set_mode_standard();
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_sprite_blit(sequence_game_background_lakeview_terrace_sprite,
                     0,
                     0,
                     NULL);
    rdpq_mode_pop();
}

void sequence_game_draw_press_start_to_pause()
{
    if (sequence_game_started == true && sequence_game_finished == false)
    {
        rdpq_mode_push();
        rdpq_set_mode_standard();
        rdpq_mode_alphacompare(1);
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY_CONST);
        rdpq_set_fog_color(RGBA32(0, 0, 0, (int)(255.0f * 0.5f)));
        // Draw "Start" button
        rdpq_sprite_blit(sequence_game_start_button_sprite,
                         RESOLUTION_320x240.width - sequence_game_start_button_sprite->width - 36,
                         RESOLUTION_320x240.height - sequence_game_start_button_sprite->height - 1,
                         NULL);
        rdpq_mode_pop();

        // Draw "Pause" text
        rdpq_text_print(NULL, FONT_HALODEK, RESOLUTION_320x240.width - 34, RESOLUTION_320x240.height - 5, "$01^01Pause");
    }
}

void sequence_game_draw_paused()
{
    float x = powf(sequence_game_start_held_elapsed, 3) * ((((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f);
    float y = powf(sequence_game_start_held_elapsed, 3) * ((((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f);
    float percentage = sequence_game_start_held_elapsed / GAME_EXIT_DURATION > 1.0 ? 1.0 : sequence_game_start_held_elapsed / GAME_EXIT_DURATION;

    // COLOR
    char *utf8_text = "$02^01PAUSED";
    if (sequence_game_player_holding_start == 0)
        utf8_text = "$02^01PAUSED";
    else if (sequence_game_player_holding_start == 1)
        utf8_text = "$02^02PAUSED";
    else if (sequence_game_player_holding_start == 2)
        utf8_text = "$02^03PAUSED";
    else if (sequence_game_player_holding_start == 3)
        utf8_text = "$02^04PAUSED";

    rdpq_set_scissor(0, 0, 70 + x + (180.0f * percentage), 240);
    rdpq_text_print(NULL, FONT_HALODEK_BIG, 70 + x, 140 + y, utf8_text);

    // WHITE
    rdpq_set_scissor(70 + x + (180.0f * percentage), 0, 320, 240);
    rdpq_text_print(NULL, FONT_HALODEK_BIG, 70 + x, 140 + y, "$02^00PAUSED");
}

int get_frame_from_snowman(Snowman *snowman)
{
    switch (snowman->action)
    {
    case SNOWMAN_IDLE:
        return (snowman->frames >> 3) % SEQUENCE_GAME_SNOWMAN_IDLE_FRAMES; // Update every 8 frames
    case SNOWMAN_JUMP:
        return (snowman->frames >> 3) % SEQUENCE_GAME_SNOWMAN_JUMP_FRAMES; // Update every 8 frames
    default:
        return 0;
    }
}

sprite_t *get_sprite_from_snowman(Snowman *snowman)
{
    switch (snowman->action)
    {
    case SNOWMAN_IDLE:
        return snowman->idle_sprite;
    case SNOWMAN_JUMP:
        return snowman->jump_sprite;
    default:
        return snowman->idle_sprite;
    }
}

void sequence_game_render_snowmen()
{
    Snowman *temporary = snowmen;
    while (temporary != NULL)
    {
        rdpq_blitparms_t blitparms = {
            .s0 = get_frame_from_snowman(temporary) * 13,
            .t0 = 0,
            .width = 13,
            .height = 17,
        };

        rdpq_mode_push();
        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(get_sprite_from_snowman(temporary),
                         temporary->x,
                         temporary->y,
                         &blitparms);
        rdpq_mode_pop();

        temporary = temporary->next;
    }
}

void render_snowman_collision_box(Snowman *snowman)
{
    rdpq_mode_push();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(0, 0, 255, 128));
    rdpq_fill_rectangle(snowman->collision_box_x1, snowman->collision_box_y1, snowman->collision_box_x2, snowman->collision_box_y2);
    rdpq_mode_pop();
}

void render_duck_slap_box(Duck *duck)
{
    rdpq_mode_push();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(255, 0, 0, 128));
    rdpq_fill_rectangle(duck->slap_box_x1, duck->slap_box_y1, duck->slap_box_x2, duck->slap_box_y2);
    rdpq_mode_pop();
}

void sequence_game_render_snowmen_and_ducks()
{
    Snowman *currentSnowman = snowmen;
    Duck *currentDuck = ducks;

    // Render snowmen.
    while (currentSnowman != NULL)
    {
        // Render ducks.
        while (currentDuck != NULL && currentDuck->collision_box_y2 < currentSnowman->collision_box_y2)
        {
            // Render duck
            rdpq_blitparms_t blitparms = {
                .s0 = get_frame_from_duck(currentDuck) * 32,
                .t0 = 0,
                .width = 32,
                .height = 32,
                .flip_x = currentDuck->direction == RIGHT ? true : false,
            };

            rdpq_set_mode_standard();
            rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
            rdpq_sprite_blit(get_sprite_from_duck(currentDuck), currentDuck->x, currentDuck->y, &blitparms);

            if (currentDuck->action == DUCK_SLAP)
            {
                render_duck_slap_box(currentDuck);
            }

            // Next duck.
            currentDuck = currentDuck->next;
        }

        // Render snowman.
        rdpq_blitparms_t blitparms = {
            .s0 = get_frame_from_snowman(currentSnowman) * 13,
            .t0 = 0,
            .width = 13,
            .height = 17,
        };

        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(get_sprite_from_snowman(currentSnowman), currentSnowman->x, currentSnowman->y, &blitparms);

        render_snowman_collision_box(currentSnowman);

        // Next snowman.
        currentSnowman = currentSnowman->next;
    }

    // Render ducks.
    while (currentDuck != NULL)
    {
        // Render duck
        rdpq_blitparms_t blitparms = {
            .s0 = get_frame_from_duck(currentDuck) * 32,
            .t0 = 0,
            .width = 32,
            .height = 32,
            .flip_x = currentDuck->direction == RIGHT ? true : false,
        };

        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(get_sprite_from_duck(currentDuck), currentDuck->x, currentDuck->y, &blitparms);

        if (currentDuck->action == DUCK_SLAP)
        {
            render_duck_slap_box(currentDuck);
        }

        // Next duck.
        currentDuck = currentDuck->next;
    }
}

void sequence_game_render(float deltatime)
{
    if (sequence_game_started == true && sequence_game_finished == false)
    {
        rdpq_attach(display_get(), NULL);
        rdpq_clear(BLACK);

        sequence_game_render_map();

        sequence_game_render_snowmen_and_ducks();

        if (sequence_game_paused == false)
            sequence_game_draw_press_start_to_pause();

        if (sequence_game_paused == true)
            sequence_game_draw_paused();

        // Fade in
        if (sequence_game_fade_in_elapsed < GAME_FADE_IN_DURATION)
        {
            float percentage = sequence_game_fade_in_elapsed > GAME_FADE_IN_DURATION ? 1.0f : sequence_game_fade_in_elapsed / GAME_FADE_IN_DURATION;
            uint8_t alpha = (int)(255.0f * (1.0f - percentage));
            rdpq_mode_push();
            rdpq_set_mode_standard();
            rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
            rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
            rdpq_set_prim_color(RGBA32(0, 0, 0, alpha));
            rdpq_fill_rectangle(0, 0, RESOLUTION_320x240.width, RESOLUTION_320x240.height);
            rdpq_mode_pop();
        }

        rdpq_detach_show();

        sequence_game_fade_in_elapsed += deltatime;
    }
}