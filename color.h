#pragma once
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    Color() : r(0), g(0), b(0), a(255) {}

    Color(int red, int green, int blue, int alpha = 255) {
        r = static_cast<Uint8>(std::min(std::max(red, 0), 255));
        g = static_cast<Uint8>(std::min(std::max(green, 0), 255));
        b = static_cast<Uint8>(std::min(std::max(blue, 0), 255));
        a = static_cast<Uint8>(std::min(std::max(alpha, 0), 255));
    }

    Color(float red, float green, float blue, float alpha = 1.0f) {
        r = glm::clamp(static_cast<Uint8>(red * 255), Uint8(0), Uint8(255));
        g = glm::clamp(static_cast<Uint8>(green * 255), Uint8(0), Uint8(255));
        b = glm::clamp(static_cast<Uint8>(blue * 255), Uint8(0), Uint8(255));
        a = glm::clamp(static_cast<Uint8>(alpha * 255), Uint8(0), Uint8(255));
    }

    // Overload the + operator to add colors
    Color operator+(const Color& other) const {
        return Color(
            std::min(255, int(r) + int(other.r)),
            std::min(255, int(g) + int(other.g)),
            std::min(255, int(b) + int(other.b)),
            std::min(255, int(a) + int(other.a))
        );
    }

    // Overload the * operator to scale colors by a factor
    Color operator*(float factor) const {
        return Color(
            glm::clamp(static_cast<Uint8>(r * factor), Uint8(0), Uint8(255)),
            glm::clamp(static_cast<Uint8>(g * factor), Uint8(0), Uint8(255)),
            glm::clamp(static_cast<Uint8>(b * factor), Uint8(0), Uint8(255)),
            glm::clamp(static_cast<Uint8>(a * factor), Uint8(0), Uint8(255))
        );
    }

    // Overload the * operator to multiply colors component-wise
    Color operator*(const Color& other) const {
        return Color(
            glm::clamp(static_cast<Uint8>(r * (float)other.r / 255), Uint8(0), Uint8(255)),
            glm::clamp(static_cast<Uint8>(g * (float)other.g / 255), Uint8(0), Uint8(255)),
            glm::clamp(static_cast<Uint8>(b * (float)other.b / 255), Uint8(0), Uint8(255)),
            glm::clamp(static_cast<Uint8>(a * (float)other.a / 255), Uint8(0), Uint8(255))
        );
    }


    // Friend function to allow float * Color
    friend Color operator*(float factor, const Color& color);
};