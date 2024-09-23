#include <SDL.h>
#include <emscripten.h>
#include <stdio.h>
#include <string>

// Globalna zmienna, która przechowuje wprowadzony tekst
std::string inputText = "";

// Zmienna globalna, aby kontrolować czy program ma się zakończyć
int running = 1;

// Główna funkcja obsługująca zdarzenia SDL
void main_loop() {
    SDL_Event event;
    
    // Sprawdzamy wszystkie zdarzenia SDL
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = 0;
            emscripten_cancel_main_loop(); // Zatrzymujemy pętlę gdy zamykamy aplikację
        } else if (event.type == SDL_KEYDOWN) {
            // Obsługujemy naciśnięcie klawisza
            const char* key = SDL_GetKeyName(event.key.keysym.sym);

            // Sprawdzamy, czy wciśnięty klawisz to pojedyncza litera lub liczba
            if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
                inputText += key;  // Dodajemy wprowadzony klawisz do zmiennej tekstowej
            }

            printf("Key pressed: %s\n", key);
            printf("Current input: %s\n", inputText.c_str());  // Wyświetlamy aktualny stan zmiennej
        }
    }

    // Dalsze operacje (np. renderowanie) mogą być tutaj
    // W tej chwili mamy tylko obsługę wejścia z klawiatury
}

int main(int argc, char* argv[]) {
    // Inicjalizacja SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Tworzymy okno SDL
    SDL_Window* window = SDL_CreateWindow("SDL Input Example", 
                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                          640, 480, SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Ustawienie pętli głównej Emscripten, która będzie cyklicznie wywoływać `main_loop`
    emscripten_set_main_loop(main_loop, 0, 1);

    // Zakończenie działania SDL i czyszczenie zasobów
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
