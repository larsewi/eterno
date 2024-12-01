#include "input_handler.h"
#include "config.h"
#include "utils.h"

#include <SDL3/SDL.h>
#include <assert.h>

struct InputHandler {
  bool should_quit;
};

InputHandler *InputHandlerInit(void) {
  InputHandler *input_handler = xmalloc(sizeof(input_handler));
  memset(input_handler, 0, sizeof(input_handler));
  return input_handler;
}

void InputHandlerHandleEvents(InputHandler *input_handler) {
  assert(input_handler != NULL);

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      input_handler->should_quit = true;
      break;

    default:
      break;
    }
  }
}

bool InputHandlerGameShouldQuit(InputHandler *input_handler) {
  return input_handler->should_quit;
}

void InputHandlerDestroy(InputHandler *input_handler) {
  if (input_handler == NULL) {
    return;
  }

  free(input_handler);
}
