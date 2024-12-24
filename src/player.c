#include "config.h"

#include <SDL3_image/SDL_image.h>
#include <assert.h>

#include "logger.h"
#include "player.h"
#include "texture.h"
#include "utils.h"
#include "vector.h"

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

#define WALK_VELOCITY 1.0f
#define RUN_VELOCITY 2.0f

static void OnKeyDown(Player *player, const SDL_KeyboardEvent *event) {
  assert(player != NULL);
  assert(event != NULL);

  bool reset_animation = false;
  switch (event->scancode) {
  case SDL_SCANCODE_W:
    player->direction = PLAYER_UP;
    break;

  case SDL_SCANCODE_A:
    player->direction = PLAYER_LEFT;
    break;

  case SDL_SCANCODE_S:
    player->direction = PLAYER_DOWN;
    break;

  case SDL_SCANCODE_D:
    player->direction = PLAYER_RIGHT;
    break;

  case SDL_SCANCODE_SPACE:
    if (!player->jump) {
      LOG_DEBUG("Player should jump");
      player->animation_index = 0;
      player->frame_start = SDL_GetTicks();
      player->jump = true;
    }
    return;

  default:
    return;
  }

  if (!player->jump && reset_animation) {
    player->animation_index = 0;
    player->frame_start = SDL_GetTicks();
  }
}

static void OnEvent(GameObject *game_object, const SDL_Event *event) {
  assert(game_object != NULL);
  assert(event != NULL);

  Player *player = (Player *)game_object;

  switch (event->type) {
  case SDL_EVENT_KEY_DOWN:
    OnKeyDown(player, &event->key);
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

  const bool *keyboard_state = SDL_GetKeyboardState(NULL);
  assert(keyboard_state != NULL);

  Vector vec = {.x = 0.0f, .y = 0.0f};
  if (keyboard_state[SDL_SCANCODE_W]) {
    vec.y -= 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_A]) {
    vec.x -= 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_S]) {
    vec.y += 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_D]) {
    vec.x += 1.0f;
  }
  VectorNorm(&vec);

  if (keyboard_state[SDL_SCANCODE_LCTRL]) {
    VectorMul(&vec, 0.0f);
  } else if (keyboard_state[SDL_SCANCODE_LSHIFT]) {
    VectorMul(&vec, RUN_VELOCITY);
    if (!player->jump && player->state != PLAYER_RUN) {
      player->animation_index = 0;
    }
    player->state = PLAYER_RUN;
  } else {
    VectorMul(&vec, WALK_VELOCITY);
    if (!player->jump && player->state != PLAYER_WALK) {
      player->animation_index = 0;
    }
    player->state = PLAYER_WALK;
  }

  if (VectorIsZero(&vec)) {
    if (!player->jump && player->state != PLAYER_STAND) {
      player->animation_index = 0;
    }
    player->state = PLAYER_STAND;
  } else {
    VectorAdd(&player->super.position, &vec);
    Vector max = {.x = width, .y = height};
    VectorCap(&player->super.position, VectorZero(),
              VectorSub(&max, &player->super.size));
  }

  player->row = player->direction;

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

  player->super.callback.event = OnEvent;
  player->super.callback.update = OnUpdate;
  player->super.callback.draw = OnDraw;
  player->super.callback.clean = OnClean;

  player->state = PLAYER_STAND;
  player->direction = PLAYER_DOWN;
  player->jump = false;

  if (!TextureMapLoadTexture(texture_map, TEXTURE_FILENAME, TEXTURE_ID,
                             renderer)) {
    LOG_ERROR("Failed to load texture '%s'", TEXTURE_ID);
  }

  return (GameObject *)player;
}
