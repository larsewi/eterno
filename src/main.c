#include "config.h"

#include <SDL3/SDL.h>
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "logger.h"

#define GAME_TITLE "Eterno"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

static const struct option LONG_OPTIONS[] = {
    {"debug", no_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0},
};

static const char *const DESCRIPTIONS[] = {
    "enable debug logging",
    "print help message",
};

static void PrintHelp(const char *prog) {
  printf("%s %s: %s\n\n", PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_DESCRIPTION);

  printf("Usage: %s [OPTIONS] SOURCE\n\n", prog);

  size_t longest = 0;
  for (int i = 0; LONG_OPTIONS[i].val != 0; i++) {
    const size_t length = strlen(LONG_OPTIONS[i].name);
    if (length > longest) {
      longest = length;
    }
  }

  char format[64];
  int ret = snprintf(format, sizeof(format), "  --%%-%zus    %%s\n", longest);
  assert(ret >= 0 && (size_t)ret < sizeof(format));

  printf("OPTIONS:\n");
  for (int i = 0; LONG_OPTIONS[i].val != 0; i++) {
    printf(format, LONG_OPTIONS[i].name, DESCRIPTIONS[i]);
  }

  printf("\nReport bugs to: <%s>\n", PACKAGE_BUGREPORT);
  printf("%s home page: <%s>\n", PACKAGE_NAME, PACKAGE_URL);
}

int main(int argc, char *argv[]) {
  int c;
  while ((c = getopt_long(argc, argv, "dh", LONG_OPTIONS, NULL)) != -1) {
    switch (c) {
    case 'd':
      SetDebugLogging(true);
      break;

    case 'h':
      PrintHelp(argv[0]);
      return EXIT_SUCCESS;

    case '?':
      // Error already printed by getopt_long(3)
      return EXIT_FAILURE;

    default:
      LOG_CRITICAL("Unhandled option '%c'", c);
    }
  }

  Game *game = GameInit(GAME_TITLE, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, false);
  if (game == NULL) {
    return EXIT_FAILURE;
  }

  GameDestroy(game);
  return EXIT_SUCCESS;
}