#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"
#include "input.h"

void process_controller(float deltatime)
{
    for (size_t i = 0; i < core_get_playercount(); i++)
    {
        joypad_port_t controllerPort = core_get_playercontroller(i);
        joypad_buttons_t pressed = joypad_get_buttons_pressed(controllerPort);

        if (!joypad_is_connected(controllerPort))
        {
            continue;
        }

        // Exit the minigame.
        if (pressed.b)
        {
            fprintf(stderr, "Controller %u has pressed B. Minigame will now end.\n", controllerPort);
            minigame_end();
        }

        // Pause the video.
        if (pressed.a)
        {
            fprintf(stderr, "Controller %u has pressed A. Video is now %s.\n", controllerPort, paused ? "unpaused" : "paused");
            paused = !paused;
        }
    }
}
