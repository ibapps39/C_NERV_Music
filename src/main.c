#include "nerv_visualizer.h"
int main(void)
{
    const int INITIAL_SCREEN_WIDTH = 700;
    const int INITIAL_SCREEN_HEIGHT = 400;
    const int INITIAL_FPS = 60;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "NERV Sine Monitor");
    SetTargetFPS(INITIAL_FPS);

    Font fonts[5];
    int *cjk_ptr = NULL;
    int cp_count = 0;

    load_nerv_fonts(fonts, &cjk_ptr, &cp_count);
    
    if (!IsAudioDeviceReady())
        InitAudioDevice();
    //InitAudioDevice();
    Music audio_file = LoadMusicStream("putAudioFileHere/song.mp3");
    Sound alarm_tone = LoadSound("resources/Target_locked_tone.ogg");
    // right after LoadMusicStream:
    if (audio_file.stream.buffer == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to load audio file");
    }


    TIME = 0;
    RES_X = GetScreenWidth();
    RES_Y = GetScreenHeight();
    DT = 0.0f;

    PlayMusicStream(audio_file);
    AttachAudioStreamProcessor(audio_file.stream, audio_callback);
    bool is_playing = true;

    while (!WindowShouldClose())
    {
        TIME += GetFrameTime();
        BOUND_LEFT = RES_X / 2.0f - RES_X / 2.0f * 0.3f;
        BOUND_RIGHT = BOUND_LEFT * 2;
        window_monitor(IsWindowResized());
        music_switch(&is_playing, &audio_file);
        UpdateMusicStream(audio_file);
        play_angle_warning_sound(ANGEL, &alarm_tone);
        BeginDrawing();
        ClearBackground(OFF_BLACK);

        _draw(fonts);
        

        EndDrawing();
    }
    // --- CLEANUP SECTION ---
    UnloadMusicStream(audio_file);
    CloseAudioDevice();
    for (int i = 0; i < 5; i++) UnloadFont(fonts[i]);
    free(cjk_ptr);
    CloseWindow();
}