#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <vector>

struct Ball {
    float x, y;
    float vx, vy;
    int radius;
    SDL_Color color;
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Simple Bouncing Balls",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::srand(std::time(nullptr));

    const int numBalls = 10;
    std::vector<Ball> balls;
    for (int i = 0; i < numBalls; i++) {
        Ball b;
        b.x = rand() % 800;
        b.y = rand() % 600;
        b.vx = (rand() % 200 - 100) / 100.0f; // random speed
        b.vy = (rand() % 200 - 100) / 100.0f;
        b.radius = 10 + rand() % 10;
        b.color = { (Uint8)(rand() % 255), (Uint8)(rand() % 255), (Uint8)(rand() % 255), 255 };
        balls.push_back(b);
    }

    bool running = true;
    float gravity = 0.5f;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        // Physics update
        for (auto& b : balls) {
            b.vy += gravity;  // gravity
            b.x += b.vx;
            b.y += b.vy;

            // Bounce off walls
            if (b.x - b.radius < 0 || b.x + b.radius > 800)
                b.vx *= -1;
            if (b.y + b.radius > 600) {
                b.y = 600 - b.radius;
                b.vy *= -0.8f; // lose some speed when bouncing
            }
        }

        // Draw
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (auto& b : balls) {
            SDL_SetRenderDrawColor(renderer, b.color.r, b.color.g, b.color.b, 255);
            for (int w = -b.radius; w <= b.radius; w++) {
                for (int h = -b.radius; h <= b.radius; h++) {
                    if (w * w + h * h <= b.radius * b.radius)
                        SDL_RenderDrawPoint(renderer, (int)(b.x + w), (int)(b.y + h));
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
