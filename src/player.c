#include "player.h"
#include "config.h"
#include "logger.h"
#include "utils.h"

#include <assert.h>

static void OnLoad(GameObject *player) { assert(player != NULL); }

static void OnUpdate(GameObject *player, SDL_Renderer *renderer) {
  assert(player != NULL);

  const bool *keyboard_state = SDL_GetKeyboardState(NULL);
  assert(keyboard_state != NULL);

  int width, height;
  if (!SDL_GetRenderOutputSize(renderer, &width, &height)) {
    LOG_ERROR("Failed to get output size of rendering context");
    return;
  }

  if (keyboard_state[SDL_SCANCODE_W] && player->position.y > 0.0f) {
    player->position.y = MAX(player->position.y - player->velocity, 0.0f);
  }

  if (keyboard_state[SDL_SCANCODE_A] && player->position.x > 0.0f) {
    player->position.x = MAX(player->position.x - player->velocity, 0.0f);
  }

  if (keyboard_state[SDL_SCANCODE_S] &&
      player->position.y < (height - player->size.height)) {
    player->position.y = MIN(player->position.y + player->velocity, height);
  }

  if (keyboard_state[SDL_SCANCODE_D] &&
      player->position.x < (width - player->size.width)) {
    player->position.x = MIN(player->position.x + player->velocity, width);
  }
}

static void OnDraw(GameObject *player, SDL_Renderer *renderer) {
  assert(player != NULL);
  assert(renderer != NULL);

  if (!SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255)) {
    LOG_ERROR("Failed to set draw color: %s", SDL_GetError());
  }

  SDL_FRect rect = {
      .x = player->position.x,
      .y = player->position.y,
      .w = player->size.width,
      .h = player->size.height,
  };

  if (!SDL_RenderRect(renderer, &rect)) {
    LOG_ERROR("Failed to draw rectangle: %s", SDL_GetError());
  }
}

static void OnClean(GameObject *player) {
  assert(player != NULL);
  free(player);
}

GameObject *PlayerCreate(void) {
  GameObject *player = xmalloc(sizeof(GameObject));

  player->position.x = 0.0f;
  player->position.y = 0.0f;

  player->size.width = 64.0f;
  player->size.height = 64.0f;

  player->velocity = 3.0f;

  player->callback.load = OnLoad;
  player->callback.update = OnUpdate;
  player->callback.draw = OnDraw;
  player->callback.clean = OnClean;

  return player;
}
