# Debug A1 - GDB and Address Sanitizer Analysis

# Group Member - Raymond Owusu-Ansah

## Overview
Each group member selected one GNU utility from Assignment 1. We compiled each program
with `-fsanitize=address` and used GDB to inspect behavior and identify bugs or improvements.

---

## Program 1: sort.c

### Compilation
```
gcc -fsanitize=address -Wall -Wextra -g -o sort sort.c
```
Compiled successfully with no warnings.

### Testing
```bash
echo -e "banana\napple\ncherry\napple\nbanana" | ./sort
```
Output:
```
apple
apple
banana
banana
cherry
```
Correct output. No AddressSanitizer or LeakSanitizer warnings detected.

### GDB Analysis
Ran under GDB and stepped through `main()` and `mergesort()`. No crashes or
unexpected behavior observed. Memory was properly allocated and freed.

### Bug Found: Missing NULL Check After malloc/realloc
The program calls `malloc()` and `realloc()` without checking if the returned
pointer is NULL:

```c
char **lines = malloc(INITIAL_CAPACITY * sizeof(char *));
char **temp  = malloc(INITIAL_CAPACITY * sizeof(char *));
...
lines = realloc(lines, capacity * sizeof(char *));
temp  = realloc(temp,  capacity * sizeof(char *));
```

If the system runs out of memory, these calls return NULL, and the program will
immediately segfault when it tries to dereference the NULL pointer. This is
unsafe code that could cause hard-to-debug crashes in production.

### Fix Implemented
Added NULL checks after each malloc/realloc call:

```c
char **lines = malloc(INITIAL_CAPACITY * sizeof(char *));
if (lines == NULL) {
    fprintf(stderr, "Error: memory allocation failed\n");
    exit(1);
}
char **temp = malloc(INITIAL_CAPACITY * sizeof(char *));
if (temp == NULL) {
    fprintf(stderr, "Error: memory allocation failed\n");
    exit(1);
}
```

And similarly after each `realloc`:
```c
lines = realloc(lines, capacity * sizeof(char *));
if (lines == NULL) {
    fprintf(stderr, "Error: realloc failed\n");
    exit(1);
}
temp = realloc(temp, capacity * sizeof(char *));
if (temp == NULL) {
    fprintf(stderr, "Error: realloc failed\n");
    exit(1);
}
```

This ensures the program fails gracefully with a clear error message instead
of a cryptic segfault.

---

## Program 2: unique.c

### Compilation
```
gcc -fsanitize=address -Wall -Wextra -g -o unique unique.c
```
Compiled successfully with no warnings.

### Testing
```bash
echo -e "apple\napple\nbanana\ncherry\ncherry\ncherry\nbanana\napple" | ./unique
```
Output:
```
apple
banana
cherry
banana
apple
```
Correct output. No AddressSanitizer or LeakSanitizer warnings detected.

### GDB Analysis
Stepped through the while loop in GDB. The `strcmp()` comparison and `strcpy()`
update of `previous` worked correctly. No memory issues observed.

### Bug Found: Fixed-Size Buffer Causes Silent Truncation
The program uses fixed-size buffers of 1024 bytes:

```c
char buffer[1024];
char previous[1024] = "";
```

If any input line is longer than 1023 characters, `fgets()` will silently
truncate it. This means the program will incorrectly treat a truncated line
as unique even if the full line was a duplicate. This is a silent data
corruption bug — no error is reported, the output is just wrong.

Additionally, `strcpy(previous, buffer)` with a fixed-size destination is
unsafe if `buffer` ever exceeds the size of `previous`.

### Fix Implemented
Replaced fixed-size buffers with dynamic allocation using `getline()`, which
handles lines of any length safely:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *buffer = NULL;
    char *previous = NULL;
    size_t buf_len = 0, prev_len = 0;
    ssize_t read;
    int first_line = 1;

    while ((read = getline(&buffer, &buf_len, stdin)) != -1) {
        if (first_line || strcmp(buffer, previous) != 0) {
            fputs(buffer, stdout);
            // Update previous
            if (read + 1 > (ssize_t)prev_len) {
                free(previous);
                previous = malloc(read + 1);
                prev_len = read + 1;
            }
            memcpy(previous, buffer, read + 1);
            first_line = 0;
        }
    }
    free(buffer);
    free(previous);
    return 0;
}
```

This safely handles lines of any length without truncation or buffer overflow.

---

## Summary of Findings

| Program  | Bug Found | Type | Fixed |
|----------|-----------|------|-------|
| sort.c   | No NULL check after malloc/realloc | Unsafe code / potential crash | Yes |
| unique.c | Fixed-size buffer truncates long lines | Silent data corruption | Yes |

Both programs passed functional tests and showed no AddressSanitizer warnings
on typical inputs. The bugs identified are edge cases that would only manifest
under low memory conditions (sort.c) or unusually long input lines (unique.c).