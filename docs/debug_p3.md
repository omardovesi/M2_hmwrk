# Debug A1 - Program 3: mismatch.c

# Group Member - Zachary Johnson

## Overview
We analyzed `mismatch.c` from Assignment 1 using GDB and the AddressSanitizer.
This program reads a dictionary file, then reads words from stdin and outputs
only the words NOT found in the dictionary (the "mismatches").

---

## Compilation
```bash
gcc -fsanitize=address -Wall -Wextra -g -o mismatch mismatch.c
```
Compiled successfully with no warnings.

---

## Testing
```bash
echo -e "unix\nhello\ntimesharing\nbanana" | ./mismatch
```

Expected output: words not in the dictionary.
No AddressSanitizer or LeakSanitizer warnings detected.

---

## GDB Analysis
We ran the program under GDB and stepped through `main()`:

```
gdb mismatch
(gdb) b main
(gdb) r
(gdb) n   # step through line by line
```

Observed:
- Dictionary loads correctly from `unix_dict.text`
- Binary search correctly finds/misses words
- `strncpy()` safely copies words into the static array
- No crashes or memory issues on normal inputs

---

## Bug Found: Hard-coded MAX_DICT_WORDS Limit

The program uses a fixed static array with a hard-coded maximum:

```c
#define MAX_DICT_WORDS 4000
static char dict[MAX_DICT_WORDS][MAX_WORD_LEN];
```

If the dictionary file contains more than 4000 words, the program silently
stops loading words at 4000 and continues without error. This means words
beyond the limit will never be found in the dictionary, causing the program
to incorrectly output them as mismatches.

The condition in the while loop:
```c
while (fgets(buffer, sizeof(buffer), dict_file) && dict_size < MAX_DICT_WORDS)
```
silently drops words beyond the limit with no warning to the user.

---

## Fix Implemented
Added a warning when the dictionary limit is reached so the user knows
the results may be incomplete:

```c
while (fgets(buffer, sizeof(buffer), dict_file) && dict_size < MAX_DICT_WORDS) {
    // ... load word ...
    dict_size++;
}

if (dict_size >= MAX_DICT_WORDS) {
    fprintf(stderr, "Warning: dictionary truncated at %d words. Results may be incomplete.\n", MAX_DICT_WORDS);
}
```

A more robust fix would use dynamic allocation to handle dictionaries of
any size:

```c
// Dynamic allocation approach
char **dict = malloc(capacity * sizeof(char *));
// realloc as needed when dict_size >= capacity
```

---

## Additional Observation: Binary Search Requires Sorted Input

The program uses binary search for O(log n) lookup, which is much faster
than linear search. However, binary search only works correctly if the
dictionary is already sorted. The program assumes `unix_dict.text` is
sorted but never verifies this. If an unsorted dictionary is provided,
binary search will silently return wrong results.

A defensive improvement would be to either:
1. Sort the dictionary after loading it
2. Add a check that verifies the dictionary is sorted and error out if not

---

## Summary

| Issue | Description | Fix |
|-------|-------------|-----|
| Hard-coded MAX_DICT_WORDS | Silently drops words beyond 4000 | Add warning or use dynamic allocation |
| Binary search assumes sorted input | Wrong results if dictionary unsorted | Sort after loading or verify order |

The program is correct for the A1 use case since `unix_dict.text` is small
and sorted. However, these issues would cause incorrect behavior in more
general use cases.