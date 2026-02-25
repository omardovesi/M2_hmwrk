#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4096

// Merge sort helper
void merge(char **arr, char **temp, int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (strcmp(arr[i], arr[j]) <= 0) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    memcpy(arr + left, temp + left, (right - left + 1) * sizeof(char *));
}

void mergesort(char **arr, char **temp, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergesort(arr, temp, left, mid);
        mergesort(arr, temp, mid + 1, right);
        merge(arr, temp, left, mid, right);
    }
}

int main() {
    // Large input/output buffers
    char inbuf[65536];
    setvbuf(stdin, inbuf, _IOFBF, sizeof(inbuf));

    char **lines = malloc(INITIAL_CAPACITY * sizeof(char *));
    char **temp  = malloc(INITIAL_CAPACITY * sizeof(char *));
    int capacity = INITIAL_CAPACITY;
    int size = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (size >= capacity) {
            capacity *= 2;
            lines = realloc(lines, capacity * sizeof(char *));
            temp  = realloc(temp,  capacity * sizeof(char *));
        }
        lines[size] = malloc(read + 1);
        memcpy(lines[size], line, read + 1);
        size++;
    }
    free(line);

    if (size > 0) mergesort(lines, temp, 0, size - 1);

    // Large output buffer
    char outbuf[65536];
    setvbuf(stdout, outbuf, _IOFBF, sizeof(outbuf));

    for (int i = 0; i < size; i++) {
        fputs(lines[i], stdout);
        free(lines[i]);
    }

    free(lines);
    free(temp);
    return 0;
}