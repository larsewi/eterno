#include "game_object.h"
#include "config.h"
#include "utils.h"

#include <assert.h>

struct GameObject {
  float xpos;
  float ypos;
  float width;
  float height;
  const char *texture_id;
  GameObjectLoadCallback load_cb;
  GameObjectUpdateCallback update_cb;
  GameObjectDrawCallback draw_cb;
  GameObjectCleanCallback clean_cb;
};

GameObject *GameObjectCreate(float xpos, float ypos, float width, float height,
                             const char *texture_id,
                             GameObjectLoadCallback load_cb,
                             GameObjectUpdateCallback update_cb,
                             GameObjectDrawCallback draw_cb,
                             GameObjectCleanCallback clean_cb) {
  GameObject *game_object = xmalloc(sizeof(GameObject));
  game_object->xpos = xpos;
  game_object->ypos = ypos;
  game_object->width = width;
  game_object->height = height;
  game_object->texture_id = texture_id;
  game_object->load_cb = load_cb;
  game_object->update_cb = update_cb;
  game_object->draw_cb = draw_cb;
  game_object->clean_cb = clean_cb;
  return game_object;
}

void GameObjectLoad(GameObject *game_object) {
  assert(game_object != NULL);
  assert(game_object->load_cb != NULL);
  game_object->load_cb(game_object);
}

void GameObjectUpdate(GameObject *game_object) {
  assert(game_object != NULL);
  assert(game_object->update_cb != NULL);
  game_object->update_cb(game_object);
}

void GameObjectDraw(GameObject *game_object, SDL_Renderer *renderer) {
  assert(game_object != NULL);
  assert(game_object->draw_cb != NULL);
  game_object->draw_cb(game_object, renderer);
}

void GameObjectDestroy(GameObject *game_object) {
  assert(game_object != NULL);
  assert(game_object->clean_cb != NULL);
  game_object->clean_cb(game_object);
  free(game_object);
}

float GameObjectGetX(GameObject *game_object) {
  assert(game_object != NULL);
  return game_object->xpos;
}

float GameObjectGetY(GameObject *game_object) {
  assert(game_object != NULL);
  return game_object->ypos;
}

float GameObjectGetWidth(GameObject *game_object) {
  assert(game_object != NULL);
  return game_object->width;
}

float GameObjectGetHeight(GameObject *game_object) {
  assert(game_object != NULL);
  return game_object->height;
}
