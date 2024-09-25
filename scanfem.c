#include <stdio.h>
#include <emscripten.h>

EM_JS(int, get_user_input, (), {
    var input = prompt("Enter a number:");
    return parseInt(input);
});

int main() {
    int number = get_user_input();
    printf("You entered: %d\n", number);
    return 0;
}
