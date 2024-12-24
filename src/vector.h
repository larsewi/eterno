#ifndef __ETERNO_VECTOR_H__
#define __ETERNO_VECTOR_H__

#include <SDL3/SDL.h>

#include "utils.h"

typedef struct {
  union {
    float x;
    float width;
  };
  union {
    float y;
    float height;
  };
} Vector;

static inline const Vector *VectorZero(void) {
  static const Vector vec = {.x = 0.0f, .y = 0.0f};
  return &vec;
}

static inline bool VectorIsZero(const Vector *vec) {
  return ((vec->x == 0.0f) && (vec->y == 0.0f));
}

static inline Vector *VectorAdd(Vector *vec, const Vector *other) {
  vec->x += other->x;
  vec->y += other->y;
  return vec;
}

static inline Vector *VectorSub(Vector *vec, const Vector *other) {
  vec->x -= other->x;
  vec->y -= other->y;
  return vec;
}

static inline Vector *VectorMul(Vector *vec, float k) {
  vec->x *= k;
  vec->y *= k;
  return vec;
}

static inline float VectorMag(const Vector *vec) {
  return SDL_sqrtf(SDL_powf(vec->x, 2.0f) + SDL_powf(vec->y, 2.0f));
}

static inline Vector *VectorNorm(Vector *vec) {
  float mag = VectorMag(vec);
  if (mag != 0.0f) {
    vec->x /= mag;
    vec->y /= mag;
  }
  return vec;
}

static inline Vector *VectorCap(Vector *vec, const Vector *min,
                                const Vector *max) {
  vec->x = MIN(vec->x, max->x);
  vec->y = MIN(vec->y, max->y);
  vec->x = MAX(vec->x, min->x);
  vec->y = MAX(vec->y, min->y);
  return vec;
}

#endif /* __ETERNO_VECTOR_H__ */
