
#ifndef __ETERNO_TEXTURE_H__
#define __ETERNO_TEXTURE_H__

#include "dict.h"

#include <SDL3/SDL.h>

typedef struct Dict TextureMap;

static inline TextureMap *TextureMapCreate(void) { return DictCreate(); }

static inline void TextureMapDestroy(void *texture_map) {
  DictDestroy(texture_map);
}

bool TextureMapLoadTexture(TextureMap *texture_map, const char *filename,
                           const char *texture_id, SDL_Renderer *renderer);

bool TextureMapClearTexture(TextureMap *texture_map, const char *texture_id);

bool TextureMapDrawFrame(const TextureMap *texture_map, const char *texture_id,
                         SDL_Renderer *renderer, float x, float y, float width,
                         float height, int column, int row, double angle,
                         Uint8 alpha, SDL_FlipMode flip);

bool TextureMapGetTextureSize(const TextureMap *texture_map,
                              const char *texture_id, float *width,
                              float *height);

#endif /* __ETERNO_TEXTURE_H__ */