#include "player.h"
#include "config.h"
#include "logger.h"
#include "utils.h"

#include <assert.h>

static void OnLoad(GameObject *player) { assert(player != NULL); }

static void OnUpdate(GameObject *player) { assert(player != NULL); }

static void OnDraw(GameObject *player, SDL_Renderer *renderer) {
  assert(player != NULL);
  assert(renderer != NULL);

  if (!SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255)) {
    LOG_WARNING("Failed to set draw color: %s", SDL_GetError());
  }

  SDL_FRect rect = {
      .x = GameObjectGetX(player),
      .y = GameObjectGetY(player),
      .w = GameObjectGetWidth(player),
      .h = GameObjectGetHeight(player),
  };

  if (!SDL_RenderRect(renderer, &rect)) {
    LOG_WARNING("Failed to draw rectangle: %s", SDL_GetError());
  }
}

static void OnClean(GameObject *player) { assert(player != NULL); }

GameObject *PlayerCreate(void) {
  GameObject *player = GameObjectCreate(0, 0, 64, 64, "Player", OnLoad,
                                        OnUpdate, OnDraw, OnClean);
  return player;
}
