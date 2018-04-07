#include "streams.h"



/*##########################################################################################
    Here are defined the functions relative to streams gestions (opening, redirections, close)
    							such as files and pipes
##########################################################################################*/

void input_redirection(FILE *input, const struct line li){
  if(li.redirect_input){
    input = freopen(li.file_input, "r", stdin);
    if(input == NULL){
      perror("fopen");
      exit(5);
    }
  }
}

void input_close(FILE *input){
  if(input != stdin){
    if(fclose(input) == EOF){
      perror("fclose");
      exit(6);
    }
  }
}

void output_redirection(FILE *output, const struct line li){
  if(li.redirect_output){
    output = freopen(li.file_output, "w", stdout);
    if(output == NULL){
      perror("close");
      exit(5);
    }
  }
}

void output_close(FILE *output){
  if(output != stdout){
    if(fclose(output) == EOF){
      perror("fclose");
      exit(6);
    }
  }
}