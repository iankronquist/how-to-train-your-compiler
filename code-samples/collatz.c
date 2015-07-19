#include <stdbool.h>
#include <stdio.h>

bool is_collatz(int candidate) {
        if (candidate == 1) {
                return true;
        } else if (candidate % 2 == 0) { // If it's even
                candidate = candidate / 2;
        } else { // If it's odd
                candidate = 3 * candidate + 1;
        }
        return is_collatz(candidate);
}

int main() {
        printf("Is 27 a Collatz number?\n");
        if (is_collatz(27)) {
                printf("Yes!\n");
        } else {
                printf("No :(\n");
        }
}
