#ifndef __ETERNO_GAME_OBJECT_H__
#define __ETERNO_GAME_OBJECT_H__

#include "SDL3/SDL.h"

typedef struct GameObject GameObject;

typedef void (*GameObjectLoadCallback)(GameObject *game_object);
typedef void (*GameObjectUpdateCallback)(GameObject *game_object);
typedef void (*GameObjectDrawCallback)(GameObject *game_object,
                                       SDL_Renderer *renderer);
typedef void (*GameObjectCleanCallback)(GameObject *game_object);

GameObject *GameObjectCreate(float xpos, float ypos, float width, float height,
                             const char *texture_id,
                             GameObjectLoadCallback load_cb,
                             GameObjectUpdateCallback update_cb,
                             GameObjectDrawCallback draw_cb,
                             GameObjectCleanCallback clean_cb);

void GameObjectLoad(GameObject *game_object);

void GameObjectUpdate(GameObject *game_object);

void GameObjectDraw(GameObject *game_object, SDL_Renderer *renderer);

void GameObjectDestroy(GameObject *game_object);

float GameObjectGetX(GameObject *game_object);

float GameObjectGetY(GameObject *game_object);

float GameObjectGetWidth(GameObject *game_object);

float GameObjectGetHeight(GameObject *game_object);

#endif /* __ETERNO_GAME_OBJECT_H__ */
