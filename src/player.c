#include "config.h"

#include <SDL3_image/SDL_image.h>
#include <assert.h>

#include "logger.h"
#include "player.h"
#include "texture.h"
#include "utils.h"
#include "vector.h"

#define FRAME_DURATION 100 /* ms */

static const char *const texture_ids[] = {
    "player/idle", "player/walk",   "player/run", "player/jump",
    "player/fall", "player/attack", "player/die",
};

typedef enum {
  PLAYER_IDLE = 0,
  PLAYER_WALK,
  PLAYER_RUN,
  PLAYER_JUMP,
  PLAYER_FALL,
  PLAYER_ATTACK,
  PLAYER_DIE,
} PlayerState;

typedef struct {
  struct GameObject super;
  PlayerState state;
  Uint32 jump_start;
  Uint32 frame_start;
  unsigned frame_index;
  SDL_FlipMode flip;
} Player;

#define WALK_VELOCITY 1.5f
#define RUN_VELOCITY 3.0f
#define JUMP_VELOCITY 3.0f
#define GRAVITY 0.00028f

static void OnEvent(GameObject *game_object, const SDL_Event *event) {
  assert(game_object != NULL);
  assert(event != NULL);

  Player *player = (Player *)game_object;
  (void)player;
  return;
}

static void OnUpdate(GameObject *game_object) {
  assert(game_object != NULL);
  Player *player = (Player *)game_object;

  const Uint32 frame_time = SDL_GetTicks();
  const bool *keyboard_state = SDL_GetKeyboardState(NULL);

  /* Move player up and down */
  if (player->super.position.y >=
      (RENDER_TARGET_HEIGHT - player->super.size.height)) {
    /* Player is colliding with floor */
    player->super.position.y =
        (RENDER_TARGET_HEIGHT - player->super.size.height);
    player->super.velocity.y = 0.0f;

    if (keyboard_state[SDL_SCANCODE_SPACE]) {
      /* Player wants to jump */
      player->super.velocity.y -= JUMP_VELOCITY;
      player->jump_start = frame_time;
    }
  } else {
    /* Player is in the air */
    player->super.velocity.y += GRAVITY * (frame_time - player->jump_start);
  }

  /* Move player left and right */
  bool is_running = keyboard_state[SDL_SCANCODE_LSHIFT];
  player->super.velocity.x = 0.0f;
  if (player->super.position.x <= 0.0f) {
    /* Player is colliding with left wall */
    player->super.position.x = 0.0f;
  } else {
    if (keyboard_state[SDL_SCANCODE_A]) {
      /* Player wants to walk to the left */
      player->super.velocity.x -= (is_running) ? RUN_VELOCITY : WALK_VELOCITY;
    }
  }
  if (player->super.position.x >=
      (RENDER_TARGET_WIDTH - player->super.size.width)) {
    /* Player is colliding with right wall */
    player->super.position.x = (RENDER_TARGET_WIDTH - player->super.size.width);
  } else {
    if (keyboard_state[SDL_SCANCODE_D]) {
      /* Player wants to walk to the right */
      player->super.velocity.x += (is_running) ? RUN_VELOCITY : WALK_VELOCITY;
    }
  }

  /* Update sprite sheet */
  if (player->super.velocity.y < 0.0f) {
    if (player->state != PLAYER_JUMP) {
      player->state = PLAYER_JUMP;
      player->frame_start = frame_time;
      player->frame_index = 0;
    }
  } else if (player->super.velocity.y > 0.0f) {
    if (player->state != PLAYER_FALL) {
      player->state = PLAYER_FALL;
      player->frame_start = frame_time;
      player->frame_index = 0;
    }
  } else if (player->super.velocity.x != 0.0f) {
    if (is_running) {
      if (player->state != PLAYER_RUN) {
        player->state = PLAYER_RUN;
        player->frame_start = frame_time;
        player->frame_index = 0;
      }
    } else {
      if (player->state != PLAYER_WALK) {
        player->state = PLAYER_WALK;
        player->frame_start = frame_time;
        player->frame_index = 0;
      }
    }
  } else {
    if (player->state != PLAYER_IDLE) {
      player->state = PLAYER_IDLE;
      player->frame_start = frame_time;
      player->frame_index = 0;
    }
  }

  /* Flip texture based on direction */
  if (player->super.velocity.x < 0.0f) {
    player->flip = SDL_FLIP_NONE;
  } else if (player->super.velocity.x > 0.0f) {
    player->flip = SDL_FLIP_HORIZONTAL;
  }

  /* Update fame index */
  if ((frame_time - player->frame_start) >= FRAME_DURATION) {
    player->frame_index += 1;
    player->frame_start = frame_time;
  }

  /* Update player position */
  VectorAdd(&player->super.position, &player->super.velocity);
}

static void OnDraw(GameObject *game_object, TextureMap *texture_map,
                   SDL_Renderer *renderer) {
  assert(game_object != NULL);
  assert(renderer != NULL);

  Player *player = (Player *)game_object;
  const char *texture_id = texture_ids[player->state];

  float texture_width;
  if (!TextureMapGetTextureSize(texture_map, texture_id, &texture_width,
                                NULL)) {
    LOG_CRITICAL("Failed to get size of texture '%s'", texture_id);
  }

  int num_frames = (int)(texture_width / player->super.size.width);
  int column = player->frame_index % num_frames;

  TextureMapDrawFrame(texture_map, texture_id, renderer,
                      player->super.position.x, player->super.position.y,
                      player->super.size.width, player->super.size.height,
                      column, 0, 0.0, 255, player->flip);
}

static void OnClean(GameObject *game_object, TextureMap *texture_map) {
  assert(game_object != NULL);
  assert(texture_map != NULL);

  Player *player = (Player *)game_object;

  for (size_t i = 0; i < LENGTH(texture_ids); i++) {
    const char *id = texture_ids[i];
    LOG_DEBUG("Destroying texture '%s'", id);
    TextureMapClearTexture(texture_map, id);
  }

  free(player);
}

GameObject *PlayerCreate(TextureMap *texture_map, SDL_Renderer *renderer) {
  int width, height;
  if (!SDL_GetRenderOutputSize(renderer, &width, &height)) {
    LOG_CRITICAL("Failed to get render output size");
  }

  Player *player = xmalloc(sizeof(Player));

  player->super.size.width = 80.0f;
  player->super.size.height = 64.0f;

  /* Start by falling from the centre of the screen */
  player->super.position.x = (width / 2) - (player->super.size.width / 2);
  player->super.position.y = (height / 2) - (player->super.size.height / 2);

  player->super.velocity.x = 0.0f;
  player->super.velocity.y = 0.0f;

  player->super.callback.event = OnEvent;
  player->super.callback.update = OnUpdate;
  player->super.callback.draw = OnDraw;
  player->super.callback.clean = OnClean;

  player->state = PLAYER_FALL;
  player->jump_start = player->frame_start = SDL_GetTicks();
  player->frame_index = 0;
  player->flip = SDL_FLIP_NONE;

  for (size_t i = 0; i < LENGTH(texture_ids); i++) {
    const char *id = texture_ids[i];

    char file[PATH_MAX];
    int ret = snprintf(file, sizeof(file), "assets/%s.png", id);
    if (ret < 0 || (size_t)ret >= sizeof(file)) {
      LOG_CRITICAL("Failed to load texture '%s': Path too long (%d >= %zu)", id,
                   ret, sizeof(file));
    }

    if (!TextureMapLoadTexture(texture_map, file, id, renderer)) {
      LOG_CRITICAL("Failed to load texture '%s' from file '%s'", id, file);
    }
  }

  return (GameObject *)player;
}
