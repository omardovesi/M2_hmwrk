#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LEN 256
#define MAX_DICT_WORDS 4000

// Binary search - O(log n) for fast lookup
int binary_search(char dict[][MAX_WORD_LEN], int size, const char *word) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(word, dict[mid]);

        if (cmp == 0) return 1;  // found
        if (cmp < 0) right = mid - 1;
        else left = mid + 1;
    }
    return 0;  // not found
}

int main() {
    // Use static 2D array instead of dynamic allocation - less memory overhead
    static char dict[MAX_DICT_WORDS][MAX_WORD_LEN];
    int dict_size = 0;
    char buffer[MAX_WORD_LEN];
    FILE *dict_file;

    // Read dictionary file
    dict_file = fopen("unix_dict.text", "r");
    if (!dict_file) {
        perror("ERROR: could not open unix_dict.text");
        return 1;
    }

    // Load all dictionary words into static array
    while (fgets(buffer, sizeof(buffer), dict_file) && dict_size < MAX_DICT_WORDS) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        if (len > 0) {
            strncpy(dict[dict_size], buffer, MAX_WORD_LEN - 1);
            dict[dict_size][MAX_WORD_LEN - 1] = '\0';
            dict_size++;
        }
    }
    fclose(dict_file);

    // Read words from stdin and binary search dictionary
    while (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        if (len > 0 && !binary_search(dict, dict_size, buffer)) {
            puts(buffer);
        }
    }

    return 0;
}