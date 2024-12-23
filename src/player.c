#include "config.h"

#include <SDL3_image/SDL_image.h>
#include <assert.h>

#include "logger.h"
#include "player.h"
#include "texture.h"
#include "utils.h"

#define TEXTURE_ID "player"
#define TEXTURE_FILENAME                                                       \
  "assets/FREE Mana Seed Character Base "                                      \
  "Demo/char_a_p1/char_a_p1_0bas_humn_v01.png"

typedef struct {
  struct GameObject super;
} Player;

static void OnUpdate(GameObject *game_object, SDL_Renderer *renderer) {
  assert(game_object != NULL);
  Player *player = (Player *)game_object;

  const bool *keyboard_state = SDL_GetKeyboardState(NULL);
  assert(keyboard_state != NULL);

  int width, height;
  if (!SDL_GetRenderOutputSize(renderer, &width, &height)) {
    LOG_ERROR("Failed to get output size of rendering context");
    return;
  }

  if (keyboard_state[SDL_SCANCODE_W] && player->super.position.y > 0.0f) {
    player->super.position.y =
        MAX(player->super.position.y - player->super.velocity, 0.0f);
  }

  if (keyboard_state[SDL_SCANCODE_A] && player->super.position.x > 0.0f) {
    player->super.position.x =
        MAX(player->super.position.x - player->super.velocity, 0.0f);
  }

  if (keyboard_state[SDL_SCANCODE_S] &&
      player->super.position.y < (height - player->super.size.height)) {
    player->super.position.y =
        MIN(player->super.position.y + player->super.velocity, height);
  }

  if (keyboard_state[SDL_SCANCODE_D] &&
      player->super.position.x < (width - player->super.size.width)) {
    player->super.position.x =
        MIN(player->super.position.x + player->super.velocity, width);
  }
}

static void OnDraw(GameObject *game_object, TextureMap *texture_map,
                   SDL_Renderer *renderer) {
  assert(game_object != NULL);
  assert(renderer != NULL);

  Player *player = (Player *)game_object;

  if (!SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255)) {
    LOG_ERROR("Failed to set draw color: %s", SDL_GetError());
  }

  SDL_FRect rect = {
      .x = player->super.position.x,
      .y = player->super.position.y,
      .w = player->super.size.width,
      .h = player->super.size.height,
  };

  if (!SDL_RenderRect(renderer, &rect)) {
    LOG_ERROR("Failed to draw rectangle: %s", SDL_GetError());
  }

  TextureMapDrawFrame(texture_map, TEXTURE_ID, renderer,
                      player->super.position.x, player->super.position.y,
                      player->super.size.width, player->super.size.height, 0, 0,
                      0.0, 255, SDL_FLIP_NONE);
}

static void OnClean(GameObject *game_object, TextureMap *texture_map) {
  assert(game_object != NULL);
  assert(texture_map != NULL);

  Player *player = (Player *)game_object;

  if (player->super.texture_id != NULL) {
    LOG_DEBUG("Destroying texture");
    TextureMapClearTexture(texture_map, TEXTURE_ID);
  }

  free(player);
}

GameObject *PlayerCreate(TextureMap *texture_map, SDL_Renderer *renderer) {
  Player *player = xmalloc(sizeof(Player));

  player->super.position.x = 0.0f;
  player->super.position.y = 0.0f;

  player->super.size.width = 64.0f;
  player->super.size.height = 64.0f;

  player->super.velocity = 3.0f;

  player->super.callback.update = OnUpdate;
  player->super.callback.draw = OnDraw;
  player->super.callback.clean = OnClean;

  if (!TextureMapLoadTexture(texture_map, TEXTURE_FILENAME, TEXTURE_ID,
                             renderer)) {
    LOG_ERROR("Failed to load texture '%s'", TEXTURE_ID);
  }

  return (GameObject *)player;
}
