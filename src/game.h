#ifndef __ETERNO_GAME_H__
#define __ETERNO_GAME_H__

#include <stdbool.h>

typedef struct Game Game;

Game *GameInit(const char *title, int xpos, int ypos, int width, int height,
               bool fullscreen);

void GameDestroy(void *game);

#endif /* __ETERNO_GAME_H__ */
