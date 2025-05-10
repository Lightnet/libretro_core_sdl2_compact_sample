#include <libretro.h>
#include <SDL.h> // SDL2-compat provides SDL2 API
#include <string.h>
#include <stdio.h>

// Global Libretro callbacks
static retro_environment_t environ_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

// Core state
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* screen_texture = NULL;
static int running = 0;

// Core information
void retro_get_system_info(struct retro_system_info* info) {
    memset(info, 0, sizeof(struct retro_system_info));
    info->library_name = "My Libretro Core";
    info->library_version = "1.0";
    info->valid_extensions = "";
    info->need_fullpath = 0;
    info->block_extract = 0;
}

// System A/V information
void retro_get_system_av_info(struct retro_system_av_info* info) {
    info->geometry.base_width = 640;
    info->geometry.base_height = 480;
    info->geometry.max_width = 640;
    info->geometry.max_height = 480;
    info->geometry.aspect_ratio = 4.0f / 3.0f;
    info->timing.fps = 60.0;
    info->timing.sample_rate = 44100.0;
}

// Initialize the core
void retro_init(void) {
    printf("Initializing core...\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow(
        "My Libretro Core",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_HIDDEN // Hidden until rendering
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Create a texture for rendering
    screen_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        640, 480
    );
    if (!screen_texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    running = 1;
}

// Deinitialize the core
void retro_deinit(void) {
    if (screen_texture) SDL_DestroyTexture(screen_texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    screen_texture = NULL;
    renderer = NULL;
    window = NULL;
    SDL_Quit();
    running = 0;
}

// Set environment callback
void retro_set_environment(retro_environment_t cb) {
    environ_cb = cb;
    bool support = true;
    environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &support);
    // Set pixel format to XRGB8888
    unsigned pixel_format = RETRO_PIXEL_FORMAT_XRGB8888;
    environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format);
}

// Set video refresh callback
void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }

// Set audio sample callback
void retro_set_audio_sample(retro_audio_sample_t cb) { audio_cb = cb; }

// Set audio sample batch callback
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }

// Set input callbacks
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

// Serialization functions
size_t retro_serialize_size(void) { return 0; }
bool retro_serialize(void* data, size_t size) { return false; }
bool retro_unserialize(const void* data, size_t size) { return false; }

// Reset (not used)
void retro_reset(void) {}

// Run one frame
void retro_run(void) {
    if (!running) return;

    printf("Running frame...\n");

    // Lock the texture for direct pixel access
    void* pixels;
    int pitch;
    if (SDL_LockTexture(screen_texture, NULL, &pixels, &pitch) != 0) {
        fprintf(stderr, "SDL_LockTexture failed: %s\n", SDL_GetError());
        return;
    }

    // Clear the texture to black (XRGB8888 format)
    uint32_t* pixel_data = (uint32_t*)pixels;
    for (int y = 0; y < 480; y++) {
        for (int x = 0; x < 640; x++) {
            pixel_data[y * (pitch / 4) + x] = 0xFF000000; // Black (0xAARRGGBB)
        }
    }

    // Draw a red rectangle (100,100,200,200)
    for (int y = 100; y < 300; y++) {
        for (int x = 100; x < 300; x++) {
            pixel_data[y * (pitch / 4) + x] = 0xFFFF0000; // Red (0xAARRGGBB)
        }
    }

    SDL_UnlockTexture(screen_texture);

    // Send frame to RetroArch
    video_cb(pixels, 640, 480, pitch);

    // Optional: Update SDL renderer for debugging (not needed for RetroArch)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {100, 100, 200, 200};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);

    // Poll input
    input_poll_cb();

    // No audio
    audio_batch_cb(NULL, 0);
}

// Load content (not used)
bool retro_load_game(const struct retro_game_info* game) {
    return true;
}

// Unload content
void retro_unload_game(void) {}

// Get API version
unsigned retro_api_version(void) { return RETRO_API_VERSION; }

// Set controller description (optional)
void retro_set_controller_port_device(unsigned port, unsigned device) {}

// Other unused callbacks
unsigned retro_get_region(void) { return RETRO_REGION_NTSC; }
void* retro_get_memory_data(unsigned id) { return NULL; }
size_t retro_get_memory_size(unsigned id) { return 0; }
bool retro_load_game_special(unsigned game_type, const struct retro_game_info* info, size_t num_info) { return false; }
void retro_cheat_reset(void) {}
void retro_cheat_set(unsigned index, bool enabled, const char* code) {}