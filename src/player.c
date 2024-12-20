#include "player.h"
#include "config.h"
#include "logger.h"
#include "utils.h"

#include <SDL3_image/SDL_image.h>
#include <assert.h>

#define TEXTURE_FILENAME                                                       \
  "assets/FREE Mana Seed Character Base "                                      \
  "Demo/char_a_p1/char_a_p1_0bas_humn_v01.png"

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

  if (player->texture != NULL) {
    LOG_DEBUG("Destroying texture");
    SDL_DestroyTexture(player->texture);
    player->texture = NULL;
  }

  free(player);
}

GameObject *PlayerCreate(SDL_Renderer *renderer) {
  GameObject *player = xmalloc(sizeof(GameObject));

  player->position.x = 0.0f;
  player->position.y = 0.0f;

  player->size.width = 64.0f;
  player->size.height = 64.0f;

  player->velocity = 3.0f;

  player->callback.update = OnUpdate;
  player->callback.draw = OnDraw;
  player->callback.clean = OnClean;

  player->texture = NULL;

  LOG_DEBUG("Loading image from file '%s' into a surface", TEXTURE_FILENAME);
  SDL_Surface *surface = IMG_Load(TEXTURE_FILENAME);
  if (surface == NULL) {
    LOG_ERROR("Failed to load image '%s': %s", TEXTURE_FILENAME);
    return player;
  }

  LOG_DEBUG("Creating texture from surface");
  player->texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (player->texture == NULL) {
    LOG_ERROR("Failed to create texture from surface: %s", SDL_GetError());
  }

  LOG_DEBUG("Destroying surface");
  SDL_DestroySurface(surface);

  return player;
}
