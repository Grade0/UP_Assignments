/**
	Name: Chen Davide
	Student number: 19207819
	email:davide.chen@ucdconnect.ie
*/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
/*
  Function Declarations for builtin shell commands:
 */
int d_cd(char **args);
int d_help(char **args);
int d_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &d_cd,
  &d_help,
  &d_exit
};

int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/


int d_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "Error: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror(args[1]);
    }
  }
  return 1;
}


int d_help(char **args) {
  int i;
  printf("Simplified Shell\n");
  printf("Type command name and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}


int d_exit(char **args) {
  return 0;
}


int launch(char **args) {
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror(args[0]);
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("Error 1");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


int execute(char **args) {
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}


char **split_line(char *line) {

  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  token = strtok(line, TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
    }

    token = strtok(NULL, TOK_DELIM);
  }
  tokens[position] = NULL;

  return tokens;
}
