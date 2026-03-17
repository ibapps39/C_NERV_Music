#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h> // Required for malloc/free
#include <stdio.h>
#include <stdbool.h>

struct Vec2
{
    float x, y, z;
};
typedef Vector2 Vec2;
int RES_X, RES_Y, TIME;
int DEFAULT_SAMPLE_RATE = 44100;
float BOUND_LEFT;
float BOUND_RIGHT;
float TIP_Y;
const int FFT_SIZE = 2048; // FFT_SIZE must be a power of 2
float DT;
typedef struct SCR_INFO
{
    int width, height;
    float dt;
} SCR_INFO;

// [=======================================================================================================================================]
// TODO
//

// [=======================================================================================================================================]
// DEFAULTS, CONSTANTS, & MACROS

#define DEFAULT_BOTTOM_SCREEN DEFAULT_WINDOW_Y
#define DEFAULT_CENTER_SCREEN (Vector2){.x = DEFAULT_WINDOW_X / 2, .y = DEFAULT_WINDOW_Y / 2}
#define DEFAULT_FPS 60
#define DEFAULT_MAP_VECTOR2D \
    (Vector2) { DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y }
#define DEFAULT_PLAYER_COLOR (Color){.r = 0, .g = 255, .b = 0, .a = 255}
#define DEFAULT_PLAYER_SIZE \
    (Vector2) { .x = .0333 * DEFAULT_WINDOW_X, .y = .0667 * DEFAULT_WINDOW_Y }
#define DEFAULT_RESOLUTION ((int)DEFAULT_WINDOW_X * (int)DEFAULT_WINDOW_Y)
#define DEFAULT_WINDOW_X 600
#define DEFAULT_WINDOW_Y 700
#define DEFAULT_GRID_SQUARE_LW 8
#define PI_2 M_PI * 2
#define PI_3H M_PI * 3 / 2
#define PI_H M_PI / 2
#define TRUE 1
#define FALSE 0

// [=======================================================================================================================================]
//  NERV COLORS
// [=======================================================================================================================================]

#define SCANLINES_COLOR (Color){40, 20, 0, 100}
#define OFF_BLACK (Color){10, 5, 0, 255}

#define EVA_GREEN (Color){180, 255, 0, 255}      // 0: EVA Green (Sync ratio)
#define EVA_08_PINK (Color){255, 100, 200, 255}  // 4: EVA-08 Pink
#define EVA_01_PURPLE (Color){120, 80, 200, 255} // 5: EVA-01 Purple
#define EVA_00_BLUE (Color){40, 110, 200, 255}   // Rei / Unit-00 Plate color
#define EVA_00_ORANGE (Color){240, 140, 40, 255} // Unit-00 Prototype armor
#define EVA_02_RED (Color){200, 40, 40, 255}     // Asuka / Unit-02 Primary
#define EVA_02_YELLOW (Color){255, 230, 0, 255}  // Unit-02 Chest plate/Details
#define EVA_MARK06_NAVY (Color){30, 40, 80, 255} // Mark.06 Deep Blue

#define NERV_INTERFACE_BLUE (Color){0, 150, 255, 255} // 1: Deep Blue (Interface highlights)
#define NERV_ALERT_RED (Color){230, 30, 30, 255}      // 2: Alert Red (Pattern Blue detected!)
#define NERV_MAGI_AMBER (Color){255, 170, 0, 255}     // 3: MAGI Amber
#define NERV_MAP_ORANGE (Color){255, 120, 0, 255}     // Tactical map terrain/grid
#define NERV_TEXT_GOLD (Color){255, 200, 0, 255}      // Standard MAGI terminal text
#define NERV_CRITICAL_DARK (Color){100, 0, 0, 255}    // Background for "BLOOD TYPE: BLUE"
#define NERV_DIM_GREEN (Color){40, 100, 40, 255}      // Low-priority status/Inactive systems
#define NERV_SCANNER_TEAL (Color){0, 200, 180, 255}   // Sonar and Bio-mechanical readouts

#define NERV_HL_WHITE (Color){220, 230, 240, 200}      // High-glare UI borders (Semi-transparent)
#define NERV_WARNING_PURPLE (Color){160, 40, 255, 255} // Pattern Orange / Contamination warning
#define NERV_SHADOW_BROWN (Color){60, 30, 10, 255}     // Subtle UI drop shadows / Depth
#define NERV_LCL_ORANGE (Color){255, 150, 50, 180}     // LCL Fluid visualization
#define NERV_SYNC_GRAPH (Color){150, 255, 150, 255}    // Pulse/Graph line highlights

/*
UI Design Tips for the NERV Look

Transparency is Key:
NERV interfaces often use layers. Use an alpha value (the .a in your Color struct) of
- around 160 to 200 for window backgrounds so you can see the grid behind them.

The "Scanline" Effect: In your Draw() loop, drawing a series of horizontal lines with
- (Color){ 0, 0, 0, 50 } every 2 pixels instantly gives it that 90s CRT monitor vibe.

The Font: If you can find it, use Matisse EB—that’s the specific serif font used for the
- "ANGEL ATTACK" and "PATTERN BLUE" warnings.
*/
typedef float (*Wave_f)(float);
typedef struct Signal
{
    float freq;  // Fundamental frequency (Hz)
    float amp;   // Linear amplitude (0.0 to 1.0)
    float phase; // Current accumulator (0.0 to 2*PI)
    float omega; // Angular frequency (computed: 2 * PI * freq / sampleRate)
    Wave_f signature;
} Signal;

typedef struct Sample
{
    float samples[FFT_SIZE]; // Precomputed samples for one second of audio at the given frequency
    int write_head;
    int sample_rate;
} Sample;
Sample g_sample = {0};


//[=======================================================================================================================================
// GUI, UI, Monitors, etc
void audio_callback(void *bufferdata, unsigned int sample_pairs_count)
{
    float *incoming_samples = (float *)bufferdata;
    for (size_t i = 0; i < sample_pairs_count; i++)
    {
        int left_channel = i * 2;
        g_sample.samples[g_sample.write_head] = incoming_samples[left_channel];
        // wraps around if reached FFT_SIZE
        g_sample.write_head = (g_sample.write_head + 1) % FFT_SIZE;
    }
}
void music_switch(bool *is_playing, Music *audio_file)
{
    if (IsKeyPressed(KEY_SPACE))
    {
        if (!*is_playing && !IsMusicStreamPlaying(*audio_file))
        {
            ResumeMusicStream(*audio_file);
            *is_playing = true;
        }
        else
        {
            PauseMusicStream(*audio_file);
            *is_playing = false;
        }
    }
}
void window_monitor(bool is_resized, SCR_INFO *scr_info)
{
    if (is_resized)
    {
        RES_X = GetScreenWidth();
        RES_Y = GetScreenHeight();
        scr_info->width = RES_X;
        scr_info->height = RES_Y;
    }
    DT = GetFrameTime();
    scr_info->dt = GetFrameTime();
    if (TIME > 1000000)
    {
        TIME = 0;
    }
}
//[=======================================================================================================================================
// Processing
void initSignal(Signal *s, float frequency, float amplitude, Wave_f func, float sample_rate)
{
    s->freq = frequency;
    s->amp = amplitude;
    s->signature = func;
    s->phase = 0.0f;
    // Omega is how much the phase increases per sample/tick
    s->omega = (2.0f * PI * s->freq) / sample_rate;
}
//[=======================================================================================================================================
// Rendering/Displaying
void draw_scanlines(float width, float height, int gap_dist, Color color)
{
    for (int x = 0; x <= width; x += gap_dist)
    {
        DrawLine(x, 0, x, height, color);
    }
    for (int y = 0; y <= height; y += gap_dist)
    {
        DrawLine(0, y, width, y, color);
    }
}
void draw_ui(int pos_x, int pos_y, int font_size, Color c)
{
    // UI label (Evangelion style)
    DrawText("NERV SIGNAL MONITOR", pos_x, pos_y, font_size, c);
}

void draw_axis(int screenWidth, int screenHeight, int line_thickness, Color c)
{
    Vec2 x_axis = {.x = 0, .y = 0.9 * screenHeight};
    Vec2 x_end = {.x = screenWidth, .y = x_axis.y};
    Vec2 y_axis = {.x = .10 * screenWidth, .y = 0};
    Vec2 y_end = {.x = y_axis.x, .y = screenHeight};
    // x
    DrawLineEx(x_axis, x_end, line_thickness, c);
    float speed = 100.0f; // pixels per second
    static float offset = 0.0f;
    offset += speed * GetFrameTime();
    for (size_t i = 0; i < x_end.x; i += 50)
    {
        int dash_h = 10;
        Vec2 v0 = {
            .x = i - (int)offset % 50,
            .y = x_axis.y - dash_h};
        Vec2 v1 = {
            .x = v0.x,
            .y = v0.y + 2 * dash_h};
        DrawLineEx(v0, v1, line_thickness / 2, GREEN);
    }

    // y
    DrawLineEx(y_axis, y_end, line_thickness, c);
    for (size_t i = 0; i < x_end.x; i += 50)
    {
        int dash_w = 10;
        Vec2 v0 = {
            .x = y_axis.x - dash_w,
            .y = i};
        Vec2 v1 = {
            .x = v0.x + 2 * dash_w,
            .y = v0.y};
        DrawLineEx(v0, v1, line_thickness / 2, GREEN);
    }
    if (offset < 0)
        offset += 50;
}
void draw_waveform(Sample *s, int width, int height, Color c)
{
    for (int i = 0; i < width; i++)
    {
        // map pixel x → sample index
        int idx = (s->write_head + i * FFT_SIZE / width) % FFT_SIZE;
        float sample = s->samples[idx];
        // map -1..1 amplitude to screen y
        int y = (int)(height / 2 - sample * height / 2);
        DrawPixel(i, y, c);
    }
}
#define TRAIL_LEN 300
#define BEZIER_SIZE 2
void draw_angle(Sample *s, int width, int height, Color *colors)
{
    static float smooth_re = 0.0f;
    static float smooth_im = 0.0f;
    static float smooth_mag = 0.0f;
    static float peak_mag = 0.001f;
    static Vector2 trail[TRAIL_LEN] = {0};
    static int trail_head = 0;

    float alpha = 0.04f;
    float re = s->samples[(s->write_head - 1 + FFT_SIZE) % FFT_SIZE];
    float im = s->samples[s->write_head];
    smooth_re = alpha * re + (1.0f - alpha) * smooth_re;
    smooth_im = alpha * im + (1.0f - alpha) * smooth_im;

    float phase = atan2f(smooth_im, smooth_re);
    float raw_mag = sqrtf(smooth_re * smooth_re + smooth_im * smooth_im);
    smooth_mag = 0.1f * raw_mag + 0.9f * smooth_mag;

    // Self-calibrating peak normalization
    peak_mag *= 0.999f;
    if (raw_mag > peak_mag)
        peak_mag = raw_mag;
    float mag_normalized = smooth_mag / peak_mag;
    mag_normalized = mag_normalized > 1.0f ? 1.0f : (mag_normalized < 0.0f ? 0.0f : mag_normalized);

    float t_sec = TIME * DT;

    Vector2 origin = {width / 2.0f, height / 2.0f};

    // Slow origin drift
    float drift_r = 5.0f;
    origin.x += drift_r * sinf(t_sec * 0.3f);
    origin.y += drift_r * cosf(t_sec * 0.47f); // different freq = non-repeating lissajous drift

    // Breathing radius
    float radius = fminf(width, height) * 0.3f;
    radius *= (1.0f + 0.018f * sinf(t_sec * 0.7f));

    float needle_len = radius * mag_normalized;

    Vector2 tip = {
        origin.x + cosf(phase) * needle_len,
        origin.y - sinf(phase) * needle_len};

    // Fading trail — color shifts red when needle is long (loud)
    trail[trail_head] = tip;
    trail_head = (trail_head + 1) % TRAIL_LEN;

    for (int i = 0; i < TRAIL_LEN - 1; i++)
    {
        int a = (trail_head + i) % TRAIL_LEN;
        int b = (trail_head + i + 1) % TRAIL_LEN;
        float t = (float)i / TRAIL_LEN;
        // if (needle_len > 60.0f) trail_color = NERV_ALERT_RED;
        // else if (needle_len > 30.0f && needle_len <= 60.0f) trail_color = NERV_MAP_ORANGE;
        // else trail_color = NERV_INTERFACE_BLUE;
        Color trail_color = (Color){
            .r = NERV_MAP_ORANGE.r,
            .g = NERV_MAP_ORANGE.g,
            .b = NERV_MAP_ORANGE.b,
            // .a = (unsigned char)(255 * t * t)
            .a = 255
        };
        if (trail[a].x == 0 && trail[a].y == 0
            || trail[b].x == 0 && trail[b].y == 0) 
        {
            continue; // skip uninitialized points
        }
        DrawLineBezier(trail[a], trail[b], BEZIER_SIZE, trail_color);
    }
    Vec2 left_l = {
        .x = BOUND_LEFT,
        .y = tip.y
    };
    Vec2 right_l = {
        .x = BOUND_RIGHT,
        .y = tip.y
    };
    DrawLineBezier(left_l, tip, BEZIER_SIZE, NERV_MAP_ORANGE);
    DrawLineBezier(right_l, tip, BEZIER_SIZE, NERV_MAP_ORANGE);
    TIP_Y = tip.y;
}
//[=======================================================================================================================================]]
void draw_l()
{
    DrawLineBezier((Vector2){0, RES_Y-RES_Y/9}, (Vector2){BOUND_LEFT, TIP_Y}, BEZIER_SIZE, NERV_MAP_ORANGE);
}
void draw_r()
{
    DrawLineBezier((Vector2){BOUND_RIGHT, TIP_Y}, (Vector2){RES_X, RES_Y-RES_Y/9}, BEZIER_SIZE, NERV_MAP_ORANGE);
}
void draw_signals()
{
    // draw_waveform(&g_sample, RES_X, RES_Y, NERV_SYNC_GRAPH);
    Color angle_colors[3] = {NERV_ALERT_RED, NERV_MAP_ORANGE, NERV_INTERFACE_BLUE};
    draw_angle(&g_sample, RES_X, RES_Y, angle_colors);
    draw_l();
    draw_r();
}

void _draw()
{
    draw_signals();
    draw_axis(RES_X, RES_Y, 5, NERV_MAGI_AMBER);
    // draw_scanlines(RES_X, RES_Y, 2, BLACK);
}