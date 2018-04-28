// Exit code status
// 3 : fork error
// 4 : execvp error
// 5 : fopen error
// 6 : fclose error
// 7 : Error signal catch doesn't match
// 8 : calloc error
// 9 : pipe error
// 10 : stat error

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
    fprintf(stderr, "Error, wrong signal catched");
    exit(7);
  }

  sig_int = true;
 
}


/*##########################################################################################
                             Main program
##########################################################################################*/

int main() {
  char cwd[256];
  struct stat sf;

  struct line li;
  char buf[BUFLEN];
  int status;

  //Signal gestion
  struct sigaction action;
  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);

  action.sa_handler = handSIGINT;
  sigaction(SIGINT, &action, NULL);


  line_init(&li);

  for (;;) {
  	if (getcwd(cwd, sizeof(cwd)) == NULL) {
      perror("getcwd");
  	}
    else {
      printf("fish %s>", cwd);
    }
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

    if(li.ncmds == 0){
      printf("");
      continue;
    }

    if(strcmp(li.cmds[0].args[0], "exit") == 0) {
      line_reset(&li);
		  break;
	  } 
    else if (strcmp(li.cmds[0].args[0], "cd") == 0) {
  		if (stat(li.cmds[0].args[1], &sf) == 0) {    	
  			if(S_ISDIR(sf.st_mode)){
  				chdir(li.cmds[0].args[1]);
  			} else {
  				fprintf(stderr, "Error, %s is not a folder\n", li.cmds[0].args[1]);
  		}
  		} else {
  			fprintf(stderr, "Error, %s is not a folder\n", li.cmds[0].args[1]);
  		}
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
    
    //streams reset
    FILE *input =stdin;
    FILE *output = stdout;
    
    struct pipe_tab pipes;
      if(li.ncmds > 1){ 
        init_pipe_tab(&pipes, li.ncmds);
       }

    for(size_t i = 0; i < li.ncmds; ++i){

      //initilise pipe
      if(i < pipes.nbpipe && pipe(pipes.tab[i]->descriptor) != 0){
        perror("pipe");
        exit(9);
      }

      pid_t pid = fork();

      switch(pid){
        case -1 : 
          perror("fork");
          exit(3);
        break;

            
        case 0:   // ------------------ CHILD PROCESSUS --------------------------//

          //reinit signal default action for SIGINT
          action.sa_handler = SIG_DFL;
          sigaction(SIGINT, &action, NULL);

          //Redirect input and output to a file if needed
          input_redirection(input, li);
          output_redirection(output, li);

          //pipe creation and close descriptor for son
          pipe_redirect_input(&li, &pipes, i);



          execvp(li.cmds[i].args[0], li.cmds[i].args);
          perror("execvp");
          exit(4);
        break;  // ------------------ END CHILD PROCESSUS --------------------------//


        default : // ------------------ FATHER PROCESSUS --------------------------//

          //Ignore SIGINT suring child process
          action.sa_handler = SIG_IGN;
          sigaction(SIGINT, &action, NULL);

          //close unused dexcriptors
          pipe_father_cose_descriptor(&li, &pipes, i);
   
          //wait ending of son and close the open streams
          if(!li.background)
            waitpid(pid, &status, 0);

          //close open files if needed
          input_close(input);
          output_close(output);

          //reset the action associate to SIGINT
          action.sa_handler = handSIGINT;
          sigaction(SIGINT, &action, NULL);
      }     // ------------------ END FATHER PROCESSUS ----------------------------//
          
    } 

    if(WIFEXITED(status)){
      fprintf(stdout,"\nThe execution has correctly ended with code %d\n", WEXITSTATUS(status));
    }
    if(WIFSIGNALED(status)){
      fprintf(stdout,"\nIt has ended by signal n°%d\n", WTERMSIG(status));
    }   

    pipe_tab_destroy(&pipes);

    line_reset(&li);
  }
  
  return 0;
}

