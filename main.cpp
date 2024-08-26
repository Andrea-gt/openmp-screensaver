#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <cstdlib>
#include <string>
#include <vector>
#include <omp.h>
#include "point.h"
#include "color.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct Dvd {
    glm::vec2 velocity;
    Color color;
};

SDL_Renderer* renderer;

void drawPoint(glm::vec2 position, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
}

void render(const std::vector<Point>& points) {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw background points
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            drawPoint(glm::vec2(x, y), Color(255, 255, 0));
        }
    }

    // Draw points from vector
    for (const auto& point : points) {
        int opacity = static_cast<int>(point.opacity * 255);
        drawPoint(glm::vec2(point.x, point.y), Color(point.r, point.g, point.b, opacity));
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Load points from CSV file
    std::vector<Point> points = read_points_csv("G:\\Paralle\\openmp-screensaver\\output.csv");

    // Print first point
    std::cout << "First point: (" << points[0].x << ", " << points[0].y << ")" << std::endl;

    // Create a window
    SDL_Window* window = SDL_CreateWindow("FPS: 0", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;
    bool rendered = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        if (!rendered) {
            // Render the frame
            render(points);

            // Present the renderer
            SDL_RenderPresent(renderer);

            rendered = true;  // Draw only once
        }

        frameCount++;

        // Calculate and display FPS
        if (SDL_GetTicks() - currentTime >= 1000) {
            currentTime = SDL_GetTicks();
            std::string title = "FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
