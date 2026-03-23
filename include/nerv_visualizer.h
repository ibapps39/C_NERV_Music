#pragma once

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h> // Required for malloc/free
#include <stdio.h>
#include <stdbool.h>
// Cooley-Tukey radix-2 DIT FFT, in-place, power-of-2 length
#include <math.h>
#include <stddef.h>

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
#define TRAIL_LEN 200
#define BEZIER_SIZE 2

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

const int FFT_SIZE = 2048; // FFT_SIZE must be a power of 2

//[=======================================================================================================================================]
// CUSTOM TYPES
//[=======================================================================================================================================]
struct Vec2
{
    float x, y, z;
};
typedef Vector2 Vec2;
typedef struct Parallelogram
{
    Vec2 pos;
    float width;
    float height;
    float skew;
} Parallelogram;

// Signal structure
typedef float (*Wave_f)(float);
typedef struct Signal
{
    float freq;  // Fundamental frequency (Hz)
    float amp;   // Linear amplitude (0.0 to 1.0)
    float phase; // Current accumulator (0.0 to 2*PI)
    float omega; // Angular frequency (computed: 2 * PI * freq / sampleRate)
    Wave_f signature;
} Signal;
// structurre
typedef struct { float re, im; } Complex;
typedef struct Sample
{
    float samples[FFT_SIZE]; // Precomputed samples for one second of audio at the given frequency
    int write_head;
    int sample_rate;
} Sample;
Sample g_sample = {0};
static Complex g_fft_bins[FFT_SIZE]; // persistent, no alloc per frame


typedef enum FONTS
{
    MAT_PRO_EB,
    MAT_CLASSIC,
    MAT_STD,
    JP_MAT_STD,
    JP_MAT_CLASSIC,
    BOLD_DS_DIGITAL
} FONTS;

typedef struct { // AI
    int   bin;
    float re, im;
    float mag;
} PeakBin;

typedef struct { // AI
    float phase;
    float raw_mag;
    float smooth_mag;
} PhaseState;

typedef struct RectState{ // AI
    Vector2 tip;
    bool    is_transient;
} RectState;

//[=======================================================================================================================================]
// GLOBALS
//[=======================================================================================================================================]

// Globals
// this should all be a struct, but we ball
int RES_X, RES_Y, TIME;
int DEFAULT_SAMPLE_RATE = 44100;
float BOUND_LEFT;
float BOUND_RIGHT;
float TIP_Y;
float TIP_X;

float DT;
float DWR; // change in width resolution
float DHR; // change in height resolution
float DR;  // change in resolution
float PEAK_SAMPLE;
float MIN_SAMPLE;
int FFT_RADIUS;
float Y_AXIS;
int ANGEL;

// [=======================================================================================================================================]
// FONTS
// [=======================================================================================================================================]
void load_nerv_fonts(Font* fonts, int** cjk_out, int* cp_count_out)
{
    // Define the CJK range
    int start = 0x4E00;
    int end = 0x9FFF;
    *cp_count_out = end - start + 1;
    
    // Allocate and fill codepoints array
    *cjk_out = (int *)malloc((*cp_count_out) * sizeof(int));
    for (int i = 0; i < *cp_count_out; i++)
    {
        (*cjk_out)[i] = start + i;
    }

    // Load Basic Fonts
    fonts[MAT_PRO_EB]  = LoadFont("resources/FOT-Matisse-Pro-EB.otf");
    fonts[MAT_CLASSIC] = LoadFont("resources/EVA-Matisse_Classic.ttf");
    fonts[MAT_STD]     = LoadFont("resources/EVA-Matisse_Standard.ttf");

    // Load Heavy JP Fonts with Codepoints
    fonts[JP_MAT_STD]     = LoadFontEx("resources/EVA-Matisse_Standard.ttf", 64, *cjk_out, *cp_count_out);
    fonts[JP_MAT_CLASSIC] = LoadFontEx("resources/EVA-Matisse_Classic.ttf", 64, *cjk_out, *cp_count_out);

    fonts[BOLD_DS_DIGITAL] = LoadFont("resources/ds_digital/DS-DIGIB.TTF"); // clock font
}
// [=======================================================================================================================================]
// FFT
// [=======================================================================================================================================]
// Bit-reversal permutation — scrambles input into the order the butterfly needs
static void bit_reverse(Complex *x, int n) { // AI
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) {
            Complex tmp = x[i]; x[i] = x[j]; x[j] = tmp;
        }
    }
}

// In-place FFT — x[] is overwritten with complex frequency bins
void fft(Complex *x, int n) { // AI
    bit_reverse(x, n);

    // len = current butterfly span, doubles each stage
    for (int len = 2; len <= n; len <<= 1) {
        float ang = -2.0f * (float)M_PI / len;
        Complex wlen = { cosf(ang), sinf(ang) }; // principal twiddle for this stage

        for (int i = 0; i < n; i += len) {
            Complex w = { 1.0f, 0.0f }; // running twiddle, starts at W^0

            for (int j = 0; j < len / 2; j++) {
                Complex u = x[i + j];
                // w * x[i+j+len/2] — complex multiply
                Complex v = {
                    w.re * x[i+j+len/2].re - w.im * x[i+j+len/2].im,
                    w.re * x[i+j+len/2].im + w.im * x[i+j+len/2].re
                };
                x[i + j]          = (Complex){ u.re + v.re, u.im + v.im };
                x[i + j + len/2]  = (Complex){ u.re - v.re, u.im - v.im };

                // Advance twiddle: w *= wlen
                float wr = w.re * wlen.re - w.im * wlen.im;
                float wi = w.re * wlen.im + w.im * wlen.re;
                w.re = wr; w.im = wi;
            }
        }
    }
}

// Magnitude of bin k (what you actually visualize)
static inline float fft_mag(Complex c) { // AI
    return sqrtf(c.re * c.re + c.im * c.im);
}

void compute_fft(Sample *s) //AI
{
    // Copy ring buffer into FFT input, correcting for write_head offset
    // Apply a Hann window to suppress spectral leakage at bin edges
    for (int i = 0; i < FFT_SIZE; i++)
    {
        int idx = (s->write_head + i) % FFT_SIZE;
        float raw = s->samples[idx];

        // Hann window: w(i) = 0.5 * (1 - cos(2π·i / (N-1)))
        float window = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * i / (FFT_SIZE - 1)));

        g_fft_bins[i].re = raw * window;
        g_fft_bins[i].im = 0.0f;
    }
    fft(g_fft_bins, FFT_SIZE);
}

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
void window_monitor(bool is_resized)
{
    if (is_resized)
    {
        RES_X = GetScreenWidth();
        RES_Y = GetScreenHeight();
        DWR = RES_X / 700.0;
        DHR = RES_Y / 400.0;
    }
    DT = GetFrameTime();
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
    Y_AXIS = 0.9 * screenHeight;
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
        DrawLineEx(v0, v1, line_thickness / 2, NERV_ALERT_RED);
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
        DrawLineEx(v0, v1, line_thickness / 2, NERV_ALERT_RED);
    }
    if (offset < 0)
        offset += 50;
}
//[=======================================================================================================================================]]
// AI
//[=======================================================================================================================================]]
//[=======================================================================================================================================]]
// ── DSP 

static PeakBin find_peak_bin(void) // AI
{
    PeakBin p = { .bin = 5, .re = 0, .im = 0, .mag = 0.0f };
    for (int k = 5; k < 80; k++) {
        float m = fft_mag(g_fft_bins[k]);
        if (m > p.mag) {
            p.mag = m;
            p.bin = k;
        }
    }
    p.re = g_fft_bins[p.bin].re;
    p.im = g_fft_bins[p.bin].im;
    return p;
}

// Returns smoothed phase and magnitude. Owns its own static storage.
static PhaseState smooth_phase(float re, float im) // AI
{
    static float smooth_re  = 0.0f;
    static float smooth_im  = 0.0f;
    static float smooth_mag = 0.0f;

    const float alpha = 0.04f;
    smooth_re  = alpha * re  + (1.0f - alpha) * smooth_re;
    smooth_im  = alpha * im  + (1.0f - alpha) * smooth_im;

    float raw_mag = sqrtf(smooth_re * smooth_re + smooth_im * smooth_im);
    smooth_mag = 0.1f * raw_mag + 0.9f * smooth_mag;

    return (PhaseState){
        .phase      = atan2f(smooth_im, smooth_re),
        .raw_mag    = raw_mag,
        .smooth_mag = smooth_mag,
    };
}

static float normalize_mag(float raw_mag, float smooth_mag, float *out_peak) // AI
{
    static float peak_mag = 0.001f;
    peak_mag *= 0.999f;
    if (raw_mag > peak_mag) peak_mag = raw_mag;
    *out_peak = peak_mag;          // caller writes PEAK_SAMPLE
    float n = smooth_mag / peak_mag;
    return n > 1.0f ? 1.0f : (n < 0.0f ? 0.0f : n);
}

static float compute_radius(int width, int height) // AI
{
    float r = fminf(width, height) * 0.3f;
    return r * (1.0f + 0.018f * sinf(0.7f));
}

static Vector2 compute_tip(Vector2 origin, float phase,
                            float mag_normalized, float radius) // AI
{
    float needle_len = radius * mag_normalized;
    float noise      = 0.01f * radius * (sinf(1.3f) + cosf(0.9f));
    return (Vector2){
        .x = origin.x + cosf(phase) * needle_len,
        .y = origin.y - sinf(phase) * (needle_len + noise),
    };
}

// Returns a read-only view of the circular trail buffer.
// (trail_head is also out so callers can iterate in order.)
static const Vector2 *update_trail(Vector2 tip, int *out_head) // AI
{
    static Vector2 trail[TRAIL_LEN] = {0};
    static int     trail_head       = 0;

    trail[trail_head] = tip;
    trail_head        = (trail_head + 1) % TRAIL_LEN;
    *out_head         = trail_head;
    return trail;
}

static void draw_needle(Vector2 tip) // AI
{
    Vector2 left  = { BOUND_LEFT,  BOUND_LEFT  - tip.y };
    Vector2 right = { BOUND_RIGHT, BOUND_RIGHT - tip.y };
    DrawLineBezier(left,  tip, BEZIER_SIZE, NERV_MAP_ORANGE);
    DrawLineBezier(right, tip, BEZIER_SIZE, NERV_MAP_ORANGE);
}

// Maps phase [-π, π] and mag_normalized [0,1] onto a rectangle's interior.
// Phase → X axis, mag → Y axis, with an edge-snap on loud hits.
static RectState compute_rect_tip(Vector2 origin, float phase,
                                   float mag_normalized, float raw_mag,
                                   float rect_w, float rect_h) // AI
{
    static float snap_x      = 0.0f;  // current snapped x offset
    static float snap_y      = 0.0f;
    static float last_raw    = 0.0f;
    static int   edge_lock   = -1;    // -1 = free, 0=top,1=right,2=bottom,3=left
    

    float half_w = rect_w * 0.5f;
    float half_h = rect_h * 0.5f;


    // Phase maps [-π, π] → [-half_w, half_w]
    float x = (phase / (float)M_PI) * half_w;

    static float prev_raw = 0.0f;
    float dy     = (raw_mag - prev_raw) * 0.4f;   // velocity, not position
    prev_raw     = raw_mag;
    static float y_accum = 0.0f;
    y_accum = y_accum * 0.92f + dy;               // leaky integrator
    float y = y_accum * half_h;

    // Transient detection: sudden spike in raw_mag
    float delta      = raw_mag - last_raw;
    bool  transient  = delta > (last_raw * 0.6f + 20.0f);
    last_raw         = raw_mag;
    

    if (transient) {
        // Snap tip to a random edge, biased by which quadrant phase puts us in
        edge_lock = (int)(fabsf(phase) * 1.9f / (float)M_PI * 4.0f) % 4;
        // Inject chaos offset that decays next frame
        snap_x = ((phase > 0) ? 1.0f : -1.0f) * half_w * 0.85f;
        snap_y = ((mag_normalized > 0.5f) ? 1.0f : -1.0f) * half_h * 0.85f;
    } else {
        // Decay snap back toward computed position
        snap_x = snap_x * 0.88f + x * 0.12f;
        snap_y = snap_y * 0.88f + y * 0.12f;
        edge_lock = -1;
    }

    // If edge-locked, clamp the tip to that edge
    float out_x = snap_x;
    float out_y = snap_y;
    switch (edge_lock) {
        case 0: out_y = -half_h; break;          // top edge
        case 1: out_x =  half_w; break;          // right edge
        case 2: out_y =  half_h; break;          // bottom edge
        case 3: out_x = -half_w; break;          // left edge
        default: break;
    }

    return (RectState){
        .tip = (Vector2){
            .x = origin.x + out_x,
            .y = origin.y + out_y,
        },
        .is_transient = transient,
    };
}

static void draw_rect_trail(const Vector2 *trail, int head, bool is_transient) // AI
{
    for (int i = 0; i < TRAIL_LEN - 1; i++) {
        int a = (head + i)     % TRAIL_LEN;
        int b = (head + i + 1) % TRAIL_LEN;
        if ((trail[a].x == 0 && trail[a].y == 0) ||
            (trail[b].x == 0 && trail[b].y == 0)) continue;

        float thickness = is_transient
            ? BEZIER_SIZE * (1.0f + (1.0f - (float)i / TRAIL_LEN) * 3.0f)
            : BEZIER_SIZE;

        Color c = { NERV_MAP_ORANGE.r, NERV_MAP_ORANGE.g,
                    NERV_MAP_ORANGE.b, 255 };
        DrawLineBezier(trail[a], trail[b], thickness, c);
    }
}

// Debug: draw the bounding rect so you can see the field
static void draw_rect_bounds(Vector2 origin, float rect_w, float rect_h) // AI
{
    Rectangle r = {
        .x      = origin.x - rect_w * 0.5f,
        .y      = origin.y - rect_h * 0.5f,
        .width  = rect_w,
        .height = rect_h,
    };
    DrawRectangleLinesEx(r, 1, (Color){ NERV_MAP_ORANGE.r,
                                        NERV_MAP_ORANGE.g,
                                        NERV_MAP_ORANGE.b, 40 });
}

void draw_angle(Sample *s, int width, int height, Color *colors) // AI
{
    (void)s; (void)colors;

    PeakBin    peak   = find_peak_bin();
    PhaseState phase  = smooth_phase(peak.re, peak.im);
    float      mag_n  = normalize_mag(phase.raw_mag, phase.smooth_mag, &PEAK_SAMPLE);

    // Rectangle is 60% of the shorter screen dimension
    float rect_w = fminf(width, height) * 0.6f;
    float rect_h = fminf(width, height) * 0.45f;

    Vector2  origin    = { width / 2.0f, height / 2.0f };
    RectState rs       = compute_rect_tip(origin, phase.phase, mag_n,
                                          phase.raw_mag, rect_w, rect_h);

    int            trail_head;
    const Vector2 *trail = update_trail(rs.tip, &trail_head);

    draw_rect_trail(trail, trail_head, rs.is_transient);
    draw_needle(rs.tip);

    FFT_RADIUS = (int)(rect_w * 0.5f);
    TIP_X      = rs.tip.x;
    TIP_Y      = rs.tip.y;
    ANGEL      = (phase.raw_mag > 20) ? 1 : 0;
}
//[=======================================================================================================================================]]
//[=======================================================================================================================================]]
//[=======================================================================================================================================]]

void draw_l()
{
    Vec2 v0 = {
        .x = 0,
        .y = RES_Y - RES_Y / 9};
    Vec2 v1 = {
        .x = BOUND_LEFT,
        .y = BOUND_LEFT - TIP_Y};
    DrawLineBezier(v0, v1, BEZIER_SIZE, NERV_MAP_ORANGE);
    ANGEL = (v1.y > RES_Y/2) ? 1 : 0;
}
void draw_r()
{
    Vec2 v0 = {
        .x = BOUND_RIGHT,
        .y =  BOUND_RIGHT - TIP_Y};
    Vec2 v1 = {
        .x = RES_X,
        .y = RES_Y - RES_Y / 9};
    DrawLineBezier(v0, v1, BEZIER_SIZE, NERV_MAP_ORANGE);
}

void draw_parallelogram(Vec2 pos, float width, float height, float skew, Color color)
{
    Vec2 p1 = pos;
    Vec2 p2 = {pos.x + width, pos.y};
    Vec2 p3 = {pos.x + width + skew, pos.y + height};
    Vec2 p4 = {pos.x + skew, pos.y + height};

    DrawTriangle(p1, p2, p3, color);
    DrawTriangle(p1, p3, p4, color);
}
void draw_parallelogram_para(Parallelogram p, Color color)
{
    float x = p.pos.x;
    float y = p.pos.y;
    float w = p.width;
    float h = p.height;
    float s = p.skew;

    Vector2 p1 = {x, y};
    Vector2 p2 = {x + w, y};
    Vector2 p3 = {x + w + s, y + h};
    Vector2 p4 = {x + s, y + h};

    DrawTriangle(p1, p4, p3, color); // left triangle  (CCW)
    DrawTriangle(p1, p3, p2, color); // right triangle (CCW)
}

void draw_header(Vec2 pos, float width, float height, Color bar_c, Color backg_c)
{
    Parallelogram background = {
        .pos = pos,
        .width = width,
        .height = height,
        .skew = 0};
    draw_parallelogram_para(background, backg_c);

    float scroll_speed = width * 0.2f;
    static float pos_x = 0.0f;
    pos_x += scroll_speed * GetFrameTime();

    const int num_bars = 20;
    float bar_width = background.width / num_bars;
    float bar_height = background.height;
    float offset = fmodf(pos_x, bar_width * 2); // wraps every 1 bar period

    for (size_t i = 0; i < num_bars + 1; i++) // +2 covers edges
    {
        Parallelogram p = {
            .pos =
                {
                    .x = (i * bar_width * 2) - offset,
                    .y = 0},
            .width = bar_width,
            .height = background.height,
            .skew = -(bar_width * 0.5f)};
        draw_parallelogram_para(p, bar_c);
    }
}

void draw_overlay(int width, int height) //, int FFT_RADIUS)
{
    float origin = width / 2.0f;
    float slice = width / 3.0f;
    for (size_t i = origin - slice; i < width + slice; i += slice)
    {
        Vec2 v = {
            .x = i,
            .y = 0};
        Vec2 v2 = {
            .x = i + 1,
            .y = height};
        float center_x = i + slice / 2.0f;
        float center_y = height / 2.0f;

        float size = 5.0f;

        // horizontal line of cross
        DrawLine(center_x - size, center_y,
                 center_x + size, center_y,
                 GREEN);

        // vertical line of cross
        DrawLine(center_x, center_y - size,
                 center_x, center_y + size,
                 GREEN);
        // this is fine
        DrawLineBezier(v, v2, 1, GREEN);
    }
}

void draw_cover(Vec2 pos, float width, float height, Color bar_c, Color backg_c)
{
    float scroll_speed = height * 0.15f;
    static float pos_y = 0.0f;
    pos_y -= scroll_speed * GetFrameTime();

    const int num_bars = 12;            // 12 bars + 12 gaps = height
    int bar_height = height / num_bars; // bar + gap both = bar_height
    float period = bar_height * 2.0f;

    float offset = fmodf(pos_y, period);
    if (offset > 0)
        offset -= period;

    for (int i = -1; i < num_bars * 2 + 1; i++)
    {
        float bar_y = pos.y + (i * period) + offset;

        Rectangle r =
            {
                .width = width,
                .height = bar_height,
                .x = pos.x,
                .y = bar_y};
        DrawRectangleRec(r, bar_c);
    }
}
void draw_angel_warning(int warning, Font f_eng, Font f_jap)
{
    if (!warning) return;

    Rectangle r = {
        .width  = RES_X / 2.5f,
        .height = RES_Y / 2.5f,
        .x      = RES_X / 4.0f,
        .y      = RES_Y / 5.0f
    };

    Color r_color = NERV_INTERFACE_BLUE;

    DrawRectangleRoundedLines(r, 0.2f, 80, r_color);

    // --- fit JP text to top half of rectangle ---
    float target_w = r.width * 0.8f;   // leave 10% padding each side
    float target_h = r.height * 0.45f; // top half

    // Start at a guess, then scale to fit width
    float jp_size = target_h;
    Vector2 jp_measured = MeasureTextEx(f_jap, "天使", jp_size, 0);
    if (jp_measured.x > target_w)
        jp_size *= target_w / jp_measured.x; // shrink to fit width

    jp_measured = MeasureTextEx(f_jap, "天使", jp_size, 0);
    Vec2 jp_pos = {
        .x = r.x + (r.width - jp_measured.x) / 2.0f,  // centered
        .y = r.y + r.height * 0.05f
    };

    // --- fit ENG text to bottom half ---
    float eng_size = target_h;
    Vector2 eng_measured = MeasureTextEx(f_eng, "Angle", eng_size, 0);
    if (eng_measured.x > target_w)
        eng_size *= target_w / eng_measured.x;

    eng_measured = MeasureTextEx(f_eng, "Angle", eng_size, 0);
    Vec2 eng_pos = {
        .x = r.x + (r.width - eng_measured.x) / 2.0f, // centered
        .y = r.y + r.height * 0.5f
    };

    DrawTextEx(f_jap, "天使", jp_pos, jp_size, 0, r_color);
    DrawTextEx(f_eng, "Angle", eng_pos, eng_size, 0, r_color);
}
void play_angle_warning_sound(int warning, Sound* sound) {
    if (!warning) return;
    static double last_angle_warning_time = 0.0f;
    double now = GetTime();
    if(now - last_angle_warning_time < 5) return;
    last_angle_warning_time = now;
    SetSoundVolume(*sound, 1.0f);
    PlaySound(*sound);
}
void draw_signals()
{
    compute_fft(&g_sample);
    Color angle_colors[3] = {NERV_ALERT_RED, NERV_MAP_ORANGE, NERV_INTERFACE_BLUE};
    draw_angle(&g_sample, RES_X, RES_Y, angle_colors);
    draw_l();
    draw_r();
}
void draw_song_time(Music* audio_file, Font clock_font)
{
    Color clock_color = ANGEL ? NERV_INTERFACE_BLUE : NERV_ALERT_RED;
    Vec2 pos = 
    {
        .x = RES_X*.85,
        .y = RES_Y*.75
    };
    Rectangle r = {
        .width = RES_X*.1,
        .height = RES_Y*.1,
        .x = pos.x,
        .y = pos.y
    };
    Vec2 tri_tip = {
        .x = pos.x - r.width/3,
        .y = pos.y + r.height/2
    };
    Vec2 tri_1 = {
        .x = pos.x,
        .y = pos.y + r.height
    };
    DrawRectangleRec(r, clock_color);
    DrawTriangle(pos, tri_tip, tri_1, clock_color);
    Vec2 tri_0 = {
        .x = pos.x + r.width,
        .y = pos.y
    };
    tri_1 = (Vec2){
        .x = pos.x + r.width,
        .y = pos.y + r.height
    };

    tri_tip = (Vec2){
        .x = tri_0.x + r.width/3,
        .y = tri_0.y + r.height/2
    };
    DrawTriangle(tri_0, tri_1, tri_tip, clock_color);

    // get song length & position (keep as float for precision)
    float song_length = GetMusicTimeLength(*audio_file);
    float song_position = GetMusicTimePlayed(*audio_file);

    // Better time format (MM:SS) - highly recommended
    int milis   = (int)(song_position * 1000) % 1000;
    int min_pos = (int)song_position / 60;
    int sec_pos = (int)song_position % 60;
    int min_len = (int)song_length / 60;
    int sec_len = (int)song_length % 60;
    char* time_str = TextFormat("%02d:%02d:%02d: / %02d:%02d", min_pos, sec_pos, milis, min_len, sec_len);

    // === TEXT SIZING & CENTERING ===
    float spacing = 2.0f;
    float time_size = r.height * 0.95f;      // a bit smaller than full height so it fits nicely

    Vector2 time_measured = MeasureTextEx(clock_font, time_str, time_size, spacing);

    // Auto-shrink if too wide
    if (time_measured.x > r.width * 0.95f)
    {
        time_size *= (r.width * 0.95f) / time_measured.x;
        time_measured = MeasureTextEx(clock_font, time_str, time_size, spacing); // re-measure!
    }

    // Proper center (top-left of text = center of rectangle minus half text size)
    Vector2 time_pos = {
        .x = r.x + (r.width  - time_measured.x) / 2.0f,
        .y = r.y + (r.height - time_measured.y) / 2.0f
    };

    DrawTextEx(clock_font, time_str, time_pos, time_size, spacing, BLACK);
}
void draw_psycho_lable(Font font) 
{
    DrawTextEx(font, "PSYCHOGRAPHIC DISPLAY", (Vec2){RES_X *.14, RES_Y*.1}, RES_Y / 35, 0.0f, NERV_MAGI_AMBER);
    DrawRectangleRoundedLines(
        (Rectangle){
            .width = RES_X*.23, 
            .height = RES_Y*.1, 
            .x = RES_X*.135, 
            .y = RES_Y*.07}, 
            0.5f, 
            4, 
            NERV_MAGI_AMBER
        );
}


void _draw(Font fonts[], Music* audio_file)
{
    Vec2 header = {
        .x = RES_X,
        .y = 0,
    };
    Vec2 cover_l = {
        .x = BOUND_LEFT,
        .y = 0};
    Vec2 cover_r = {
        .x = BOUND_RIGHT,
        .y = 0};
    draw_signals();
    draw_axis(RES_X, RES_Y, 5, NERV_MAGI_AMBER);


    draw_overlay(RES_X - RES_X / 4, RES_Y);
    draw_angel_warning(ANGEL, fonts[MAT_CLASSIC], fonts[JP_MAT_CLASSIC]);
    draw_song_time(audio_file, fonts[BOLD_DS_DIGITAL]);
    
    draw_psycho_lable(fonts[MAT_PRO_EB]);

    // draw_header(header, RES_X, RES_Y / 9, EVA_02_RED, NERV_MAGI_AMBER);
    //  draw_grid_bars_slants((Vec2){RES_X/2-RES_X*0.5f, RES_Y-RES_Y*0.5f});
    //   draw_scanlines(RES_X, RES_Y, 2, BLACK);
}