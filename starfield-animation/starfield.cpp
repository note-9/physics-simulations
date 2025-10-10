#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

struct Star {
    float x, y, z;
};

int main() {
    const int WIDTH = 1920;
    const int HEIGHT = 1080;
    const int NUM_STARS = 1000000;
    const float NEAR_Z = 0.1f;
    const float FAR_Z = 100.0f;
    const float FORWARD_SPEED = 20.0f;
    const float ROTATION_SPEED = 0.5f; // radians per second
    const float FOV = 90.0f;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL2 Rotating Starfield",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::vector<Star> stars(NUM_STARS);
    for (auto &s : stars) {
        s.x = (std::rand() / (float)RAND_MAX - 0.5f) * 2.0f * WIDTH;
        s.y = (std::rand() / (float)RAND_MAX - 0.5f) * 2.0f * HEIGHT;
        s.z = NEAR_Z + (std::rand() / (float)RAND_MAX) * (FAR_Z - NEAR_Z);
    }

    Uint32 lastTicks = SDL_GetTicks();
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        Uint32 currentTicks = SDL_GetTicks();
        float dt = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Update and draw stars
        float angle = ROTATION_SPEED * dt;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        float scale = (WIDTH / 2.0f) / std::tan(FOV * 0.5f * M_PI / 180.0f);

        for (auto &s : stars) {
            // Move forward
            s.z -= FORWARD_SPEED * dt;
            if (s.z <= NEAR_Z) {
                s.x = (std::rand() / (float)RAND_MAX - 0.5f) * 2.0f * WIDTH;
                s.y = (std::rand() / (float)RAND_MAX - 0.5f) * 2.0f * HEIGHT;
                s.z = FAR_Z;
            }

            // Rotate around Z
            float newX = s.x * cosA - s.y * sinA;
            float newY = s.x * sinA + s.y * cosA;
            s.x = newX;
            s.y = newY;

            // Project to screen
            float px = (s.x / s.z) * scale + WIDTH / 2.0f;
            float py = (s.y / s.z) * scale + HEIGHT / 2.0f;

            // Brightness based on depth
            float brightness = 1.0f / (s.z * 0.05f);
            if (brightness > 1.0f) brightness = 1.0f;
            Uint8 color = static_cast<Uint8>(brightness * 255);

            // Size of star depends on depth
            int size = static_cast<int>(3.0f / s.z * 50); 
            if (size < 1) size = 1;
            if (size > 4) size = 4;

            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                SDL_SetRenderDrawColor(renderer, color, color, color, 255);
                SDL_Rect rect = { (int)px, (int)py, size, size };
                SDL_RenderFillRect(renderer, &rect);
            }

        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
