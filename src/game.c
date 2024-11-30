#include "game.h"

#include <SDL3/SDL.h>
#include <stdio.h>

#include "logger.h"
#include "utils.h"

struct Game {
  bool running;
  SDL_Window *window;
  SDL_Renderer *renderer;
};

Game *GameInit(const char *title, int xpos, int ypos, int width, int height,
               bool fullscreen) {
  Game *ret = NULL;

  Game *game = xmalloc(sizeof(Game));
  memset(game, 0, sizeof(Game));

  LOG_DEBUG("Initializing subsystems...");
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_ERROR("Failed to initialize subsystems: %s", SDL_GetError());
    GameDestroy(game);
    return NULL;
  }

  LOG_DEBUG("Creating window...");
  int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
  game->window = SDL_CreateWindow(title, width, height, flags);
  if (game->window == NULL) {
    LOG_ERROR("Failed to create window: %s", SDL_GetError());
    GameDestroy(game);
    return NULL;
  }

  LOG_DEBUG("Creating renderer...");
  game->renderer = SDL_CreateRenderer(game->window, NULL);
  if (game->renderer == NULL) {
    LOG_ERROR("Failed to create renderer: %s", SDL_GetError());
    GameDestroy(game);
    return NULL;
  }

  if (!SDL_SetRenderDrawColor(game->renderer, 0x0, 0x0, 0x0, 0xFF)) {
    LOG_WARNING("Failed to draw black screen");
  }

  return game;
}

void GameDestroy(void *ptr) {
  Game *game = (Game *)ptr;
  if (game == NULL) {
    return;
  }

  LOG_DEBUG("Destroying renderer...");
  SDL_DestroyRenderer(game->renderer);

  LOG_DEBUG("Destroying window...");
  SDL_DestroyWindow(game->window);

  LOG_DEBUG("Shutting down subsystems...");
  SDL_Quit();
}
