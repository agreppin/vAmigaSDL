#include <SDL3/SDL_events.h>

typedef unsigned int action_t;

extern action_t convert_scancode(const SDL_KeyboardEvent *e);
