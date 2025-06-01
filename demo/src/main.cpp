#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../../embedded_visualizer.hpp"
#include <vector>
#include <cmath>
#include <iostream>

void SDLCALL audio_postmix(void* udata, Uint8* stream, int len) {
    std::vector<float>* audioData = static_cast<std::vector<float>*>(udata);
    if (audioData) {
        const Sint16* samples = reinterpret_cast<const Sint16*>(stream);
        int num_samples = len / sizeof(Sint16);
        
        for (int i = 0; i < audioData->size() && i < num_samples/2; i++) {
            float left = samples[i*2] / 32768.0f;
            float right = samples[i*2+1] / 32768.0f;
            float combined = (left + right) * 0.5f;
            float amplified = std::copysign(std::pow(std::abs(combined), 0.7f), combined) * 2.5f;
            (*audioData)[i] = std::max(-1.0f, std::min(1.0f, amplified));
        }
    }
}

Mix_Music* try_load_music() {
    Mix_Music* music = Mix_LoadMUS("audio.mp3");
    if (!music) {
        std::cout << "Failed to load audio.mp3, trying audio.ogg..." << std::endl;
        music = Mix_LoadMUS("audio.ogg");
        if (!music) {
            std::cerr << "Failed to load audio files: " << Mix_GetError() << std::endl;
        }
    }
    return music;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "sdl2.vis demo lmao made by koshii/maybekoi",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        return 1;
    }

    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
        std::cerr << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        return 1;
    }

    Mix_Music* music = try_load_music();

    sdl2vis::EmbeddedVisualizer::Config visConfig;
    visConfig.x = 20;
    visConfig.y = 400;
    visConfig.width = 760;
    visConfig.height = 160;
    visConfig.bar_count = 48;
    visConfig.background_color = {30, 30, 30, 255};
    visConfig.bar_color = {0, 255, 100, 255};
    visConfig.smoothing_factor = 0.15f;

    auto visualizer = std::make_unique<sdl2vis::EmbeddedVisualizer>(renderer, visConfig);

    const size_t BUFFER_SIZE = 1024;
    std::vector<float> audioData(BUFFER_SIZE, 0.0f);
    Mix_SetPostMix(audio_postmix, &audioData);

    if (music) {
        Mix_PlayMusic(music, -1);
    }

    bool running = true;
    float t = 0.0f;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    if (music) {
                        if (Mix_PausedMusic()) {
                            Mix_ResumeMusic();
                        } else {
                            Mix_PauseMusic();
                        }
                    }
                }
            }
        }

        if (!music) {
            t += 0.016f;
            for (size_t i = 0; i < BUFFER_SIZE; i++) {
                float phase = t + i * 0.1f;
                float value = std::sin(phase) * 0.3f + 
                             std::sin(phase * 2.5f) * 0.2f +
                             std::sin(phase * 0.5f) * 0.15f;
                audioData[i] = value * 2.0f;
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        visualizer->update_audio_data(audioData.data(), audioData.size());
        visualizer->render();

        SDL_RenderPresent(renderer);
    }

    if (music) {
        Mix_FreeMusic(music);
    }
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
