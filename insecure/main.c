#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Global variable - will be near buffer on stack in child
int m = 42;

int main(int argc, char *argv[]) {
    // Set up shared memory between parent and child
    int *shared = mmap(NULL, sizeof(int),
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    *shared = 0;

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        char buffer[16];

        printf("Buffer address: %p\n", buffer);
        printf("m address: %p\n", &m);

        printf("Enter some text: ");
        fflush(stdout);

        // UNSAFE: gets() does not check buffer size - buffer overflow vulnerability
        gets(buffer);

        // Comment out the above and uncomment below for safe version:
        // fgets(buffer, sizeof(buffer), stdin);

        printf("You entered: %s\n", buffer);
        printf("m is now: %d\n", m);

        // Modify shared memory
        *shared = 1;

        exit(0);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);

        printf("Parent: shared memory value = %d\n", *shared);
        printf("Parent: m = %d (unchanged in parent)\n", m);
    }

    munmap(shared, sizeof(int));
    return 0;
}