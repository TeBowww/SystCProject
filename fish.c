// Exit code status
// 3 : fork error
// 4 : execvp error
// 5 : fopen error
// 6 : fclose error


/*##########################################################################################
                             Includes and define
##########################################################################################*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>

#include "cmdline.h"
#include "streams.h"

#define BUFLEN 2048

#define YESNO(i) ((i) ? "Y" : "N")





/*##########################################################################################
                             Main program
##########################################################################################*/

int main() {
  struct line li;
  char buf[BUFLEN];

  line_init(&li);

  for (;;) {
    printf("fish> ");
    fgets(buf, BUFLEN, stdin);

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


   FILE *input =stdin;
   FILE *output = stdout;

    pid_t pid = fork();
    int status;

    switch(pid){
      case -1 : 
        perror("fork");
        exit(3);
      break;

      case 0:
        input_redirection(input, li);
        output_redirection(output, li);
        execvp(li.cmds[0].args[0], li.cmds[0].args);
        perror("execvp");
        exit(4);
      break;

      default:
        
        input_close(input);
        output_close(output);
 
        waitpid(pid, &status, 0);

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

