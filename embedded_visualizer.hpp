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
        int x;              // X position in the window
        int y;              // Y position in the window
        int width;        // Visualizer width
        int height;       // Visualizer height
        int bar_count;     // Number of bars
        SDL_Color background_color;
        SDL_Color bar_color;
        float smoothing_factor;
        
        Config() : x(0), y(0), width(400), height(200), bar_count(32), 
                   background_color({0, 0, 0, 255}), bar_color({0, 255, 0, 255}), 
                   smoothing_factor(0.2f) {}
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