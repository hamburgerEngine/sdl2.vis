#include "embedded_visualizer.hpp"
#include <algorithm>
#include <cmath>

namespace sdl2vis {

EmbeddedVisualizer::EmbeddedVisualizer(SDL_Renderer* renderer, const Config& config)
    : m_renderer(renderer)
    , m_config(config) {
    m_spectrum_data.resize(config.bar_count, 0.0f);
    m_smoothed_data.resize(config.bar_count, 0.0f);
}

void EmbeddedVisualizer::update_audio_data(const float* data, size_t size) {
    const size_t samples_per_bar = size / m_config.bar_count;
    
    for (int i = 0; i < m_config.bar_count; ++i) {
        float sum = 0.0f;
        size_t start = i * samples_per_bar;
        size_t end = start + samples_per_bar;
        
        for (size_t j = start; j < end && j < size; ++j) {
            sum += std::abs(data[j]);
        }
        
        m_spectrum_data[i] = sum / samples_per_bar;
    }
    
    smooth_data();
}

void EmbeddedVisualizer::smooth_data() {
    for (int i = 0; i < m_config.bar_count; ++i) {
        m_smoothed_data[i] = m_smoothed_data[i] * (1.0f - m_config.smoothing_factor) +
                            m_spectrum_data[i] * m_config.smoothing_factor;
    }
}

void EmbeddedVisualizer::render() {
    SDL_Rect bg = {
        m_config.x,
        m_config.y,
        m_config.width,
        m_config.height
    };
    
    SDL_SetRenderDrawColor(
        m_renderer,
        m_config.background_color.r,
        m_config.background_color.g,
        m_config.background_color.b,
        m_config.background_color.a
    );
    SDL_RenderFillRect(m_renderer, &bg);
    
    draw_visualization();
}

void EmbeddedVisualizer::draw_visualization() {
    const int bar_width = m_config.width / m_config.bar_count;
    const int max_height = m_config.height * 0.8f;
    
    SDL_SetRenderDrawColor(
        m_renderer,
        m_config.bar_color.r,
        m_config.bar_color.g,
        m_config.bar_color.b,
        m_config.bar_color.a
    );
    
    for (int i = 0; i < m_config.bar_count; ++i) {
        int bar_height = static_cast<int>(m_smoothed_data[i] * max_height);
        SDL_Rect bar = {
            m_config.x + (i * bar_width),
            m_config.y + m_config.height - bar_height,
            bar_width - 1,
            bar_height
        };
        SDL_RenderFillRect(m_renderer, &bar);
    }
}

} // namespace sdl2vis 