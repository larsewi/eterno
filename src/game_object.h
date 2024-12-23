#ifndef __ETERNO_GAME_OBJECT_H__
#define __ETERNO_GAME_OBJECT_H__

#include <assert.h>

#include "SDL3/SDL.h"

#include "texture.h"

typedef struct GameObject GameObject;

typedef void (*GameObjectCallbackEvent)(GameObject *game_object,
                                        const SDL_Event *event);
typedef void (*GameObjectCallbackUpdate)(GameObject *game_object,
                                         SDL_Renderer *renderer);
typedef void (*GameObjectCallbackDraw)(GameObject *game_object,
                                       TextureMap *texture_map,
                                       SDL_Renderer *renderer);
typedef void (*GameObjectCallbackClean)(GameObject *game_object,
                                        TextureMap *texture_map);

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
    GameObjectCallbackEvent event;
    GameObjectCallbackUpdate update;
    GameObjectCallbackDraw draw;
    GameObjectCallbackClean clean;
  } callback;
  const char *texture_id;
};

static inline void GameObjectEvent(GameObject *game_object,
                                   const SDL_Event *event) {
  assert(game_object != NULL);
  game_object->callback.event(game_object, event);
}

static inline void GameObjectUpdate(GameObject *game_object,
                                    SDL_Renderer *renderer) {
  assert(game_object != NULL);
  game_object->callback.update(game_object, renderer);
}

static inline void GameObjectDraw(GameObject *game_object,
                                  TextureMap *texture_map,
                                  SDL_Renderer *renderer) {
  assert(game_object != NULL);
  game_object->callback.draw(game_object, texture_map, renderer);
}

static inline void GameObjectDestroy(GameObject *game_object,
                                     TextureMap *texture_map) {
  assert(game_object != NULL);
  game_object->callback.clean(game_object, texture_map);
}

#endif /* __ETERNO_GAME_OBJECT_H__ */
