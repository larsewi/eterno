#ifndef __ETERNO_GAME_OBJECT_H__
#define __ETERNO_GAME_OBJECT_H__

#include <assert.h>

#include "SDL3/SDL.h"

typedef struct GameObject GameObject;

typedef void (*GameObjectCallbackUpdate)(GameObject *game_object,
                                         SDL_Renderer *renderer);
typedef void (*GameObjectCallbackDraw)(GameObject *game_object,
                                       SDL_Renderer *renderer);
typedef void (*GameObjectCallbackClean)(GameObject *game_object);

struct GameObject {
  struct {
    float x;
    float y;
  } position;
  struct {
    float width;
    float height;
  } size;
  float velocity;
  struct {
    GameObjectCallbackUpdate update;
    GameObjectCallbackDraw draw;
    GameObjectCallbackClean clean;
  } callback;
  SDL_Texture *texture;
};

static inline void GameObjectUpdate(GameObject *game_object,
                                    SDL_Renderer *renderer) {
  assert(game_object != NULL);
  game_object->callback.update(game_object, renderer);
}

static inline void GameObjectDraw(GameObject *game_object,
                                  SDL_Renderer *renderer) {
  assert(game_object != NULL);
  game_object->callback.draw(game_object, renderer);
}

static inline void GameObjectDestroy(GameObject *game_object) {
  assert(game_object != NULL);
  game_object->callback.clean(game_object);
}

#endif /* __ETERNO_GAME_OBJECT_H__ */
