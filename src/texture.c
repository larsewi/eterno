#include "config.h"

#include <SDL3_image/SDL_image.h>
#include <assert.h>

#include "dict.h"
#include "logger.h"
#include "texture.h"
#include "utils.h"

typedef struct {
  SDL_Texture *texture;
  unsigned ref_counter;
} TextureMapEntry;

static void TextureMapEntryDestroy(void *ptr) {
  TextureMapEntry *map_entry = ptr;
  assert(map_entry != NULL);

  if (map_entry->ref_counter != 0) {
    LOG_ERROR("Refusing to destroy texture map entry: "
              "Reference counter %d != 0",
              map_entry->ref_counter);
    return;
  }

  SDL_DestroyTexture(map_entry->texture);
  free(map_entry);
}

bool TextureMapLoadTexture(TextureMap *texture_map, const char *filename,
                           const char *texture_id, SDL_Renderer *renderer) {
  assert(texture_map != NULL);
  assert(filename != NULL);
  assert(texture_id != NULL);
  assert(renderer != NULL);

  if (DictHasKey(texture_map, texture_id)) {
    TextureMapEntry *map_entry =
        (TextureMapEntry *)DictGet(texture_map, texture_id);
    LOG_DEBUG("Incrementing reference counter for texture '%s' from %d to %d",
              texture_id, map_entry->ref_counter, map_entry->ref_counter + 1);
    map_entry->ref_counter += 1;
    return true;
  }

  TextureMapEntry *map_entry = xmalloc(sizeof(TextureMapEntry));
  memset(map_entry, 0, sizeof(TextureMapEntry));

  LOG_DEBUG("Loading surface from file '%s'", filename);
  SDL_Surface *surface = IMG_Load(filename);
  if (surface == NULL) {
    LOG_ERROR("Failed to load image from '%s'", filename);
    TextureMapEntryDestroy(map_entry);
    return false;
  }

  LOG_DEBUG("Creating texture from surface");
  map_entry->texture = SDL_CreateTextureFromSurface(renderer, surface);

  LOG_DEBUG("Destroying surface");
  SDL_DestroySurface(surface);

  if (map_entry->texture == NULL) {
    LOG_ERROR("Failed to create texture: %s", SDL_GetError());
    TextureMapEntryDestroy(map_entry);
    return false;
  }

  DictSet(texture_map, texture_id, map_entry, TextureMapEntryDestroy);

  LOG_DEBUG("Incrementing reference counter for texture '%s' from %d to %d",
            texture_id, map_entry->ref_counter, map_entry->ref_counter + 1);
  map_entry->ref_counter += 1;
  return true;
}

bool TextureMapClearTexture(TextureMap *texture_map, const char *texture_id) {
  assert(texture_map != NULL);
  assert(texture_id != NULL);

  if (!DictHasKey(texture_map, texture_id)) {
    LOG_ERROR("Attempted to clear non-existent texture map entry with id '%s'",
              texture_id);
    return false;
  }

  TextureMapEntry *map_entry =
      (TextureMapEntry *)DictGet(texture_map, texture_id);
  if (map_entry->ref_counter > 0) {
    LOG_DEBUG("Decrementing reference counter for texture '%s' from %d to %d",
              texture_id, map_entry->ref_counter, map_entry->ref_counter - 1);
    map_entry->ref_counter -= 1;
  } else {
    LOG_DEBUG("Destroying texture '%d': Reference counter %s", texture_id,
              map_entry->ref_counter);
    DictRemove(texture_map, texture_id);
  }

  return true;
}

bool TextureMapDrawFrame(const TextureMap *texture_map, const char *texture_id,
                         SDL_Renderer *renderer, float x, float y, float width,
                         float height, int column, int row, double angle,
                         Uint8 alpha, SDL_FlipMode flip) {
  assert(texture_map != NULL);
  assert(texture_id != NULL);

  if (!DictHasKey(texture_map, texture_id)) {
    LOG_ERROR("Failed to draw frame: Texture '%s' does not exist", texture_id);
    return false;
  }

  const TextureMapEntry *map_entry = DictGet(texture_map, texture_id);
  SDL_Texture *texture = map_entry->texture;

  SDL_FRect src_rect = {
      .x = width * column,
      .y = height * row,
      .w = width,
      .h = height,
  };

  SDL_FRect dst_rect = {
      .x = x,
      .y = y,
      .w = width,
      .h = height,
  };

  bool success = true;

  if (!SDL_SetTextureAlphaMod(texture, alpha)) {
    LOG_ERROR("Failed to set texture alpha modulation: %s", SDL_GetError());
    success = false;
  }

  if (!SDL_RenderTextureRotated(renderer, texture, &src_rect, &dst_rect, angle,
                                NULL, flip)) {
    LOG_ERROR("Failed to render texture '%s': %s", texture_id, SDL_GetError());
    success = false;
  }

  return success;
}
