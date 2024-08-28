#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include <omp.h>
#include "point.h"
#include "color.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct Bubble
{
    glm::vec2 direction;
    glm::vec2 position;
    SDL_Texture *texture;
    int limit_x;
    int limit_y;
};

SDL_Renderer *renderer;
std::vector<Bubble> bubbles;

// Centro de la pantalla
glm::vec2 center(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

// Generador de números aleatorios
std::random_device rd;  // Obtener una semilla de hardware
std::mt19937 gen(rd()); // Inicializar el generador con la semilla
std::uniform_int_distribution<> dis(1, 5); // Distribución uniforme para el rango de 1 a 5

void spawn_bubble()
{
    Bubble bubble;
    bubble.position = center;

    // Generar valores aleatorios para x y y en el rango de 1 a 5
    int x_random = dis(gen);  // Genera un número entre 1 y 5
    int y_random = dis(gen);  // Genera un número entre 1 y 5

    // Crear un vector de dirección aleatorio dentro del rango
    bubble.direction = glm::vec2(x_random, y_random);

    // Normalizar el vector de dirección
    bubble.direction = glm::normalize(bubble.direction);

    // Multiplicar la dirección por la velocidad
    bubble.direction *= 0.1f;

    // Cargar la imagen y crear la textura
    SDL_Surface *surface = IMG_Load("G:\\Paralle\\openmp-screensaver\\image\\bubble.png");
    if (!surface)
    {
        SDL_Log("Unable to load image: %s", IMG_GetError());
        return;
    }

    bubble.texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!bubble.texture)
    {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        return;
    }

    // Establecer los límites de la burbuja según el tamaño de la imagen
    SDL_QueryTexture(bubble.texture, NULL, NULL, &bubble.limit_x, &bubble.limit_y);
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

    // Dibujar las burbujas usando sus texturas
    for (auto &bubble : bubbles)
    {
        SDL_Rect destRect;
        destRect.x = static_cast<int>(bubble.position.x);
        destRect.y = static_cast<int>(bubble.position.y);
        destRect.w = bubble.limit_x;
        destRect.h = bubble.limit_y;

        SDL_RenderCopy(renderer, bubble.texture, NULL, &destRect);
    }

    // Presentar el renderizador en la pantalla
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    // Pedir al usuario cuantos quiere
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <number of bubbles>" << std::endl;
        return 1;
    }

    int num_bubbles = std::stoi(argv[1]);
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Inicializar SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

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

    // Cargar puntos desde el archivo CSV y crear la burbuja
    #pragma omp parallel for
    for (int i = 0; i < num_bubbles; i++)
    {
        spawn_bubble();
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
    for (auto &bubble : bubbles)
    {
        SDL_DestroyTexture(bubble.texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
