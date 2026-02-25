# Debug Insecure Code - Part 2A

## Overview
In this part, we analyzed a C program (`insecure/main.c`) that contains a buffer overflow
vulnerability using `gets()`, shared memory between forked processes, and a global variable `m`.
We used GDB to observe the behavior before and after the overflow.

---

## Program Description
The program:
1. Creates a shared memory region between parent and child using `mmap()`
2. Forks a child process
3. In the child, declares a 16-byte buffer and reads input using the unsafe `gets()` function
4. The parent waits for the child and prints the shared memory value

The global variable `m = 42` is declared at the top of the program.

---

## Compilation
```bash
gcc -o overflow main.c -fno-stack-protector -z execstack -g
```

The `-fno-stack-protector` flag disables stack canaries, and `-z execstack` makes the
stack executable. These flags are used purely for academic purposes to make the
buffer overflow behavior observable.

The compiler immediately warned us:
```
warning: implicit declaration of function 'gets'
warning: the `gets' function is dangerous and should not be used.
```

This confirms that `gets()` is so dangerous that modern compilers warn against it by default.

---

## Run 1: With Buffer Overflow (`gets()`)

### GDB Session
We ran the program in GDB with `set follow-fork-mode child` to attach to the child process.

```
(gdb) set follow-fork-mode child
(gdb) b gets
(gdb) r
```

### Stack Trace BEFORE Overflow
Taken at the breakpoint when `gets()` was called:
```
#0  _IO_gets (buf=0xfffffffff420 "@\365\377\377\377\377") at ./libio/iogets.c:37
#1  0x0000aaaaaaaa0a8c in main (argc=1, argv=0xfffffffff5b8) at main.c:35
```

The buffer starts at address `0xfffffffff420`.
The global variable `m` is at address `0xaaaaaaab1010`.

### Input Provided
We typed 31 'A' characters (well beyond the 16-byte buffer limit):
```
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
```

### Stack Trace AFTER Overflow
```
#0  0x0000aaaaaaaa0ac4 in main (argc=1, argv=0xfffffffff5b8) at main.c:44
```

The program received `SIGSEGV` (segmentation fault) at line 44 (`*shared = 1`),
meaning the overflow corrupted the stack enough to cause a crash when the child
tried to write to shared memory.

---

## Run 2: With Safe Version (`fgets()`)
We commented out `gets(buffer)` and uncommented `fgets(buffer, sizeof(buffer), stdin)`.

With the same long input, `fgets()` safely truncated the input to 15 characters
(plus null terminator), and the program completed without any crash or segfault.

---

## Analysis and Observations

### Buffer Overflow Impact
`gets()` does not check the buffer size. When we typed 31 characters into a 16-byte
buffer, it wrote 31 bytes starting at `0xfffffffff420`, overwriting memory beyond
the buffer. This corrupted the stack frame, leading to a segfault when the child
tried to execute `*shared = 1`.

### Why m Was Not Changed
Even though the overflow was significant, the global variable `m` (at address
`0xaaaaaaab1010`) was not overwritten. This is because `m` is a global variable
stored in the data segment, which is at a completely different memory region from
the stack where `buffer` lives. The overflow only affects adjacent stack memory.

### Shared Memory and Fork
Each child process gets its own copy of the stack and global variables. So even
if `m` were changed in the child, the parent's `m` would remain 42. The shared
memory (`mmap`) is the only region truly shared between parent and child — changes
there ARE visible across processes.

### gets() vs fgets()
| Feature | `gets()` | `fgets()` |
|---------|----------|-----------|
| Buffer size check | No | Yes |
| Buffer overflow risk | High | None |
| Newline handling | Discards `\n` | Keeps `\n` |
| Safety | Unsafe (deprecated) | Safe |

---

## Conclusion
This exercise demonstrated how a simple use of `gets()` can cause a segmentation
fault by overflowing a stack buffer and corrupting adjacent memory. The fix is
straightforward: always use `fgets()` with an explicit size limit. Modern compilers
warn against `gets()` precisely because of vulnerabilities like this. In real-world
systems, buffer overflows like this can be exploited to execute arbitrary code,
making them one of the most dangerous classes of security vulnerabilities.
