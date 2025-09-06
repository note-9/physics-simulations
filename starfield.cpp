// starfield.cpp
// Compile: g++ -std=c++17 starfield.cpp -O2 -lSDL2 -o starfield

#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <iostream>

struct Star {
    float x, y, z; // 3D coordinates, z = depth
};

template <typename T>
T clamp(T value, T low, T high) {
    return (value < low) ? low : (value > high ? high : value);
}

int main(int argc, char** argv) {
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    const int N_STARS = 1000;
    const float FOV = 300.0f;       // camera field-of-view scale
    const float SPEED = 60.0f;      // units per second for forward movement (optional)
    const float ROT_SPEED = 0.6f;   // radians per second rotation speed

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Rotating Starfield",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // random generator for star initial positions
    std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> distX(-WIDTH, WIDTH);
    std::uniform_real_distribution<float> distY(-HEIGHT, HEIGHT);
    std::uniform_real_distribution<float> distZ(1.0f, FOV * 4.0f);

    // initialize stars
    std::vector<Star> stars;
    stars.reserve(N_STARS);
    for (int i = 0; i < N_STARS; ++i) {
        stars.push_back({ distX(rng)*0.5f, distY(rng)*0.5f, distZ(rng) });
    }

    bool running = true;
    SDL_Event e;

    auto last_time = std::chrono::high_resolution_clock::now();
    float angle = 0.0f;     // rotation angle around Z
    float forward = 0.0f;   // optional forward motion

    while (running) {
        // time
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = now - last_time;
        last_time = now;
        float dt = delta.count();
        if (dt <= 0.0f) dt = 1.0f/60.0f;

        // events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                else if (e.key.keysym.sym == SDLK_UP) forward += 50.0f;
                else if (e.key.keysym.sym == SDLK_DOWN) forward -= 50.0f;
                else if (e.key.keysym.sym == SDLK_SPACE) forward = 0.0f;
            }
        }

        // update rotation
        angle += ROT_SPEED * dt;

        // clear (black)
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // center
        float cx = WIDTH * 0.5f;
        float cy = HEIGHT * 0.5f;

        // precompute sin/cos
        float ca = std::cos(angle);
        float sa = std::sin(angle);

        for (auto &s : stars) {
            // rotate star around Z axis (X,Y rotate)
            float rx = s.x * ca - s.y * sa;
            float ry = s.x * sa + s.y * ca;
            float rz = s.z;

            // optional forward motion (move stars closer)
            rz -= forward * dt;
            if (rz < 0.5f) {
                // recycle star to far plane with new random x,y
                s.x = distX(rng) * 0.5f;
                s.y = distY(rng) * 0.5f;
                s.z = distZ(rng);
                continue;
            }
            s.z = rz; // write back depth change

            // perspective projection: screen_x = (x / z) * scale + cx
            float proj = FOV / rz;
            float sx = rx * proj + cx;
            float sy = ry * proj + cy;

            // if outside screen, you can wrap or skip drawing
            if (sx < -50 || sx > WIDTH + 50 || sy < -50 || sy > HEIGHT + 50) {
                // optionally recycle far-away / out-of-view stars
                // we'll sometimes just draw them when in view
            }

            // compute brightness/size based on depth (closer => brighter, larger)
            float depthNorm = clamp((4.0f * FOV - rz) / (4.0f * FOV), 0.0f, 1.0f);
            int size = 1 + int(depthNorm * 3.5f); // 1..4 pixels
            Uint8 bright = (Uint8)(80 + depthNorm * 175); // 80..255

            // draw star as small rect (faster than circle)
            SDL_Rect rrect;
            rrect.w = size;
            rrect.h = size;
            rrect.x = int(sx - size/2);
            rrect.y = int(sy - size/2);

            SDL_SetRenderDrawColor(ren, bright, bright, bright, 255);
            SDL_RenderFillRect(ren, &rrect);
        }

        // optional HUD text (no SDL_ttf here) - draw a small indicator using renderer
        // draw a faint center cross to see rotation center
        SDL_SetRenderDrawColor(ren, 40, 40, 40, 255);
        SDL_RenderDrawLine(ren, cx-8, cy, cx+8, cy);
        SDL_RenderDrawLine(ren, cx, cy-8, cx, cy+8);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
