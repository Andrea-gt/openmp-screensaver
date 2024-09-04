/**
 * Bubble Screensaver
 *
 * @brief
 * This program creates a screensaver-like animation with bubbles that move around the screen. The bubbles 
 * originate from the center of the screen and move in random directions. Each bubble changes its color 
 * gradually over time. The program utilizes SDL2 for rendering, SDL_image for loading images, and OpenMP 
 * for parallel bubble generation.
 *
 * @author Valdez D., Flores A., Ramirez A. 
 * @date Sep 2024
 * @note Documentation generated with ChatGPT
 *
 * @usage:
 *  Compile the program with the required SDL2 and SDL_image libraries. 
 *  Run the program with the command:
 *      ./BubbleScreensaver <number of bubbles>
 *  Replace <number of bubbles> with the desired number of bubbles to display.
 *
 * @libraries:
 *  - SDL2
 *  - SDL_image
 *  - GLM (for vector mathematics)
 *  - OpenMP (for parallel processing)
**/

// SDL2 library for handling graphics, events, and window management
#include <SDL2/SDL.h>        // SDL main library
#include <SDL_image.h>      // SDL_image extension for handling image files

// GLM library for OpenGL mathematics (e.g., vectors and matrices)
#include <glm/glm.hpp>      // GLM core functions and types

// Standard C++ libraries for various functionalities
#include <vector>           // STL vector container
#include <iostream>         // For input and output operations
#include <string>           // For string handling

// Random number generation
#include <random>           // For random number generation

// OpenMP library for parallel programming (if needed for parallel execution)
#include <omp.h>            // OpenMP support for multi-threading

// Define screen dimensions
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1000;

// Define frames per second (FPS) and frame delay
int FPS;
int FRAME_DELAY;

// To handle collisions between bubbles
const int COLLISION_THRESHOLD = 10; // Set your desired threshold
const Uint32 COLLISION_TIME_PERIOD = 5000; // Time period in milliseconds

// Structure representing a bubble
struct Bubble
{
    glm::vec2 direction;      // Direction vector for the bubble's movement
    glm::vec2 position;       // Current position of the bubble
    SDL_Texture *texture;     // Texture of the bubble image
    int limit_x;              // Width of the bubble texture
    int limit_y;              // Height of the bubble texture
    SDL_Color color;          // Current color of the bubble
    SDL_Color targetColor;    // Target color for the bubble
    float colorChangeSpeed;   // Speed at which the color changes
    int collisionCount;       // Number of collisions detected
    Uint32 lastCollisionTime; // Time of the last collision detection
    bool isCollisionActive;     // Flag to activate/deactivate collision detection

    // Equality operator to compare two Bubble objects
    // Reference --> https://stackoverflow.com/questions/16843323/c-object-equality
    bool operator != (const Bubble &other) const
    {
        return (position == other.position && direction == other.direction &&
                color.r == other.color.r && color.g == other.color.g && color.b == other.color.b);
    }

};

struct BoundingCircle
{
    glm::vec2 center; // Center of the circle
    float radius;     // Radius of the circle
};

// Function to get the bounding circle of a bubble
// This function calculates the circular bounding area that approximates the bubble.
// It is used for more accurate collision detection when bubbles are round.
BoundingCircle getBoundingCircle(const Bubble &bubble)
{
    BoundingCircle circle;
    // Calculate the center of the bounding circle based on bubble position and texture dimensions
    circle.center = bubble.position + glm::vec2(bubble.limit_x / 2, bubble.limit_y / 2);
    // Radius of the bounding circle, chosen as the smallest dimension divided by 2
    circle.radius = std::min(bubble.limit_x, bubble.limit_y) / 2.0f;
    return circle;
}

// Function to check if two circles collide
// This function determines if two bounding circles overlap.
// It returns true if the circles intersect, indicating a collision.
bool isCollision(const BoundingCircle &circle1, const BoundingCircle &circle2)
{
    // Calculate the distance between the centers of the two circles
    float distance = glm::distance(circle1.center, circle2.center);
    // Check if the distance is less than the sum of the radii (collision condition)
    return distance < (circle1.radius + circle2.radius);
}

// Function to handle collision between two bubbles
// This function updates the direction of the bubbles if they collide,
// and increments their collision counts if collision handling is active.
void handleCollision(Bubble &bubble, Bubble &other, BoundingCircle &bubbleBound, BoundingCircle &otherBound) {
    // Check if collision handling is active for both bubbles
    if (bubble.isCollisionActive && other.isCollisionActive) {
        // Calculate the normal vector at the collision point
        glm::vec2 collision_normal = glm::normalize(otherBound.center - bubbleBound.center);
        // Calculate the dot product of the bubble's direction and collision normal
        float dot_product = glm::dot(bubble.direction, collision_normal);
        // Reflect the bubble's direction off the collision normal
        bubble.direction -= 2.0f * dot_product * collision_normal;

        // Update the collision count for both bubbles
        bubble.collisionCount++;
        other.collisionCount++;
    }
}

// Global variables
SDL_Renderer *renderer;            // SDL Renderer
std::vector<Bubble> bubbles;       // Vector containing all bubbles

// Random number generator
std::random_device rd;          // Obtain a seed from hardware
std::mt19937 gen(rd());         // Initialize the generator with the seed
std::uniform_int_distribution<> dis(-100, 100);     // Distribution for random direction values

std::uniform_int_distribution<> spawn_dis_x(100, 1700);   // Distribution for random spawn value in x
std::uniform_int_distribution<> spawn_dis_y(100, 700);   // Distribution for random spawn value in y
std::uniform_int_distribution<> color_dis(0, 255);      // Distribution for random color values

// Function to spawn a new bubble
void spawnBubble() {
    glm::vec2 spawn_point(spawn_dis_x(gen), spawn_dis_y(gen));
    Bubble bubble;
    bubble.position = spawn_point;

    // Generate random direction values within a range
    int x_random, y_random = 0;

    do {
        x_random = dis(gen);
        y_random = dis(gen);
    } while (x_random == 0 && y_random == 0); // Keep generating until it's not zero

    // Create a direction vector and normalize it
    bubble.direction = glm::vec2(x_random, y_random);
    bubble.direction = glm::normalize(bubble.direction);

    // Set a fixed speed for the bubble
    bubble.direction *= 1.0f;

    // Generate a random initial color for the bubble
    bubble.color.r = color_dis(gen);
    bubble.color.g = color_dis(gen);
    bubble.color.b = color_dis(gen);
    bubble.color.a = 255; // Full opacity

    // Generate a random target color for the bubble
    bubble.targetColor.r = color_dis(gen);
    bubble.targetColor.g = color_dis(gen);
    bubble.targetColor.b = color_dis(gen);
    bubble.targetColor.a = 255; // Full opacity

    // Set the color change speed
    bubble.colorChangeSpeed = 0.01f;

    // Load the bubble image and create a texture
    SDL_Surface *surface = IMG_Load("image/bubble.png");
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

    // Set the texture's dimensions as the bubble's limits
    SDL_QueryTexture(bubble.texture, NULL, NULL, &bubble.limit_x, &bubble.limit_y);
    bubbles.push_back(bubble);
}

// Function to change the direction of bubbles when they hit the screen borders
void changeBubbleDirection()
{
    #pragma omp parallel for
    for (int i = 0; i < bubbles.size(); i++)
    {
        auto &bubble = bubbles[i];
        BoundingCircle bubbleBound = getBoundingCircle(bubble);

        // Reverse direction if the bubble reaches the screen's edges
        if (bubble.position.x <= 0 || bubble.position.x >= SCREEN_WIDTH - bubble.limit_x)
        {
            bubble.direction.x *= -1;
        }
        if (bubble.position.y <= 0 || bubble.position.y >= SCREEN_HEIGHT - bubble.limit_y)
        {
            bubble.direction.y *= -1;
        }

        // Check for collisions with other bubbles
        for (int j = 0; j < bubbles.size(); j++) {
            if (i != j) { 
                BoundingCircle otherBound = getBoundingCircle(bubbles[j]);
                if (isCollision(bubbleBound, otherBound)) {
                    #pragma omp critical
                    {
                        handleCollision(bubble, bubbles[j], bubbleBound, otherBound);
                    }
                }
            }
        }

        // Move the bubble in the current direction
        bubble.position += bubble.direction;
    }
}

// Function to check and update collision states for all bubbles
// This function manages the activation of collision detection based on the bubble's collision history
// and the elapsed time since the last collision.
void checkCollisions() {
    Uint32 currentTime = SDL_GetTicks(); // Get the current time in milliseconds

    // Iterate over each bubble to check and update collision status
    #pragma omp parallel for
    for (int i = 0; i < bubbles.size(); i++)
    {
        auto &bubble = bubbles[i];
        // Check if the bubble's collision count exceeds the threshold
        // and if the time since the last collision is less than the specified period
        if (bubble.collisionCount > COLLISION_THRESHOLD && 
            currentTime - bubble.lastCollisionTime < COLLISION_TIME_PERIOD)
        {
            // Deactivate collision detection for this bubble
            bubble.isCollisionActive = false;
        }
        else
        {
            // Activate collision detection for this bubble
            bubble.isCollisionActive = true;
        }

        // Check if the time period since the last collision has passed
        if (currentTime - bubble.lastCollisionTime >= COLLISION_TIME_PERIOD)
        {
            // Reset the collision count and update the last collision time
            bubble.collisionCount = 0;
            bubble.lastCollisionTime = currentTime;
        }
    }
}

// Function to gradually change the bubble's color toward the target color
void updateBubbleColors()
{
    #pragma omp parallel for
    for (int i = 0; i < bubbles.size(); i++)
    {
        auto &bubble = bubbles[i];

        // If the current color is close to the target color, set a new target color
        if (abs(bubble.color.r - bubble.targetColor.r) < 1 &&
            abs(bubble.color.g - bubble.targetColor.g) < 1 &&
            abs(bubble.color.b - bubble.targetColor.b) < 1)
        {
            bubble.color.r = bubble.targetColor.r;
            bubble.color.g = bubble.targetColor.g;
            bubble.color.b = bubble.targetColor.b;

            // Generate new target colors in a thread-safe way
            #pragma omp critical
            {
                bubble.targetColor.r = color_dis(gen);
                bubble.targetColor.g = color_dis(gen);
                bubble.targetColor.b = color_dis(gen);
            }
        }
        else
        {
            // Interpolate the bubble's color toward the target color
            bubble.color.r = (bubble.color.r < bubble.targetColor.r)
                ? std::min(bubble.color.r + bubble.colorChangeSpeed * 255, static_cast<float>(bubble.targetColor.r))
                : std::max(bubble.color.r - bubble.colorChangeSpeed * 255, static_cast<float>(bubble.targetColor.r));

            bubble.color.g = (bubble.color.g < bubble.targetColor.g)
                ? std::min(bubble.color.g + bubble.colorChangeSpeed * 255, static_cast<float>(bubble.targetColor.g))
                : std::max(bubble.color.g - bubble.colorChangeSpeed * 255, static_cast<float>(bubble.targetColor.g));

            bubble.color.b = (bubble.color.b < bubble.targetColor.b)
                ? std::min(bubble.color.b + bubble.colorChangeSpeed * 255, static_cast<float>(bubble.targetColor.b))
                : std::max(bubble.color.b - bubble.colorChangeSpeed * 255, static_cast<float>(bubble.targetColor.b));
        }
    }
}

// Function to render bubbles on the screen
void render()
{
    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    changeBubbleDirection(); // Update bubble directions
    checkCollisions(); // Check and deactivate collisions if necessary
    updateBubbleColors(); // Update bubble colors

    // Draw each bubble
    for (auto &bubble : bubbles)
    {
        SDL_Rect destRect;
        destRect.x = static_cast<int>(bubble.position.x);
        destRect.y = static_cast<int>(bubble.position.y);
        destRect.w = bubble.limit_x;
        destRect.h = bubble.limit_y;

        // Apply color modulation to the bubble texture
        SDL_SetTextureColorMod(bubble.texture, bubble.color.r, bubble.color.g, bubble.color.b);

        SDL_RenderCopy(renderer, bubble.texture, NULL, &destRect);
    }

    // Present the rendered frame on the screen
    SDL_RenderPresent(renderer);
}

// Main function
int main(int argc, char *argv[])
{
    std::cout << "Initializing SDL" << std::endl;

    // Ensure the correct number of arguments is provided
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <Number of Bubbles> <Target FPS>" << std::endl;
        return 1;
    }

    int num_bubbles = std::stoi(argv[1]);
    FPS = std::stoi(argv[2]);
    FRAME_DELAY = 1000 / FPS;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a window
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

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Spawn the bubbles
    for (int i = 0; i < num_bubbles; i++)
    {
        spawnBubble();
    }

    // Variables for frame rate calculation
    SDL_Event event;
    bool running = true;
    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;
    float totalFrameTime = 0;
    int framesAccumulated = 0;
    float endAvg;

    // Main loop
    while (running)
    {
        Uint32 frameStart = SDL_GetTicks();
        
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Render the bubbles
        render();
        frameCount++;

        // Calculate the frame time for this frame
        Uint32 frameEnd = SDL_GetTicks();
        float frameTime = static_cast<float>(frameEnd - frameStart);
        totalFrameTime += frameTime; // Accumulate total frame time
        framesAccumulated++; // Count the number of frames accumulated

        // Update the FPS and average frame time in the window title
        if (frameEnd - currentTime >= 1000)
        {
            float avgFrameTime = totalFrameTime / framesAccumulated;
            std::string title = "FPS: " + std::to_string(frameCount) + " | Avg Frame Time: " + std::to_string(avgFrameTime) + " ms";
            SDL_SetWindowTitle(window, title.c_str());
            endAvg = avgFrameTime;
            frameCount = 0;
            totalFrameTime = 0; // Reset total frame time for the next second
            framesAccumulated = 0; // Reset frame accumulation count
            currentTime = frameEnd; // Update the current time
        }

        Uint32 frameDelay = SDL_GetTicks() - frameStart;
        if (frameDelay < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frameDelay);
        }
    }

    // Clean up resources
    for (auto &bubble : bubbles)
    {
        SDL_DestroyTexture(bubble.texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    std::cout << "End Average Frame Time: " << std::to_string(static_cast<float>(endAvg));

    return 0;
}