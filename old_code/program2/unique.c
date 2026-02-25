#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[1024];
    char previous[1024] = "";
    int first_line = 1;

    // Read data from stdin and write it to stdout (standard output)
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Only output if different from previous line
        if (first_line || strcmp(buffer, previous) != 0) {
            fputs(buffer, stdout);
            strcpy(previous, buffer);
            first_line = 0;
        }
    }

    return 0;
}