# Debug A1 - Program 2: unique.c

# Group Member - Omar Dovesi


## Overview
We analyzed `unique.c` from Assignment 1 using GDB and the AddressSanitizer.
This program reads lines from stdin and outputs only consecutive unique lines,
removing duplicate consecutive lines.

---

## Compilation
```bash
gcc -fsanitize=address -Wall -Wextra -g -o unique unique.c
```
Compiled successfully with no warnings.

---

## Testing
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

Correct output. No AddressSanitizer or LeakSanitizer warnings detected on
typical inputs.

---

## GDB Analysis
We ran the program under GDB and stepped through the while loop:

```
gdb unique
(gdb) b main
(gdb) r
(gdb) n   # step through line by line
```

Observed:
- `fgets()` correctly reads each line into `buffer`
- `strcmp()` correctly compares `buffer` with `previous`
- `strcpy()` correctly updates `previous` after each unique line
- No crashes or unexpected behavior on normal inputs

---

## Bug Found: Fixed-Size Buffer Causes Silent Truncation

The program uses fixed-size buffers of 1024 bytes:

```c
char buffer[1024];
char previous[1024] = "";
```

If any input line is longer than 1023 characters, `fgets()` will silently
truncate it. This means:
- The truncated portion of the line is treated as a new line on the next
  `fgets()` call
- The program may incorrectly output duplicate content
- No error is reported — the output is silently wrong

This is a silent data corruption bug that only appears with unusually long lines.

### Demonstrating the Bug
```bash
# Create a line longer than 1024 characters
python3 -c "print('A' * 1100)" | ./unique
```

The output will be split across two lines instead of one, showing the truncation.

---

## Fix Implemented
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
            // Resize previous if needed
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

### Why This Fix Works
- `getline()` dynamically resizes `buffer` as needed — no truncation
- `previous` is also dynamically allocated and resized
- All memory is properly freed at the end — no memory leaks
- The fix was verified with AddressSanitizer showing no warnings

---

## Summary

| Issue | Description | Fix |
|-------|-------------|-----|
| Fixed-size buffer | Lines > 1023 chars are silently truncated | Use `getline()` with dynamic allocation |
| Potential unsafe `strcpy` | If buffer somehow exceeded previous size | Use `memcpy` with explicit size |

The program is functionally correct for typical inputs but has a hidden bug
for long lines. Switching to `getline()` makes it robust for all input sizes.