#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>

struct Ball {
    float x, y;
    float vx, vy;
    int radius;
    SDL_Color color;
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Bouncing Balls with Collisions",
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
        b.vx = (rand() % 200 - 100) / 50.0f;
        b.vy = (rand() % 200 - 100) / 50.0f;
        b.radius = 10 + rand() % 15;
        b.color = { (Uint8)(rand() % 255), (Uint8)(rand() % 255), (Uint8)(rand() % 255), 255 };
        balls.push_back(b);
    }

    bool running = true;
    float gravity = 0.4f;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        // ---- Physics update ----
        for (auto& b : balls) {
            b.vy += gravity;
            b.x += b.vx;
            b.y += b.vy;

            // Bounce off walls
            if (b.x - b.radius < 0) { b.x = b.radius; b.vx *= -1; }
            if (b.x + b.radius > 800) { b.x = 800 - b.radius; b.vx *= -1; }
            if (b.y + b.radius > 600) {
                b.y = 600 - b.radius;
                b.vy *= -0.8f;
            }
            if (b.y - b.radius < 0) { b.y = b.radius; b.vy *= -1; }
        }

        // ---- Ball-to-ball collision ----
        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                Ball& A = balls[i];
                Ball& B = balls[j];

                float dx = B.x - A.x;
                float dy = B.y - A.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                float minDist = A.radius + B.radius;

                if (dist < minDist && dist > 0) {
                    // Push them apart
                    float overlap = 0.5f * (minDist - dist);
                    A.x -= overlap * (dx / dist);
                    A.y -= overlap * (dy / dist);
                    B.x += overlap * (dx / dist);
                    B.y += overlap * (dy / dist);

                    // Swap their velocity directions (approx elastic collision)
                    float nx = dx / dist;
                    float ny = dy / dist;

                    float kx = A.vx - B.vx;
                    float ky = A.vy - B.vy;
                    float p = 2 * (nx * kx + ny * ky) / 2; // equal mass

                    A.vx -= p * nx;
                    A.vy -= p * ny;
                    B.vx += p * nx;
                    B.vy += p * ny;
                }
            }
        }

        // ---- Rendering ----
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
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
