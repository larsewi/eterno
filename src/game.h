#ifndef __ETERNO_GAME_H__
#define __ETERNO_GAME_H__

#include <stdbool.h>

typedef struct Game Game;

Game *GameInit(const char *title, int width, int height, bool fullscreen);

bool GameIsRunning(Game *game);

bool GameHandleEvents(Game *game);

bool GameUpdate(Game *game);

bool GameRender(Game *game);

void GameDestroy(Game *game);

#endif /* __ETERNO_GAME_H__ */
