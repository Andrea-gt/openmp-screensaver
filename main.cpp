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

struct Bubble
{
    glm::vec2 direction;
    glm::vec2 position;
    std::vector<Point> points;
    int limit_x;
    int limit_y;
};

SDL_Renderer *renderer;

std::vector<Bubble> bubbles;

// Centro de la pantalla
glm::vec2 center(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

std::vector<std::vector<Color>> screenMatrix(SCREEN_WIDTH, std::vector<Color>(SCREEN_HEIGHT));

void drawPoint(glm::vec2 position, Color color, float opacity = 1.0f)
{
    if (position.x < 0 || position.x >= SCREEN_WIDTH || position.y < 0 || position.y >= SCREEN_HEIGHT)
    {
        return;
    }
    // Mezcla el color con la opacidad dada
    screenMatrix[position.x][position.y] = color * opacity + screenMatrix[position.x][position.y] * (1.0f - opacity);
}

void spawn_bubble()
{
    Bubble bubble;
    bubble.position = center;
    bubble.direction = glm::vec2(rand() % 3 - 1, rand() % 3 - 1); // Dirección aleatoria entre -1 y 1

    // Si la dirección es 0,0, establecerla en 1,0
    if (bubble.direction.x == 0 && bubble.direction.y == 0)
    {
        bubble.direction.x = 1;
    }
    
    // Normalizar el vector de dirección
    bubble.direction = glm::normalize(bubble.direction);

    // Cargar puntos desde el archivo CSV
    bubble.points = read_points_csv("G:\\Paralle\\openmp-screensaver\\image\\output.csv");

    bubble.limit_x = 86;
    bubble.limit_y = 86;
    bubbles.push_back(bubble);
}

void change_bubble_dir()
{
    for (auto &bubble : bubbles)
    {   
        // Cambiar dirección si la posición está fuera de los límites
        if (bubble.position.x <= 0 || bubble.position.x >= SCREEN_WIDTH - bubble.limit_x)
        {
            bubble.direction.x *= -1;
        }
        if (bubble.position.y <= 0 || bubble.position.y >= SCREEN_HEIGHT - bubble.limit_y)
        {
            bubble.direction.y *= -1;
        }
        // Mover la burbuja
        bubble.position += bubble.direction;
    }
}

void render()
{
    // Limpiar la pantalla
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    change_bubble_dir();

    // Rellenar la matriz con color negro
    #pragma omp parallel for
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {   
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            screenMatrix[x][y] = Color(0, 0, 0);
        }
    }

    // Dibujar puntos para las burbujas
    for (auto &bubble : bubbles)
    {
        
        for (auto &point: bubble.points) {
            drawPoint(glm::vec2(point.x, point.y) + bubble.position, Color(point.r, point.g, point.b), point.opacity);
        }
    }

    // Dibujar la matriz de pantalla en el renderizador
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            Color color = screenMatrix[x][y];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    // Presentar el renderizador en la pantalla
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Cargar puntos desde el archivo CSV
    spawn_bubble();

    // Crear una ventana
    SDL_Window *window = SDL_CreateWindow("FPS: 0",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Crear un renderizador
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
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

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Renderizar el frame
        render();
        frameCount++;

        // Calcular y mostrar FPS
        if (SDL_GetTicks() - currentTime >= 1000)
        {
            currentTime = SDL_GetTicks();
            std::string title = "FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    // Limpiar
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
