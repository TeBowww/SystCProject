// Exit code status
// 3 : fork error
// 4 : execvp error
// 5 : fopen error
// 6 : fclose error
// 7 : Error signal catch doesn't match


/*##########################################################################################
                             Includes and define
##########################################################################################*/

#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

#include "cmdline.h"
#include "streams.h"

#define BUFLEN 2048

#define YESNO(i) ((i) ? "Y" : "N")

bool sig_int = false;

void handSIGINT(int sig){

  //Check the signal recieved
  if(sig != SIGINT){
    fprintf(stderr, "error, wrong signal catched");
    exit(7);
  }

  sig_int = true;
 
}



/*##########################################################################################
                             Main program
##########################################################################################*/

int main() {
  struct line li;
  char buf[BUFLEN];
  int status;

  FILE *input =stdin;
  FILE *output = stdout;

  //Signal gestion
  struct sigaction action;
  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);

  action.sa_handler = handSIGINT;
  sigaction(SIGINT, &action, NULL);


  line_init(&li);

  for (;;) {

    printf("fish> ");
    fgets(buf, BUFLEN, stdin);

    if(sig_int == true){
      sig_int = false;
      printf("\n");
      continue;
    }

    int err = line_parse(&li, buf);
    if (err) { 
      //the command line entered by the user isn't valid
      line_reset(&li);
      continue;
    }

    fprintf(stderr, "Command line:\n");
    fprintf(stderr, "\tNumber of commands: %zu\n", li.ncmds);

    for (size_t i = 0; i < li.ncmds; ++i) {
      fprintf(stderr, "\t\tCommand #%zu:\n", i);
      fprintf(stderr, "\t\t\tNumber of args: %zu\n", li.cmds[i].nargs);
      fprintf(stderr, "\t\t\tArgs:");
      for (size_t j = 0; j < li.cmds[i].nargs; ++j) {
        fprintf(stderr, " \"%s\"", li.cmds[i].args[j]);
      }
      fprintf(stderr, "\n");
    }

    fprintf(stderr, "\tRedirection of input: %s\n", YESNO(li.redirect_input));
    if (li.redirect_input) {
      fprintf(stderr, "\t\tFilename: '%s'\n", li.file_input);
    }

    fprintf(stderr, "\tRedirection of output: %s\n", YESNO(li.redirect_output));
    if (li.redirect_output) {
      fprintf(stderr, "\t\tFilename: '%s'\n", li.file_output);
    }

    fprintf(stderr, "\tBackground: %s\n", YESNO(li.background));


    // --------------------------------------- Main bloc -----------------------//

    pid_t pid = fork();

    switch(pid){
      case -1 : 
        perror("fork");
        exit(3);
      break;

      case 0:
        //reinit signal default action for SIGINT
        action.sa_handler = SIG_DFL;
        sigaction(SIGINT, &action, NULL);

        //Redirect input and output to a file if needed
        input_redirection(input, li);
        output_redirection(output, li);

        execvp(li.cmds[0].args[0], li.cmds[0].args);
        perror("execvp");
        exit(4);
      break;

      default:
        //Ignore SIGINT suring child process
        action.sa_handler = SIG_IGN;
        sigaction(SIGINT, &action, NULL);
 
        //wait ending of son and close the open streams
        waitpid(pid, &status, 0);
        input_close(input);
        output_close(output);

        //reset the action associate to SIGINT
        action.sa_handler = handSIGINT;
        sigaction(SIGINT, &action, NULL);
    
        if(WIFEXITED(status)){
        fprintf(stdout,"\nThe execution has correctly ended with code %d\n", WEXITSTATUS(status));
        }
        if(WIFSIGNALED(status)){
          fprintf(stdout,"\nIt has ended by signal n°%d\n", WTERMSIG(status));
        }
    }
    line_reset(&li);
  }
  
  return 0;
}

