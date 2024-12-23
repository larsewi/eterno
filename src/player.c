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

typedef enum {
  PLAYER_STAND = 0,
  PLAYER_WALK,
  PLAYER_RUN,
} PlayerState;

typedef enum {
  PLAYER_DOWN = 0,
  PLAYER_UP,
  PLAYER_RIGHT,
  PLAYER_LEFT,
} PlayerDirection;

typedef struct {
  struct GameObject super;
  PlayerState state;
  PlayerDirection direction;
  bool jump;
  int animation_index;
  Uint32 frame_start;
  int column;
  int row;
} Player;

#define WALK_VELOCITY 3.0f
#define RUN_VELOCITY 6.0f

static void OnKeyDown(Player *player, const SDL_KeyboardEvent *event) {
  assert(player != NULL);
  assert(event != NULL);

  switch (event->scancode) {
  case SDL_SCANCODE_W:
    LOG_DEBUG("Player should look up");
    player->direction = PLAYER_UP;
    break;

  case SDL_SCANCODE_A:
    LOG_DEBUG("Player should look left");
    player->direction = PLAYER_LEFT;
    break;

  case SDL_SCANCODE_S:
    LOG_DEBUG("Player should look down");
    player->direction = PLAYER_DOWN;
    break;

  case SDL_SCANCODE_D:
    LOG_DEBUG("Player should look right");
    player->direction = PLAYER_RIGHT;
    break;

  case SDL_SCANCODE_SPACE:
    LOG_DEBUG("Player should jump");
    player->animation_index = 0;
    player->frame_start = SDL_GetTicks();
    player->jump = true;
    /* fallthrough */

  default:
    return;
  }

  player->animation_index = 0;
  player->frame_start = SDL_GetTicks();

  if (event->mod & SDL_KMOD_CTRL) {
    LOG_DEBUG("Player should stand");
    player->state = PLAYER_STAND;
    player->super.velocity = 0.0f;
  } else if (event->mod & SDL_KMOD_SHIFT) {
    LOG_DEBUG("Player should run");
    player->state = PLAYER_RUN;
    player->super.velocity = RUN_VELOCITY;
  } else {
    LOG_DEBUG("Player should walk");
    player->state = PLAYER_WALK;
    player->super.velocity = WALK_VELOCITY;
  }

  LOG_DEBUG("Player velocity %.2f", player->super.velocity);
}

static void OnKeyUp(Player *player, const SDL_KeyboardEvent *event) {
  assert(player != NULL);
  assert(event != NULL);

  switch (event->scancode) {
  case SDL_SCANCODE_W:
    if (player->direction == PLAYER_UP) {
      player->state = PLAYER_STAND;
    }
    break;

  case SDL_SCANCODE_A:
    if (player->direction == PLAYER_LEFT) {
      player->state = PLAYER_STAND;
    }
    break;

  case SDL_SCANCODE_S:
    if (player->direction == PLAYER_DOWN) {
      player->state = PLAYER_STAND;
    }
    break;

  case SDL_SCANCODE_D:
    if (player->direction == PLAYER_RIGHT) {
      player->state = PLAYER_STAND;
    }
    break;

  default:
    return;
  }

  LOG_DEBUG("Player should stand");
  player->animation_index = 0;
  player->super.velocity = 0.0f;
  LOG_DEBUG("Player velocity %.2f", player->super.velocity);
}

static void OnEvent(GameObject *game_object, const SDL_Event *event) {
  assert(game_object != NULL);
  assert(event != NULL);

  Player *player = (Player *)game_object;

  switch (event->type) {
  case SDL_EVENT_KEY_DOWN:
    OnKeyDown(player, &event->key);
    break;

  case SDL_EVENT_KEY_UP:
    OnKeyUp(player, &event->key);
    break;

  default:
    break;
  }
}

static void OnUpdate(GameObject *game_object, SDL_Renderer *renderer) {
  assert(game_object != NULL);
  Player *player = (Player *)game_object;

  int width, height;
  if (!SDL_GetRenderOutputSize(renderer, &width, &height)) {
    LOG_ERROR("Failed to get output size of rendering context");
    return;
  }

  switch (player->direction) {
  case PLAYER_DOWN:
    if (player->super.position.y < (height - player->super.size.height)) {
      player->super.position.y =
          MIN(player->super.position.y + player->super.velocity, height);
    }
    player->row = 0;
    break;

  case PLAYER_UP:
    if (player->super.position.y > 0.0f) {
      player->super.position.y =
          MAX(player->super.position.y - player->super.velocity, 0.0f);
    }
    player->row = 1;
    break;

  case PLAYER_RIGHT:
    if (player->super.position.x < (width - player->super.size.width)) {
      player->super.position.x =
          MIN(player->super.position.x + player->super.velocity, width);
    }
    player->row = 2;
    break;

  case PLAYER_LEFT:
    if (player->super.position.x > 0.0f) {
      player->super.position.x =
          MAX(player->super.position.x - player->super.velocity, 0.0f);
    }
    player->row = 3;
    break;
  }

  const Uint32 frame_time = SDL_GetTicks();
  int num_frames;
  const int *cycle;
  const Uint32 *timing;

  if (player->jump) {
    static const int jump_cycle[] = {5, 6, 7, 5};
    static const Uint32 jump_timing[] = {300, 150, 100, 300};

    num_frames = LENGTH(jump_cycle);
    assert(num_frames == LENGTH(jump_timing));

    cycle = jump_cycle;
    timing = jump_timing;
  } else {
    switch (player->state) {
    case PLAYER_STAND: {
      static const int stand_cycle[] = {0};
      static const Uint32 stand_timing[] = {0};

      num_frames = LENGTH(stand_cycle);
      assert(num_frames == LENGTH(stand_timing));

      cycle = stand_cycle;
      timing = stand_timing;
    } break;

    case PLAYER_WALK: {
      static const int walk_cycle[] = {0, 1, 2, 3, 4, 5};
      static const Uint32 walk_timing[] = {135, 135, 135, 135, 135, 135};

      num_frames = LENGTH(walk_cycle);
      assert(num_frames == LENGTH(walk_timing));

      cycle = walk_cycle;
      timing = walk_timing;

      player->row += 4;
    } break;

    case PLAYER_RUN: {
      static const int run_cycle[] = {0, 1, 6, 3, 4, 7};
      static const Uint32 run_timing[] = {80, 55, 125, 80, 55, 125};

      num_frames = LENGTH(run_cycle);
      assert(num_frames == LENGTH(run_timing));

      cycle = run_cycle;
      timing = run_timing;

      player->row += 4;
    } break;
    }
  }

  if ((num_frames > 1) &&
      (frame_time - player->frame_start) > timing[player->animation_index]) {
    LOG_DEBUG("Changing animation index from %d to %d", player->animation_index,
              (player->animation_index + 1) % num_frames);
    player->animation_index = (player->animation_index + 1) % num_frames;
    player->frame_start = frame_time;

    /* Stop the jump animation when sequence is done */
    if (player->jump && (player->animation_index == 0)) {
      LOG_DEBUG("Jump sequence done");
      player->jump = false;
    }
  }

  player->column = cycle[player->animation_index];
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
                      player->super.size.width, player->super.size.height,
                      player->column, player->row, 0.0, 255, SDL_FLIP_NONE);
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

  player->super.velocity = 0.0f;

  player->super.callback.event = OnEvent;
  player->super.callback.update = OnUpdate;
  player->super.callback.draw = OnDraw;
  player->super.callback.clean = OnClean;

  player->state = PLAYER_STAND;
  player->direction = PLAYER_DOWN;

  if (!TextureMapLoadTexture(texture_map, TEXTURE_FILENAME, TEXTURE_ID,
                             renderer)) {
    LOG_ERROR("Failed to load texture '%s'", TEXTURE_ID);
  }

  return (GameObject *)player;
}
