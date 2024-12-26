#include "game.h"
#include "config.h"
#include "logger.h"
#include "player.h"
#include "texture.h"
#include "utils.h"

#include <SDL3/SDL.h>
#include <assert.h>
#include <stdio.h>

struct Game {
  bool running;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *render_target;
  TextureMap *texture_map;
  GameObject *player;
};

Game *GameInit(const char *title, int width, int height, bool fullscreen) {
  assert(title != NULL);

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

  LOG_DEBUG("Creating render target");
  game->render_target = SDL_CreateTexture(
      game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      RENDER_TARGET_WIDTH, RENDER_TARGET_HEIGHT);
  if (game->render_target == NULL) {
    LOG_ERROR("Failed to create render target: %s", SDL_GetError());
    GameDestroy(game);
    return NULL;
  }

  LOG_DEBUG("Creating texture map");
  game->texture_map = TextureMapCreate();
  assert(game->texture_map != NULL);

  LOG_DEBUG("Creating player");
  game->player = PlayerCreate(game->texture_map, game->renderer);

  game->running = true;

  return game;
}

bool GameIsRunning(Game *game) {
  assert(game != NULL);
  return game->running;
}

void GameHandleEvents(Game *game) {
  assert(game != NULL);

  bool window_resized = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      game->running = false;
      break;

    case SDL_EVENT_WINDOW_RESIZED:
      window_resized = true;
      break;

    default:
      break;
    }

    GameObjectEvent(game->player, &event);
  }

  if (window_resized) {
    int width, height;
    if (SDL_GetWindowSize(game->window, &width, &height)) {
      LOG_DEBUG("Window resized (w: %d, h: %d)", width, height);
    } else {
      LOG_ERROR("Failed to get window size: %s", SDL_GetError());
    }
  }
}

void GameUpdate(Game *game) {
  assert(game != NULL);

  GameObjectUpdate(game->player);
}

void GameRender(Game *game) {
  assert(game != NULL);

  /* Set render target to texture */
  if (!SDL_SetRenderTarget(game->renderer, game->render_target)) {
    LOG_ERROR("Failed to set render target to texture: %s", SDL_GetError());
    return;
  }

  /* Set render draw color to dark grey */
  if (!SDL_SetRenderDrawColor(game->renderer, 20, 20, 20, 255)) {
    LOG_ERROR("Failed to set draw color: %s", SDL_GetError());
    return;
  }

  /* Clear texture render target */
  if (!SDL_RenderClear(game->renderer)) {
    LOG_ERROR("Failed to clear the current rendering target: %s",
              SDL_GetError());
    return;
  }

  /* Draw to render target */
  GameObjectDraw(game->player, game->texture_map, game->renderer);

  /* Set render target back to screen */
  if (!SDL_SetRenderTarget(game->renderer, NULL)) {
    LOG_ERROR("Failed to set render target to screen: %s", SDL_GetError());
    return;
  }

  /* Set render draw color to black */
  if (!SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255)) {
    LOG_ERROR("Failed to set draw color: %s", SDL_GetError());
    return;
  }

  /* Clear screen render target */
  if (!SDL_RenderClear(game->renderer)) {
    LOG_ERROR("Failed to clear the current rendering target: %s",
              SDL_GetError());
    return;
  }

  /* Get window size */
  int window_width, window_height;
  if (!SDL_GetRenderOutputSize(game->renderer, &window_width, &window_height)) {
    LOG_ERROR("Failed to get window size: %s", SDL_GetError());
    return;
  }

  /* Compute scaling factor */
  const float scale_x = window_width / RENDER_TARGET_WIDTH;
  const float scale_y = window_height / RENDER_TARGET_HEIGHT;
  const float scale = (scale_x < scale_y) ? scale_x : scale_y;

  /* Compute centered view port */
  const float scaled_width = RENDER_TARGET_WIDTH * scale;
  const float scaled_height = RENDER_TARGET_HEIGHT * scale;
  const float offset_x = (window_width - scaled_width) / 2;
  const float offset_y = (window_height - scaled_height) / 2;

  const SDL_FRect dst_rect = {
      .x = offset_x,
      .y = offset_y,
      .w = scaled_width,
      .h = scaled_height,
  };

  /* Render texture to screen */
  if (!SDL_RenderTexture(game->renderer, game->render_target, NULL,
                         &dst_rect)) {
    LOG_ERROR("Failed to render texture to screen: %s", SDL_GetError());
    return;
  }

  /* Present final image */
  if (!SDL_RenderPresent(game->renderer)) {
    LOG_ERROR("Failed update screen with rendering: %s", SDL_GetError());
    return;
  }
}

void GameDestroy(Game *game) {
  if (game == NULL) {
    return;
  }

  LOG_DEBUG("Destroying player");
  GameObjectDestroy(game->player, game->texture_map);

  LOG_DEBUG("Destroying texture map");
  TextureMapDestroy(game->texture_map);

  LOG_DEBUG("Destroying render target");
  SDL_DestroyTexture(game->render_target);

  LOG_DEBUG("Destroying renderer");
  SDL_DestroyRenderer(game->renderer);

  LOG_DEBUG("Destroying window");
  SDL_DestroyWindow(game->window);

  LOG_DEBUG("Shutting down subsystems");
  SDL_Quit();
}
