#include "nerv_visualizer.h"
int main(void)
{
    const int INITIAL_SCREEN_WIDTH = 700;
    const int INITIAL_SCREEN_HEIGHT = 400;
    const int INITIAL_FPS = 60;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "NERV Sine Monitor");
    SetTargetFPS(INITIAL_FPS);

    Font Matisse_Pro_EB = LoadFont("resources/FOT-Matisse-Pro-EB.otf");
    Font font = Matisse_Pro_EB;

    if (!IsAudioDeviceReady())
        InitAudioDevice();
    Music audio_file = LoadMusicStream("putAudioFileHere/song.mp3");
    // right after LoadMusicStream:
    if (audio_file.stream.buffer == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to load audio file");
    }
    if (Matisse_Pro_EB.texture.id == 0)
{
    TraceLog(LOG_ERROR, "Font failed to load");
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
        BOUND_LEFT = RES_X / 2.0f - RES_X/ 2.0f * 0.3f;
        BOUND_RIGHT = BOUND_LEFT*2;
        window_monitor(IsWindowResized());
        music_switch(&is_playing, &audio_file);
        UpdateMusicStream(audio_file);

        BeginDrawing();
        ClearBackground(OFF_BLACK);

        _draw();
        
        EndDrawing();
    }
    UnloadMusicStream(audio_file);
    CloseAudioDevice();
    CloseWindow();
}