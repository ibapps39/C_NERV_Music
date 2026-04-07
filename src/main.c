#include "nerv_visualizer.h"
int main(void)
{
    const int INITIAL_SCREEN_WIDTH = 700;
    const int INITIAL_SCREEN_HEIGHT = 400;
    const int INITIAL_FPS = 60;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(0, 0, "NERV Sine Monitor");
    if (!IsWindowFullscreen())
        ToggleFullscreen();

    Font fonts[6];
    int *cjk_ptr = NULL;
    int cp_count = 0;
    load_nerv_fonts(fonts, &cjk_ptr, &cp_count);
    Shader bloom_s = LoadShader(NULL, "resources/shaders/bloom.fs");

    if (!IsAudioDeviceReady())
        InitAudioDevice();
    // InitAudioDevice();
    Music audio_file = LoadMusicStream("putAudioFileHere/song.mp3");
    Sound alarm_tone = LoadSound("resources/Target_locked_tone.ogg");
    // right after LoadMusicStream:
    if (audio_file.stream.buffer == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to load audio file");
    }

    // TIME = 0;
    RES_X = GetScreenWidth();
    RES_Y = GetScreenHeight();
    DT = 0.0f;

    PlayMusicStream(audio_file);
    AttachAudioStreamProcessor(audio_file.stream, audio_callback);
    bool is_playing = true;
    RenderTexture2D target = LoadRenderTexture(RES_X, RES_Y);
    // 4. Re-calculate your render texture to match the new screen size
    // If you don't do this, your shader will look like shit because it's upscaling
    UnloadRenderTexture(target);
    target = LoadRenderTexture(RES_X, RES_Y);

    while (!WindowShouldClose())
    {
        TIME = GetTime();

        Rectangle shader_rect = {
            .x = 0,
            .y = 0,
            .width = RES_X,
            .height = -RES_Y,
        };
        if (IsKeyDown(KEY_F))
            SeekMusicStream(audio_file, (GetTime()) + 5.0f);
        BOUND_LEFT = RES_X / 2.0f - RES_X / 2.0f * 0.3f;
        BOUND_RIGHT = BOUND_LEFT * 2;
        window_monitor(IsWindowResized());
        shader_monitor(&target);
        music_switch(&is_playing, &audio_file);
        UpdateMusicStream(audio_file);
        play_angle_warning_sound(ANGEL, &alarm_tone);

        BeginTextureMode(target);
        ClearBackground(OFF_BLACK);
        _draw(fonts, &audio_file);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(OFF_BLACK);
        BeginShaderMode(bloom_s);
        // NOTE: Rendertextures are Y-flipped, so we use a negative height
        DrawTextureRec(target.texture, shader_rect, (Vector2){0, 0}, WHITE);
        EndShaderMode();
        EndDrawing();
    }

    // --- CLEANUP SECTION ---
    UnloadShader(bloom_s);
    UnloadRenderTexture(target);
    UnloadMusicStream(audio_file);
    CloseAudioDevice();
    for (int i = 0; i < 5; i++)
        UnloadFont(fonts[i]);
    free(cjk_ptr);
    CloseWindow();
}