#include <stdio.h>    // for printf, perror, getline
#include <stdlib.h>   // for memory allocating functions (malloc, free), as well as exit
#include <string.h>   //working with strings
#include <unistd.h>   // fork, execvp, pipe, chdir, dup2
#include <sys/wait.h> // wait, waitpid

#define MAX_ARGS 100   // max number of arguments for one command
#define MAX_STAGES 10  // max number of pipeline stages
#define MAX_ALIASES 20 // max number of stored aliases

typedef struct // structure to store aliases
{
  char name[50];
  char cmd[200];
} Alias;

Alias aliases[MAX_ALIASES]; // array to store the aliases
int alias_count = 0;        // counter for the nubmer of aliases

int split(char *str, char *delim, char **arr)
{                                   // function that splits strings into parts
  int i = 0;                        // token counter
  char *token = strtok(str, delim); // split string into tokens
  while (token != NULL)             // while there are still tokens
  {
    while (*token == ' ') // trip leading spaces
      token++;            // increase the number of tokens

    char *end = token + strlen(token) - 1; // trim trailing spaces
    while (end > token && *end == ' ')     // when the token is trimemd
    {
      *end = '\0'; // end with \0 to signify the end of the token
      end--;
    }

    if (*token != '\0') // skip empty tokens
      arr[i++] = token;

    token = strtok(NULL, delim); // gets the next token
  }
  return i; // return the number of tokens
}

char *check_alias(char *word) // check for an alias
{
  for (int i = 0; i < alias_count; i++)
  {
    if (strcmp(word, aliases[i].name) == 0) // if the word matches an alias
      return aliases[i].cmd;                // return the command of that alias
  }
  return NULL; // otherwise, return NULL
}

void expand_aliases_args(char **args, int *argc)
{
  char *alias_cmd = check_alias(args[0]); // check if the first word is an alias
  if (!alias_cmd)
    return; // if not, skip

  char *temp[MAX_ARGS]; // tokenize alias command into temp array
  int temp_argc = 0;    // counter

  char buf[200];                // buffer
  strcpy(buf, alias_cmd);       // copy the command to buffer
  char *tok = strtok(buf, " "); // split the string into tokens
  while (tok)
  {
    temp[temp_argc++] = strdup(tok); // make a separate copy of tok and store in temp
    tok = strtok(NULL, " ");         // get the next token
  }

  // shift original args to make room
  char *rest[MAX_ARGS];
  int rest_argc = *argc - 1;
  for (int i = 0; i < rest_argc; i++)
    rest[i] = args[i + 1];

  // rebuild args[]
  int j = 0;
  for (int i = 0; i < temp_argc; i++)
    args[j++] = temp[i];
  for (int i = 0; i < rest_argc; i++)
    args[j++] = rest[i];

  args[j] = NULL;
  *argc = j;
}

int main(void)
{
  char *line = NULL; // input buffer
  size_t cap = 0;    // buffer size for getline()

  while (1)
  {
    printf("<mini-shell> "); // start by writing "<mini-shell> "
    fflush(stdout);          // flush outputs

    ssize_t n = getline(&line, &cap, stdin); // read the line from the input
    if (n < 0)                               // break if it doesn't exist
      break;

    if (n > 0 && line[n - 1] == '\n') // remove trailing newline
      line[n - 1] = '\0';

    if (strcmp(line, "quit") == 0) // exit if the line is 'quit'
      break;

    // -- CD WORK --

    if (strncmp(line, "cd ", 3) == 0) // if the line is 'cd'
    {
      char *dir = line + 3;      // get the directory after cd
      if (strcmp(dir, "~") == 0) // if dir = ~
      {
        dir = getenv("HOME"); // go to home directory
        if (!dir)             // if no home set
        {
          fprintf(stderr, "cd: HOME not set\n");
          continue;
        }
      }
      if (chdir(dir) != 0) // if change directory finds an error
      {
        perror("cd");
      }
      continue;
    }

    printf("%s\n", line); // print the input line

    // -- ALIAS WORK --

    if (strncmp(line, "alias ", 6) == 0) // if the first word is alias
    {
      char *eq = strchr(line, '='); // searching for the '=' symbol in the line
      if (eq != NULL)               // when found
      {
        *eq = '\0';            // replace '=' with \0
        char *name = line + 6; // find the mame of the alias
        char *cmd = eq + 1;    // pointer to the start of the command

        if (cmd[0] == '\'') // remove quotations if there are any
          cmd++;
        if (cmd[strlen(cmd) - 1] == '\'')
          cmd[strlen(cmd) - 1] = '\0';

        // overwrite if alias exists
        int found = 0;
        for (int i = 0; i < alias_count; i++)
        {
          if (strcmp(aliases[i].name, name) == 0)
          {
            strcpy(aliases[i].cmd, cmd);
            found = 1;
            break;
          }
        }
        if (!found) // if not found, add the new alias
        {
          strcpy(aliases[alias_count].name, name);
          strcpy(aliases[alias_count].cmd, cmd);
          alias_count++;
        }
      }
      continue;
    }

    // -- PIPELINE WORK --

    char *stages[MAX_STAGES];                  // stages pointer
    int num_stages = split(line, "|", stages); // split the line into pipeline stages

    int pipefd[2 * (num_stages - 1)]; // array of file descriptors for pipes

    for (int i = 0; i < num_stages - 1; i++) // create pipes between stages
    {
      if (pipe(pipefd + i * 2) < 0)
      {
        perror("pipe");
        exit(EXIT_FAILURE);
      }
    }

    for (int i = 0; i < num_stages; i++) // split the stages into argumenst
    {
      char *args[MAX_ARGS];
      int argc = split(stages[i], " ", args);
      args[argc] = NULL;

      expand_aliases_args(args, &argc); // expamf alias

      pid_t pid = fork(); // fork child process
      if (pid < 0)        // if fork failed
      {
        perror("Fork failed");
        return 1;
      }

      if (pid == 0)
      {
        if (i > 0)                      // if not the first stage
          dup2(pipefd[(i - 1) * 2], 0); // set stdin to previous pipe’s read end

        if (i < num_stages - 1)       // if not last stage
          dup2(pipefd[i * 2 + 1], 1); // set stdout to current pipe’s write end

        for (int j = 0; j < 2 * (num_stages - 1); j++) // close the pipes
          close(pipefd[j]);

        execvp(args[0], args); // execute the command
        perror("execvp");      // only reached if exec fails
        exit(EXIT_FAILURE);
      }
    }

    for (int i = 0; i < 2 * (num_stages - 1); i++) // parent process closes all pipes
      close(pipefd[i]);

    for (int i = 0; i < num_stages; i++) // waits for all the child processess to finish
      wait(NULL);
  }

  free(line); // free line pointer
  return 0;   // exit
}