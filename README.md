AI Generated README + personal edits (temporary but better than nothing)
# C_NERV_Music - Evangelion Themed Music Visualizer

An immersive, real-time music visualization application inspired by the NERV interface from Neon Genesis Evangelion. Built with C and Raylib, this project transforms audio input into dynamic visual displays featuring AT Field analysis, sync rate monitoring, and Angel detection systems.

# About the fonts
- The fonts are copyrighted, so, obviously I can't distrbute that. That said, there's stopping you front find & replac'ng the font enum with your own or just naming the fonts you wish to use the same as referenced here. 
- That said, for the japanese fonts you very likely need to change the loading code some. Iconographic languages can and often do have a huge number of glyphs (characters) -- so loading all would use considerably more resources. 

## Features

### 🎵 Audio Visualization
- Real-time FFT (Fast Fourier Transform) audio processing with Cooley-Tukey radix-2 DIT algorithm
- Dynamic waveform visualization with envelope-based amplitude mapping
- Peak frequency detection and phase tracking for audio signals
- Circular and rectangular visualization modes for frequency analysis

### 🎨 NERV Interface Design
- Authentic Evangelion UI color scheme (EVA-00 Blue, EVA-01 Purple, EVA-02 Red, NERV Interface Blue)
- Japanese and English font support with EVA Matisse typefaces
- Scanline effects and parallax scrolling UI elements
- Hexagonal background patterns with animated alert text
- **Bloom shader effect** for enhanced visual atmosphere

### ⚡ Advanced Visual Effects
- **AT Field Visualization**: Dynamic angle detection with needle and trail effects
- **Sync Rate Monitor**: Real-time synchronization percentage display
- **Pattern Blue Detection**: Visual and audio alerts for "Angel" detection
- **Psychographic Display**: Animated waveform analysis with convergence effects

### 🎮 Interactive Elements
- Real-time audio input processing via audio stream callback
- Mouse tracking for experimental visualization
- **Keyboard controls**: SPACE for play/pause, F for seeking, 1-2 for debug views
- Fullscreen mode by default with adaptive UI scaling
- Window resizing support with dynamic render texture updates

## System Requirements

- **OS**: Windows, Linux, or macOS
- **Dependencies**: Raylib graphics library
- **Audio**: Standard audio input/output support
- **Memory**: Minimum 256MB RAM
- **Display**: Fullscreen recommended (adaptive scaling supported)

## Installation

### Prerequisites
```bash
# Install Raylib (Ubuntu/Debian)
sudo apt-get install libraylib-dev

# Install Raylib (macOS)
brew install raylib

# Install Raylib (Windows)
# Download from https://www.raylib.com/
```

### Building from Source
```bash
# Clone the repository
git clone https://github.com/ibapps39/C_NERV_Music.git
cd C_NERV_Music

# Build the project
make

# Prepare audio file (see Configuration section)
mkdir -p putAudioFileHere # if not there already
cp /path/to/your/song.mp3 putAudioFileHere/

# Run the application
./NERV_Visualizer
```

## Configuration

### Audio Setup
1. Place your MP3 file in `putAudioFileHere/song.mp3`
2. Supported formats: MP3 (OGG, FLAC, WAV - via Raylib, not tested)

### Resource Files
Ensure the following files exist in the `resources/` directory(these are not provided as they are copyrighted):
- `FOT-Matisse-Pro-EB.otf` - Primary Japanese/English font
- `EVA-Matisse_Classic.ttf` - Classic NERV interface font
- `EVA-Matisse_Standard.ttf` - Standard UI font
- `DS-DIGIB.TTF` - Digital clock display font
- `Target_locked_tone.ogg` - Angel warning sound effect
- `shaders/bloom.fs` - Bloom post-processing shader

### Build Configuration
```makefile
# Example Makefile
CC = gcc
CFLAGS = -Wall -std=c99 -O2
LIBS = -lraylib -lm
SRC = src/main.c
TARGET = nerv_music

$(TARGET): $(SRC)
    $(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)
```

## Usage

### Basic Controls
- **SPACE**: Play/Pause music
- **F**: Seek forward 5 seconds (for testing different song sections)
- **1**: Toggle hex background overlay
- **2**: Toggle signal visualization
- **Window Resize**: Automatically adjusts UI scaling and render texture

### Visualization Timeline

The application responds to the audio timeline with different visual states:

#### Phase 1: Opening (0-10 seconds)
- Typing animation: "Shinji, get in the robot"
- Sync rate percentage display
- Waveform convergence visualization

#### Phase 2: Normal Operation (10-21.5 seconds)
- Sync rate increases over time
- Waveform alignment visualization
- Normal UI operation

#### Phase 3: Transition (21.5-30 seconds)
- Hex background activation
- Animated header bars
- Building tension visuals

#### Phase 4: Pattern Blue Alert (30-40 seconds)
- Angel detection system activates
- AT Field visualization with needle display
- "GET IN THE FUCKING ROBOT SHINJI" warning
- Angel warning overlay with Japanese text
- **Audio alert**: Target locked tone plays

#### Phase 5: Congratulations (40+ seconds)
- Multi-language congratulations messages
- Developer credit display
- Snow effect
- Easter egg messages from the series

## Technical Architecture

### Core Pipeline
```c
1. Audio Stream → Audio Callback → Sample Buffer (2048 samples)
2. FFT Processing → Frequency Domain Analysis  
3. Peak Detection → Dominant Frequency Identification
4. Phase Tracking → Angle/Magnitude Calculation
5. Bloom Shader → Post-processing effects
6. Visual Rendering → Dynamic UI Updates
```

### Key Components

#### Main Application Flow
```c
int main() {
    // Initialize fullscreen window
    InitWindow(0, 0, "NERV Sine Monitor");
    ToggleFullscreen();  // Start in fullscreen
    
    // Load resources
    load_nerv_fonts(fonts, &cjk_ptr, &cp_count);
    Shader bloom_s = LoadShader(NULL, "resources/shaders/bloom.fs");
    
    // Setup audio
    Music audio_file = LoadMusicStream("putAudioFileHere/song.mp3");
    AttachAudioStreamProcessor(audio_file.stream, audio_callback);
    
    // Main loop with render texture for shader effects
    while (!WindowShouldClose()) {
        UpdateMusicStream(audio_file);
        BeginTextureMode(target);
        _draw(fonts, &audio_file);
        EndTextureMode();
        
        BeginShaderMode(bloom_s);
        DrawTextureRec(target.texture, shader_rect, (Vector2){0,0}, WHITE);
        EndShaderMode();
    }
}
```

#### Signal Processing Pipeline
- **Audio Callback**: Collects samples into ring buffer
- **Hann Window**: Applied to reduce spectral leakage
- **In-place FFT**: Computes frequency domain representation
- **Peak Detection**: Finds dominant frequency (5-80Hz range)
- **Phase Smoothing**: Exponential moving average for stability
- **Magnitude Normalization**: Peak tracking for dynamic range

### Data Structures
```c
// Audio buffer management
typedef struct Sample {
    float samples[FFT_SIZE];     // Precomputed samples
    int write_head;               // Circular buffer position
    int sample_rate;              // Audio sample rate
} Sample;

// FFT processing
typedef struct Complex {
    float re, im;                 // Real and imaginary parts
} Complex;

// Visualization state
typedef struct PhaseState {
    float phase;                  // Current phase angle
    float raw_mag;                // Raw magnitude
    float smooth_mag;             // Smoothed magnitude
} PhaseState;
```

## Performance Optimization

- **FFT Optimization**: In-place bit-reversal and butterfly operations
- **Memory Management**: Static allocation for audio buffers and visualization data
- **Render Textures**: Off-screen rendering with bloom shader effect
- **Adaptive Scaling**: Resolution-independent UI calculations
- **Frame Rate**: Locked to 60 FPS for consistent performance

## Troubleshooting

### Common Issues & Solutions

**Audio not playing:**
```bash
# Check if audio file exists
ls putAudioFileHere/song.mp3

# Verify audio device initialization
# Add debug print after InitAudioDevice()
if (!IsAudioDeviceReady()) {
    TraceLog(LOG_ERROR, "Audio device failed to initialize");
}
```

**Missing fonts or shaders:**
- Ensure all resource files are in correct directories
- Check file permissions
- Verify shader compilation in console output

**Poor performance:**
- Reduce bloom shader quality
- Disable hex background (remove KEY_ONE check)
- Lower FFT_SIZE from 2048 to 1024

**Fullscreen issues:**
```c
// Modify main.c to start windowed mode
InitWindow(700, 400, "NERV Sine Monitor");
// Remove ToggleFullscreen() line
```

## Project Structure
```
C_NERV_Music/
├── include/
│   └── nerv_visualizer.h    # Main visualization header
├── src/
│   └── main.c                # Application entry point
├── resources/
│   ├── shaders/
│   │   └── bloom.fs          # Bloom post-processing
│   ├── fonts/                # EVA Matisse font files
│   └── Target_locked_tone.ogg # Alert sound
├── putAudioFileHere/
│   └── song.mp3              # Your audio file
└── Makefile
```

## Easter Eggs & References

The application contains numerous Evangelion references:
- **Pattern Blue**: Angel approach warning system
- **AT Field**: Absolute Terror Field visualization
- **MAGI**: Supercomputer system interface
- **LCL**: Liquid oxygen-like fluid visualization
- **Entry Plug**: Immersion-based UI design
- **Congratulations Scene**: End of Evangelion reference with multi-language messages
- **SEELE**:神秘组织 reference in post-40 second sequence

## Customization

### Modifying Visual Parameters
```c
// In nerv_visualizer.h
#define FFT_SIZE 4096          // Increase for better frequency resolution
#define TRAIL_LEN 300          // Longer trails for smoother animation
#define ANGEL_LIMIT 15.0f      // More sensitive Angel detection
```

### Color Customization
```c
// Modify NERV color palette
#define EVA_01_PURPLE (Color){180, 100, 220, 255}  // Adjust purple intensity
#define NERV_ALERT_RED (Color){255, 50, 50, 255}   // Brighter alerts
```

## Credits

- **Developer**: ibapps39
- **Graphics Library**: Raylib (raysan5)
- **Inspiration**: Neon Genesis Evangelion, Hideaki Anno
- **Fonts**: EVA Matisse fonts (Evangelion official typography)
- **Sound Effect**: Target locked tone from Evangelion series

## License
GNU Affero General Public License v3.0.
This project is for protfolio and fan art purposes. Evangelion is a registered trademark of Khara, Inc. All rights reserved. The code is provided as-is for these purposes. GNU Affero General Public License v3.0

## Contributing

Contributions welcome! Areas for improvement:
- Additional visualization modes (spectrogram, oscilloscope)
- Audio effect processing (reverb, delay)
- Network synchronization for multi-display setups
- MIDI controller support
- Recording/export functionality
- Support for multiple audio file formats
- Config file for customization

---

*"The fate of destruction is also the joy of rebirth."* - NERV Command Interface
