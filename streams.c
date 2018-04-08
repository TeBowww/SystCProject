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

void init_pipe_tab(struct pipe_tab *pi, size_t ncmds){
  pi->nbpipe = ncmds -1;
  pi->tab = calloc(pi->nbpipe, sizeof(struct pipe));
  if(pi->tab == NULL){
    perror("calloc");
    exit(8);
  }

  for(size_t i = 0; i < pi->nbpipe; ++i){
    struct pipe * new = calloc(1, sizeof(struct pipe));
    if(new == NULL){
      perror("calloc");
      exit(8);
    }
    pi->tab[i] = new;
  }
}

void pipe_redirect_input(struct line *li, struct pipe_tab *pipes, size_t i){

  if(li->ncmds < 2){
    return;
  }

  if(i == 0){
    dup2(pipes->tab[i]->descriptor[1], 1);
    close(pipes->tab[i]->descriptor[1]);
    close(pipes->tab[i]->descriptor[0]);
  }
  else if(i == pipes->nbpipe){
    dup2(pipes->tab[i-1]->descriptor[0], 0);
    close(pipes->tab[i-1]->descriptor[0]);
  }
  else{
    dup2(pipes->tab[i-1]->descriptor[0], 0);
    dup2(pipes->tab[i]->descriptor[1], 1);
    close(pipes->tab[i-1]->descriptor[0]);
    close(pipes->tab[i]->descriptor[1]);
    close(pipes->tab[i]->descriptor[0]);
  }
}

void pipe_tab_destroy(struct pipe_tab *pi){
  for(size_t i = 0; i < pi->nbpipe; ++i){
    free(pi->tab[i]);
  }
}

void pipe_father_cose_descriptor(struct line *li, struct pipe_tab *pipes, size_t i){

  if(li->ncmds < 2){
    return;
  }

  if(i == 0){
    close(pipes->tab[i]->descriptor[1]);
  }
  else if(i == pipes->nbpipe){
    close(pipes->tab[i -1]->descriptor[0]);
  }
  else{
    close(pipes->tab[i -1]->descriptor[0]);
    close(pipes->tab[i]->descriptor[1]);
  }
}