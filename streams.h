#ifndef STREAMS_H
#define STREAMS_H

#include <stdlib.h>
#include <stdio.h>

#include "cmdline.h"

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


#endif

