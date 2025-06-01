#ifndef SDL2_EMBEDDED_VISUALIZER_HPP
#define SDL2_EMBEDDED_VISUALIZER_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <cstdint>

namespace sdl2vis {

class EmbeddedVisualizer {
public:
    struct Config {
        int x = 0;              // X position in the window
        int y = 0;              // Y position in the window
        int width = 400;        // Visualizer width
        int height = 200;       // Visualizer height
        int bar_count = 32;     // Number of bars
        SDL_Color background_color = {0, 0, 0, 255};
        SDL_Color bar_color = {0, 255, 0, 255};
        float smoothing_factor = 0.2f;
    };

    EmbeddedVisualizer(SDL_Renderer* renderer, const Config& config = Config{});
    
    void update_audio_data(const float* data, size_t size);
    
    void render();

private:
    SDL_Renderer* m_renderer;
    Config m_config;
    std::vector<float> m_spectrum_data;
    std::vector<float> m_smoothed_data;

    void draw_visualization();
    void smooth_data();
};

} // namespace sdl2vis

#endif