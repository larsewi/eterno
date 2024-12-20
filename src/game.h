#ifndef __ETERNO_GAME_H__
#define __ETERNO_GAME_H__

#include <stdbool.h>

typedef struct Game Game;

Game *GameInit(const char *title, int width, int height, bool fullscreen);

bool GameIsRunning(Game *game);

void GameHandleEvents(Game *game);

void GameUpdate(Game *game);

void GameRender(Game *game);

void GameDestroy(Game *game);

#endif /* __ETERNO_GAME_H__ */
