cmake_minimum_required(VERSION 3.5)
project(BubbleScreensaver)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include FetchContent module
include(FetchContent)

# Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

# Fetch SDL2
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.2  # Cambia esto según la versión deseada
)
FetchContent_MakeAvailable(SDL2)

# Fetch SDL2_image
FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.6.3  # Cambia esto según la versión deseada
)

# START ADDITION
set(SDL2IMAGE_INSTALL OFF)
set(BUILD_SHARED_LIBS FALSE)
# END ADDITION

FetchContent_MakeAvailable(SDL2_image)

# Check for OpenMP support
find_package(OpenMP REQUIRED)

# Define your executable
add_executable(BubbleScreensaver main.cpp)
add_executable(BubbleScreensaverParallel mainParallel.cpp)

# Include directories for SDL2, SDL2_image, and utils
target_include_directories(BubbleScreensaver PRIVATE 
    ${SDL2_SOURCE_DIR}/include
    ${SDL2_image_SOURCE_DIR}/include  # Asegúrate de que esta ruta apunte correctamente al directorio que contiene SDL_image.h
    ${CMAKE_SOURCE_DIR}/utils  # Include the utils directory
)

target_include_directories(BubbleScreensaverParallel PRIVATE 
    ${SDL2_SOURCE_DIR}/include
    ${SDL2_image_SOURCE_DIR}/include  # Asegúrate de que esta ruta apunte correctamente al directorio que contiene SDL_image.h
    ${CMAKE_SOURCE_DIR}/utils  # Include the utils directory
)

# Link SDL2, SDL2_image, GLM, and OpenMP
target_link_libraries(BubbleScreensaver PRIVATE SDL2::SDL2-static SDL2_image::SDL2_image-static glm OpenMP::OpenMP_CXX)
target_link_libraries(BubbleScreensaverParallel PRIVATE SDL2::SDL2-static SDL2_image::SDL2_image-static glm OpenMP::OpenMP_CXX)

# Optional: Link SDL2 main if you want to avoid redefining `main` in SDL2
target_link_libraries(BubbleScreensaver PRIVATE SDL2::SDL2main)
target_link_libraries(BubbleScreensaverParallel PRIVATE SDL2::SDL2main)
