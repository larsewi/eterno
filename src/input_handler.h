#ifndef __ETERNO_INPUT_HANDLER_H__
#define __ETERNO_INPUT_HANDLER_H__

#include "game.h"

typedef struct InputHandler InputHandler;

InputHandler *InputHandlerInit(void);

void InputHandlerHandleEvents(InputHandler *input_handler);

bool InputHandlerGameShouldQuit(InputHandler *input_handler);

void InputHandlerDestroy(InputHandler *input_handler);

#endif /* __ETERNO_INPUT_HANDLER_H__ */
