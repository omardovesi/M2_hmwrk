#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @file 412shell.c
 * @brief A simple command interpreter.
 */

/**
 * @brief Maximum number of arguments allowed for a command.
 */
#define MAX_ARGS 64

/**
 * @brief Maximum length of user input allowed.
 */
#define MAX_INPUT 1024

/**
 * @brief A struct to hold the parsed command.
 */
typedef struct {
    /**
     * @brief The command to execute.
     */
    char* command;

    /**
     * @brief The arguments to pass to the command.
     */
    char* args[MAX_ARGS];

    /**
     * @brief Number of arguments in args.
     */
    unsigned short numArgs;

    /**
     * @brief Output filename for redirection, if any.
     */
    char* outputFile;
} ParsedCommand;

/**
 * @brief Forks the current process and executes the parsed command in the child process.
 *
 *  This is the function you will use to fork to create a child process
 *  The child process will execute a user specified command (see the given ParsedCommand struct)
 *  The three major steps are listed as TODOs for you below
 *  Make sure you handle any errors and/or fork failure appropriately
 *
 * @param command The parsed command to execute.
 */
void do_fork(const ParsedCommand* command) {
    // Safety check: ensure command and command->command are valid
    if (command == NULL || command->command == NULL) {
        fprintf(stderr, "Error: Invalid command\n");
        return;
    }
    
    // TODO 1: Use 'fork' to create a child process
    pid_t child_pid = fork();

    if (child_pid < 0){
        perror("fork failed");
        return;
    }

    // Child process
    else if (child_pid == 0){
        // Build argv array (arguments array)
        char* argv[MAX_ARGS + 2]; //+2 for program name and NULL

        argv[0] = command->command; // First grab the program name

        // Copy Arguments
        for (int i = 0; i < command->numArgs; i++){
            argv[i + 1] = command->args[i]; // argv[1]="hello", argv[2]="world"
        }
        argv[command->numArgs + 1] = NULL; // End with NULL

        execv(command->command, argv);
        perror("ERROR: execv failed!");
        exit(EXIT_FAILURE);
    }
    // TODO 2: Handle child process case: research file descriptors, 'dup2', and 'execv'

    // TODO 3: Handle parent process case: research 'waitpid'
    else if (child_pid > 0) {
        // Parent process
        int status;
        waitpid(child_pid, &status, 0);

    }
}

/**
 * @brief Parse input into a ParsedCommand struct.
 *
 * The input is expected to be in the format "command arg1 arg2 ... argN".
 *
 * @param input The input command string.
 * @return The parsed command.
 */
ParsedCommand parse_command(const char* input) {
    ParsedCommand command = {NULL, {NULL}, 0, NULL};

    char* token = strtok((char*)input, " ");
    while (token != NULL) {
        if (command.command == NULL) {
            command.command = token;
        } else if (strcmp(token, ">") == 0) { // is strcmp a memory safe operation? what are some alternatives?
            // Advance to next token for output file
            token = strtok(NULL, " ");
            command.outputFile = token;
        } else {
            command.args[command.numArgs++] = token;
        }
        token = strtok(NULL, " ");
    }

    // Remove wrapping quotes from arguments
    // Variables are just for readability :)
    char firstArgFirstChar = command.args[0][0];
    unsigned int lastArgIndex = command.numArgs - 1;
    unsigned int lastArgLastCharIndex = strlen(command.args[lastArgIndex]) - 1;
    char lastArgLastChar = command.args[lastArgIndex][lastArgLastCharIndex];

    if (command.numArgs > 0 && firstArgFirstChar == '"' && lastArgLastChar == '"') {
        // Remove the last quote first. If there is only one arg, then shifting the
        // string to remove the first quote will change the index of the last quote.
        // Removing the last quote first means we can avoid another strlen operation.
        command.args[lastArgIndex][lastArgLastCharIndex] = '\0';
        command.args[0]++;
    }

    return command;
}

/**
 * @brief Main function for the shell.
 *
 * Reads input from the user, parses it into a command and its arguments, then creates a child process to execute the command.
 *
 * @return 0 on success.
 */
int main(void) {
    char input[MAX_INPUT]; // Stores user input

    // Main loop: prompt for input, parse, and execute commands until "exit" is entered
    // is strcmp a memory safe operation? what are some alternatives?
    while (printf("412shell> ") && fflush(stdout) == 0 && fgets(input, MAX_INPUT, stdin) && strcmp(input, "exit\n") != 0) {
        // Strip newline from input
        char* newline = strchr((char*)input, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        ParsedCommand command = parse_command(input); // Parse the user input into a command
        do_fork(&command);
    }

    return 0;
}
