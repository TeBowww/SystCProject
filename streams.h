#ifndef STREAMS_H
#define STREAMS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "cmdline.h"


struct pipe{
  int descriptor[2];
};

//structure that define an array of pipes use between the processus
struct pipe_tab{
  struct pipe **tab;
  size_t nbpipe;
};


/*
	Redirect standard input (stdin) to a file

	@param	input 	input stream used
	@param	li 		Structure line that contain file to redirect to
*/
void input_redirection(FILE *input, const struct line li);

/*
	Close a file attached to the input stream

	@param	input 	input stream used	
*/
void input_close(FILE *input);

/*
	Redirect standard output (stdout) to a file

	@param	output 	output stream used
	@param	li 		Structure line that contain file to redirect to
*/
void output_redirection(FILE *output, const struct line li);

/*
	Close a file attached to the output stream

	@param	output 	output stream used	
*/
void output_close(FILE *output);

/*
	Initialise the structure pipe_tab and the right number of struct pipe to use

	@param	pi 			Structure pipe_tab to initalize
	@param	ncmds 		number of commands to define the number of pipes to create
*/
void init_pipe_tab(struct pipe_tab *pi, size_t ncmds);

/*
	Free the memory dynamicly allocated during pipe_tab initialazation

	@param	pi 	Structure pipe_tab to free
*/
void pipe_tab_destroy(struct pipe_tab *pi);

/*
	Redirect stdin and stdout of son on the appropriate pipe and close the unused descriptor

	@param	li 		Struct line to get the number of the commands in process
	@param	pipes 	Struct pipe_tab to access all the pipes created
	@param	i 	 	Get the current process to redirect stdin and atdout
*/
void pipe_redirect_input(struct line *li, struct pipe_tab *pipes, size_t i);

/*
	close desciptors in the father's process

	@param	li 		Struct line to get the number of the commands in process
	@param	pipes 	Struct pipe_tab to access all the pipes created
	@param	i 	 	Get the current process to correctly close unused descriptors
*/
void pipe_father_cose_descriptor(struct line *li, struct pipe_tab *pipes, size_t i);


#endif

