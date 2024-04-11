#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL_rect.h>
#include <SDL_render.h>

#define WHITE 0xFF
#define BLACK 0x00
#define OPAQUE 0xFF
#define TRANSPARENT 0x00

#define FILE_SHEET_SPACESHIPS "../assets/stitchedFiles/spaceships.png"
#define NUM_SPACESHIPS 9
#define SHEET_SPACESHIPS_PER_ROW 5

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

typedef struct Context {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *screenSurface;
} Context;

typedef struct Sprite {
  SDL_Texture *texture;
  SDL_FRect src[NUM_SPACESHIPS];
  SDL_FRect dst[NUM_SPACESHIPS];
} Sprite;

static void ClipSpacehips(Sprite *spaceships) {
  for (size_t i = 0; i < NUM_SPACESHIPS; ++i) {
    int x, y, w, h = 0;
    if (i < SHEET_SPACESHIPS_PER_ROW) {
      x = i * 128;
      y = 0;
    } else {
      x = i - SHEET_SPACESHIPS_PER_ROW;
      y = 128;
    }

    w = 128;
    h = 128;

    spaceships->src[i].x = x;
    spaceships->src[i].y = y;
    spaceships->src[i].w = w;
    spaceships->src[i].h = h;
    spaceships->dst[i] = spaceships->src[i];
  }
}

static int gameLoop(Context *ctx) {
  int retVal = 0;

  Sprite spaceships = {
    .texture = IMG_LoadTexture(ctx->renderer, FILE_SHEET_SPACESHIPS),
    .src = {0},
    .dst = {0}
  };

  if (!spaceships.texture) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "SDL_RenderClear failed! SDL_Error: %s\n", SDL_GetError());
      retVal = -1;
      goto FAIL_SDL_LOAD_TEXTURE_SPACESHIPS;
  }
  
  ClipSpacehips(&spaceships);

  SDL_bool quit = SDL_FALSE;
  while (!quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT)
        quit = SDL_TRUE;
    }

    if (SDL_RenderClear(ctx->renderer)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "SDL_RenderClear failed! SDL_Error: %s\n", SDL_GetError());
      retVal = -1;
      goto FAIL_SDL_RENDER_CLEAR;
    }

    for (size_t i = 0; i < NUM_SPACESHIPS; ++i) {
      if (SDL_RenderTexture(ctx->renderer, spaceships.texture,
                            &spaceships.src[i], &spaceships.dst[i])) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL_RenderTexture failed! SDL_Error: %s\n",
                     SDL_GetError());
        retVal = -1;
        goto FAIL_SDL_RENDER_TEXTURE;
      }
    }

    if (SDL_RenderPresent(ctx->renderer)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "SDL_RenderPresent failed! SDL_Error: %s\n", SDL_GetError());
      retVal = -1;
      goto FAIL_SDL_RENDER_PRESENT;
    }
  }

FAIL_SDL_RENDER_PRESENT:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_SDL_RENDER_PRESENT");
  SDL_DestroyTexture(spaceships.texture);
FAIL_SDL_RENDER_TEXTURE:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_SDL_RENDER_TEXTURE");
FAIL_SDL_RENDER_CLEAR:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_SDL_RENDER_CLEAR");
FAIL_SDL_LOAD_TEXTURE_SPACESHIPS:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_SDL_LOAD_TEXTURE");

  return retVal;
}

int main(int argc, char *argv[]) {
  int retVal = 0;

  Context ctx = {.window = NULL, .screenSurface = NULL};

  ctx.window = NULL;
  ctx.screenSurface = NULL;

  if (!SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1")) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Could not set framebuffer acceleration hint\n");
  }

  if (SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    retVal = -1;
    goto FAIL_SDL_INIT;
  }

  ctx.window = SDL_CreateWindow("Galaga", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (!ctx.window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Window could not be created! SDL_Error: %s\n",
                 SDL_GetError());
    retVal = -1;
    goto FAIL_SDL_CREATE_WINDOW;
  }

  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Could not init IMG with PNG support! IMG_Error: %s\n",
                 IMG_GetError());
    retVal = -1;
    goto FAIL_IMG_INIT;
  }

  ctx.renderer = SDL_CreateRenderer(ctx.window, NULL, SDL_RENDERER_ACCELERATED);
  if (!ctx.renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Could not create renderer: SDL_Error: %s\n", SDL_GetError());
    retVal = -1;
    goto FAIL_SDL_CREATE_RENDERER;
  }
  SDL_SetRenderDrawColor(ctx.renderer, WHITE, WHITE, WHITE, OPAQUE);

  if (gameLoop(&ctx)) {
    retVal = -1;
    goto FAIL_GAME_LOOP;
  }

FAIL_GAME_LOOP:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_GAME_LOOP");
FAIL_SDL_CREATE_RENDERER:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_SDL_CREATE_RENDERER");
  SDL_DestroyRenderer(ctx.renderer);
  SDL_DestroyWindow(ctx.window);
FAIL_IMG_INIT:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_IMG_INIT");
FAIL_SDL_CREATE_WINDOW:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_SDL_CREATE_WINDOW");
  SDL_Quit();
FAIL_SDL_INIT:
  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "In FAIL_SDL_INIT");
  return retVal;
}
