/**
	Name: Chen Davide
	Student number: 19207819
	email:davide.chen@ucdconnect.ie
*/

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

char *read_input(void);
char **split_line(char *input);
int execute(char **args);

int main(int argc, char **argv) {
  
  char *input; //variable to store the input line 
  char **args; //variable to store the arguments of the line
  int flag;	//flag for the loop

  //variables for the current date output
  char buf[128];
  struct tm *today;
  time_t now;

  do {

  	//print the current date
    time(&now);
    today = localtime(&now);
	strftime(buf,128,"%d/%m/%Y", today);
    printf("[%s]# ", buf);
  	
    signal(SIGINT, SIG_IGN); //ignore the SIGINT signal

    input = NULL;
  	size_t bufsize = 0;
  	if(getline(&input, &bufsize, stdin) == EOF) {
  		puts("");
  		return 0; //if stdin is EOF terminates the program
  	}
    args = split_line(input); //split the input line into arguments
    flag = execute(args); //excute the command and store the return value;

    free(input);
    free(args);
  } while (flag);


  return EXIT_SUCCESS;
}
