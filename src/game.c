#include "game.h"
#include "config.h"
#include "input_handler.h"
#include "logger.h"
#include "player.h"
#include "utils.h"

#include <SDL3/SDL.h>
#include <assert.h>
#include <stdio.h>

struct Game {
  bool running;
  SDL_Window *window;
  SDL_Renderer *renderer;
  InputHandler *input_handler;
  GameObject *player;
};

Game *GameInit(const char *title, int xpos, int ypos, int width, int height,
               bool fullscreen) {
  Game *ret = NULL;

  Game *game = xmalloc(sizeof(Game));
  memset(game, 0, sizeof(Game));

  LOG_DEBUG("Initializing subsystems");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    LOG_ERROR("Failed to initialize subsystems: %s", SDL_GetError());
    GameDestroy(game);
    return NULL;
  }

  LOG_DEBUG("Creating window");
  int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
  flags |= SDL_WINDOW_RESIZABLE;
  game->window = SDL_CreateWindow(title, width, height, flags);
  if (game->window == NULL) {
    LOG_ERROR("Failed to create window: %s", SDL_GetError());
    GameDestroy(game);
    return NULL;
  }

  LOG_DEBUG("Creating renderer");
  game->renderer = SDL_CreateRenderer(game->window, NULL);
  if (game->renderer == NULL) {
    LOG_ERROR("Failed to create renderer: %s", SDL_GetError());
    GameDestroy(game);
    return NULL;
  }

  LOG_DEBUG("Initializing input handler");
  game->input_handler = InputHandlerInit();
  if (game->input_handler == NULL) {
    LOG_ERROR("Failed to initialize input handler");
    GameDestroy(game);
    return NULL;
  }

  LOG_DEBUG("Creating player");
  game->player = PlayerCreate();

  game->running = true;

  return game;
}

bool GameIsRunning(Game *game) {
  assert(game != NULL);
  return game->running;
}

void GameHandleEvents(Game *game) {
  assert(game != NULL);
  InputHandlerHandleEvents(game->input_handler);
}

void GameUpdate(Game *game) {
  assert(game != NULL);
  if (InputHandlerGameShouldQuit(game->input_handler)) {
    game->running = false;
  }

  GameObjectUpdate(game->player);
}

void GameRender(Game *game) {
  assert(game != NULL);

  if (!SDL_SetRenderDrawColor(game->renderer, 0x0, 0x0, 0x0, 0xFF)) {
    LOG_WARNING("Failed to set draw color: %s", SDL_GetError());
  }

  if (!SDL_RenderClear(game->renderer)) {
    LOG_WARNING("Failed to clear the current rendering target: %s",
                SDL_GetError());
  }

  GameObjectDraw(game->player, game->renderer);

  if (!SDL_RenderPresent(game->renderer)) {
    LOG_WARNING("Failed update screen with rendering: %s", SDL_GetError());
  }
}

void GameDestroy(Game *game) {
  if (game == NULL) {
    return;
  }

  GameObjectDestroy(game->player);

  LOG_DEBUG("Destroying input handler");
  InputHandlerDestroy(game->input_handler);

  LOG_DEBUG("Destroying renderer");
  SDL_DestroyRenderer(game->renderer);

  LOG_DEBUG("Destroying window");
  SDL_DestroyWindow(game->window);

  LOG_DEBUG("Shutting down subsystems");
  SDL_Quit();
}
