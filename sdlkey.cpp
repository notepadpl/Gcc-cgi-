
#include <stdio.h>
#include <SDL/SDL.h>
#include <string>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
std::string inputText = "";

// Zmienna globalna, aby kontrolować czy program ma się zakończyć
int running = 1;
int main(int argc, char** argv) {
  printf("hello, world!\n");
   const char* key;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE);

#ifdef TEST_SDL_LOCK_OPTS
  EM_ASM("SDL.defaults.copyOnLock = false; SDL.defaults.discardOnLock = true; SDL.defaults.opaqueFrontBuffer = false;");
#endif

  if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 256; j++) {
#ifdef TEST_SDL_LOCK_OPTS
      // Alpha behaves like in the browser, so write proper opaque pixels.
      int alpha = 255;
#else
      // To emulate native behavior with blitting to screen, alpha component is ignored. Test that it is so by outputting
      // data (and testing that it does get discarded)
      int alpha = (i+j) % 255;
#endif
      *((Uint32*)screen->pixels + i * 256 + j) = SDL_MapRGBA(screen->format, i, j, 255-i, alpha);
    }
  }

SDL_Event event;
    
    // Sprawdzamy wszystkie zdarzenia SDL
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = 0;
            //emscripten_cancel_main_loop(); // Zatrzymujemy pętlę gdy zamykamy aplikację
        } else if (event.type == SDL_KEYDOWN) {
            // Obsługujemy naciśnięcie klawisza
             key = SDL_GetKeyName(event.key.keysym.sym);

            // Sprawdzamy, czy wciśnięty klawisz to pojedyncza litera lub liczba
            if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
                inputText += key;  // Dodajemy wprowadzony klawisz do zmiennej tekstowej
            }

          
        }
    }
      if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  SDL_Flip(screen);
  printf("Key pressed: %s\n", key);
            printf("Current input: %s\n", inputText.c_str());  // Wyświetlamy aktualny stan zmiennej

  SDL_Quit();

  return 0;
}