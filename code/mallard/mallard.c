#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"
#include "profile.h"

#include <stdio.h>
#include <unistd.h>

const MinigameDef minigame_def = {
    .gamename = "A Mallard 64",
    .developername = "Josh Kautz",
    .description = "",
    .instructions = "",
};

// Number of frame back buffers we reserve.
// These buffers are used to render the video ahead of time.
// More buffers help ensure smooth video playback at the cost of more memory.
#define NUM_DISPLAY 8

// Maximum target audio frequency.
//
// Needs to be 48 kHz if Opus audio compression is used.
// In this example, we are using VADPCM audio compression
// which means we can use the real frequency of the audio track.
#define AUDIO_HZ 32000.0f

// Target screen resolution that we render at.
// Choosing a resolution above 240p (interlaced) can't be recommended for video playback.
#define SCREEN_WIDTH 288
#define SCREEN_HEIGHT 208

// Sound globals
wav64_t audio_track;

float fps;
mpeg2_t *video_track;
yuv_blitter_t yuv;
int nframes = 0;

/*==============================
    minigame_init
    The minigame initialization function
==============================*/
void minigame_init()
{
    joypad_init();
    // debug_init_isviewer();
    // debug_init_usblog();

    display_init((resolution_t){
                     .width = SCREEN_WIDTH,
                     .height = SCREEN_HEIGHT,
                     .interlaced = INTERLACE_OFF},
                 // 32-bit display mode is mandatory for video playback.
                 DEPTH_32_BPP, NUM_DISPLAY, GAMMA_NONE,
                 // FILTERS_DISABLED disables all VI post-processing to achieve the sharpest
                 // possible image. If you'd like to soften the image a little bit, switch to
                 // FITLERS_RESAMPLE.
                 FILTERS_DISABLED);

    dfs_init(DFS_DEFAULT_LOCATION);
    rdpq_init();
    profile_init();
    yuv_init();

    audio_init(AUDIO_HZ, 4);
    // mixer_init(8);

    // Check if the video is present in the filesystem, so that we can provide
    // a specific error message.
    FILE *f = fopen("rom:/mallard/video.m1v", "rb");
    assertf(f, "Video not found!\n");
    fclose(f);

    // Open the video using the mpeg2 module and create a YUV blitter to draw it.
    video_track = mpeg2_open("rom:/mallard/video.m1v");
    yuv = yuv_blitter_new_fmv(
        // Resolution of the video we expect to play.
        // Video needs to have a width divisible by 32 and a height divisible by 16.
        //
        // Here we have a video resolution of 288x160 which is a nice, valid resolution
        // that leaves a margin of 32 pixels on the side - great for making sure
        // CRT TVs with overscan still display the entire frame of your video.
        // The resolution is not an exact 16:9 ratio (16:8.88) but it's close enough that
        // most people won't notice. The lower resolution can also help with performance.
        mpeg2_get_width(video_track), mpeg2_get_height(video_track),
        // Set blitter's output area to our entire display
        display_get_width(), display_get_height(),
        // Override default FMV parms to not zoom the video.
        // This will leave our desired CRT TV-friendly margin around the video.
        &(yuv_fmv_parms_t){.zoom = YUV_ZOOM_NONE});

    // Engage the fps limiter to ensure proper video pacing.
    fps = mpeg2_get_framerate(video_track);
    display_set_fps_limit(fps);

    // Open the audio track and start playing it in channel 0.
    wav64_open(&audio_track, "rom:/mallard/video.wav64");
    mixer_ch_play(0, &audio_track.wave);

    fprintf(stderr, "End Init\n");
}

/*==============================
    minigame_fixedloop
    Code that is called every loop, at a fixed delta time.
    Use this function for stuff where a fixed delta time is
    important, like physics.
    @param  The fixed delta time for this tick
==============================*/
void minigame_fixedloop(float deltatime)
{
}

/*==============================
    minigame_loop
    Code that is called every loop.
    @param  The delta time for this tick
==============================*/
void minigame_loop(float deltatime)
{
    mixer_throttle(AUDIO_HZ / fps);

    if (!mpeg2_next_frame(video_track))
    {
        fprintf(stderr, "Video Ended\n");
        return;
    }

    // This polls the mixer to try and play the next chunk of audio, if available.
    // We call this function twice during the frame to make sure the audio never stalls.
    mixer_try_play();

    rdpq_attach(display_get(), NULL);

    PROFILE_START(PS_YUV, 0);
    // Get the next video frame and feed it into our previously set up blitter.
    yuv_frame_t frame = mpeg2_get_frame(video_track);
    yuv_blitter_run(&yuv, &frame);
    PROFILE_STOP(PS_YUV, 0);

    rdpq_detach_show();

    nframes++;

    mixer_try_play();

    PROFILE_START(PS_SYNC, 0);
    rspq_wait();
    PROFILE_STOP(PS_SYNC, 0);

    profile_next_frame();
    if (nframes % 128 == 0)
    {
        profile_dump();
        profile_init();
    }

    //////////////////////////////////////////////////////////////
    // Check if the start button is pressed to end the minigame //
    //////////////////////////////////////////////////////////////

    joypad_port_t controllerPort = core_get_playercontroller(0);

    if (!joypad_is_connected(controllerPort))
    {
        fprintf(stderr, "Controller not connected\n");
        return;
    }

    joypad_buttons_t btn = joypad_get_buttons_held(controllerPort);

    if (btn.start)
    {
        fprintf(stderr, "Start button pressed\n");
        minigame_end();
    }
}

/*==============================
    minigame_cleanup
    Clean up any memory used by your game just before it ends.
==============================*/
void minigame_cleanup()
{
    wav64_close(&audio_track);

    display_close();

    fprintf(stderr, "End Cleanup\n");
}