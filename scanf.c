#include <emscripten.h>
#include <stdio.h>
#include <string.h>

    void getUserInput() {
        // Przechwycenie wyniku z prompt i zapisanie go do zmiennej w C
        const char* result = emscripten_run_script_string("prompt('Enter some text:')");
        
        // Utworzenie zmiennej lokalnej w C i skopiowanie wyniku
        char userInput[256];  // Zakładamy, że maksymalna długość tekstu to 256 znaków
        strncpy(userInput, result, sizeof(userInput) - 1);  // Kopiujemy wynik do zmiennej
        userInput[sizeof(userInput) - 1] = '\0';  // Zapewniamy, że łańcuch znaków będzie zakończony '\0'

        // Wyświetlenie wyniku
        printf("User entered: %s\n", userInput);
    }


void loop() {
    emscripten_run_script("getUserInput();");
    emscripten_cancel_main_loop();  // Zatrzymaj pętlę po jednym wywołaniu
}

int main() {
    printf("WebAssembly is ready!\n");
    emscripten_set_main_loop(loop, 0, 1);  // Ustawiamy pętlę główną
    return 0;
}
