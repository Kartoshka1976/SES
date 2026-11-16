// SES — Social Economic Simulator (skeleton)
// Minimal SDL2 window: grid map + colored pawn agents that wander each tick.

#include <SDL.h>

#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

namespace {

constexpr int kCellSize = 24;
constexpr int kMapW = 24;
constexpr int kMapH = 18;
constexpr int kWindowW = kMapW * kCellSize;
constexpr int kWindowH = kMapH * kCellSize;
constexpr int kTickMs = 200; // agent step interval
constexpr int kFpsDelayMs = 16;

struct Color {
  Uint8 r, g, b;
};

struct Pawn {
  int x = 0;
  int y = 0;
  Color color{};
};

struct Resource {
  int x = 0;
  int y = 0;
};

int Clamp(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

int RandInt(int lo, int hi) {
  return lo + (std::rand() % (hi - lo + 1));
}

void DrawGrid(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 40, 48, 56, 255);
  SDL_RenderClear(renderer);

  // Slightly lighter ground cells
  for (int y = 0; y < kMapH; ++y) {
    for (int x = 0; x < kMapW; ++x) {
      const bool odd = ((x + y) & 1) != 0;
      if (odd) {
        SDL_SetRenderDrawColor(renderer, 48, 58, 68, 255);
        SDL_Rect cell{x * kCellSize, y * kCellSize, kCellSize, kCellSize};
        SDL_RenderFillRect(renderer, &cell);
      }
    }
  }

  // Grid lines
  SDL_SetRenderDrawColor(renderer, 70, 82, 94, 255);
  for (int x = 0; x <= kMapW; ++x) {
    const int px = x * kCellSize;
    SDL_RenderDrawLine(renderer, px, 0, px, kWindowH);
  }
  for (int y = 0; y <= kMapH; ++y) {
    const int py = y * kCellSize;
    SDL_RenderDrawLine(renderer, 0, py, kWindowW, py);
  }
}

void DrawResource(SDL_Renderer* renderer, const Resource& res) {
  // Green square = food / resource cell
  SDL_SetRenderDrawColor(renderer, 46, 160, 90, 255);
  SDL_Rect r{
      res.x * kCellSize + 4,
      res.y * kCellSize + 4,
      kCellSize - 8,
      kCellSize - 8};
  SDL_RenderFillRect(renderer, &r);
}

void DrawPawn(SDL_Renderer* renderer, const Pawn& p) {
  SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, 255);
  const int cx = p.x * kCellSize + kCellSize / 2;
  const int cy = p.y * kCellSize + kCellSize / 2;
  const int radius = kCellSize / 3;

  // Filled circle approximated with horizontal spans
  for (int dy = -radius; dy <= radius; ++dy) {
    const int dx = static_cast<int>(std::sqrt(static_cast<double>(radius * radius - dy * dy)));
    SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
  }
}

void StepPawn(Pawn& p, const Resource& res) {
  // Prefer walking toward the resource; sometimes take a random step.
  if (RandInt(0, 100) < 70) {
    int dx = 0;
    int dy = 0;
    if (p.x < res.x) dx = 1;
    else if (p.x > res.x) dx = -1;
    if (p.y < res.y) dy = 1;
    else if (p.y > res.y) dy = -1;

    // Move on one axis at a time (simple, readable)
    if (dx != 0 && dy != 0) {
      if (RandInt(0, 1) == 0) dy = 0;
      else dx = 0;
    }
    p.x = Clamp(p.x + dx, 0, kMapW - 1);
    p.y = Clamp(p.y + dy, 0, kMapH - 1);
  } else {
    static const std::array<int, 4> kDx{{-1, 1, 0, 0}};
    static const std::array<int, 4> kDy{{0, 0, -1, 1}};
    const int dir = RandInt(0, 3);
    p.x = Clamp(p.x + kDx[dir], 0, kMapW - 1);
    p.y = Clamp(p.y + kDy[dir], 0, kMapH - 1);
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  std::srand(static_cast<unsigned>(std::time(nullptr)));

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(
      "SES — Social Economic Simulator",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      kWindowW,
      kWindowH,
      SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  }
  if (!renderer) {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  Resource food{kMapW / 2, kMapH / 2};

  const std::array<Color, 6> palette{{
      {220, 70, 70},
      {70, 140, 230},
      {240, 180, 50},
      {180, 90, 220},
      {50, 200, 200},
      {240, 120, 60},
  }};

  std::vector<Pawn> pawns;
  pawns.reserve(palette.size());
  for (std::size_t i = 0; i < palette.size(); ++i) {
    Pawn p;
    p.x = RandInt(0, kMapW - 1);
    p.y = RandInt(0, kMapH - 1);
    p.color = palette[i];
    pawns.push_back(p);
  }

  bool running = true;
  Uint32 last_tick = SDL_GetTicks();

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
          running = false;
        }
      }
    }

    const Uint32 now = SDL_GetTicks();
    if (now - last_tick >= static_cast<Uint32>(kTickMs)) {
      for (Pawn& p : pawns) {
        StepPawn(p, food);
      }
      // If someone reaches the resource, move it elsewhere
      for (const Pawn& p : pawns) {
        if (p.x == food.x && p.y == food.y) {
          food.x = RandInt(0, kMapW - 1);
          food.y = RandInt(0, kMapH - 1);
          break;
        }
      }
      last_tick = now;
    }

    DrawGrid(renderer);
    DrawResource(renderer, food);
    for (const Pawn& p : pawns) {
      DrawPawn(renderer, p);
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(kFpsDelayMs);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
