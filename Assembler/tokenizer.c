#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"

#define BUFFER_SIZE 8000

static FILE *stream = NULL;
static char buffer[BUFFER_SIZE] ="";
static char *end, *start = buffer + BUFFER_SIZE;

static inline int is_blank(char c) {
	return (c == ' ' || c == '\t' || c == '\n' || c == ';' || c == '\0' || c == ',');
}

//Moves the last n characters in the buffer to the front
//and then reads in characters from the file into the buffer
size_t read_helper(int n) {
	char *s = buffer;
	if (n) {
		char *e = buffer + BUFFER_SIZE - n;
		for (int i = 0; i < n; s++, e++, i++) {
			*s = *e;
		}
	}
	start = buffer;
	end = buffer + n;
	return fread(s, BUFFER_SIZE - n, sizeof(char), stream);
}

int next_token(char **returnstr) {
	if (start - buffer == BUFFER_SIZE) {
		read_helper(0);
	}
	//Find the beginning of the next token
	while (is_blank(*start)) {
		if (*start == ';') {
			do {
				start++;
				if (start - buffer == BUFFER_SIZE) {
					//Find the beginning of the next line
					read_helper(0);
				}
			} while (*start != '\n');
			line_number++;
		} else if (*start == '\n') line_number++;
		start++;
		if (start - buffer == BUFFER_SIZE) {
			//Find the beginning of the next line
			read_helper(0);
		}
	}
	//Find the end of the token
	end = start + 1;
	int slashes = 0;
	if (*start == '\"') {
		do {
			if (*end == '\n') line_number++;
			else if (*end == '\\' && *(end-1) != '\\') slashes++;
			end++;
			if (end - buffer == BUFFER_SIZE) {
				read_helper(end - start);
			}
		} while (*end != '\"' || *(end-1) == '\\');
		end++;
	} else {
		do {
			end++;
			if (end - buffer == BUFFER_SIZE) {
				read_helper(end - start);
			}
		} while (!(is_blank(*end)));
	}
	//Length of the token
	size_t toklen = end - (start + slashes);
	//Copy the string the the buffer to the return string
	*returnstr = realloc(*returnstr, (toklen + 1) * sizeof(char));
	if (*returnstr == NULL) return 0;
	if (slashes) {
		for (int i = 0; i < toklen; i++, start++) {
			if (*start == '\\') {
				switch (*(++start)) {
					case 'n':
						(*returnstr)[i] = '\n';
						break;
					case 't':
						(*returnstr)[i] = '\t';
						break;
					case '\"':
						(*returnstr)[i] = '\"';
						break;
					case '\\':
						(*returnstr)[i] = '\\';
						break;
				}
			} else {
				(*returnstr)[i] = *start;
			}
		}
		(*returnstr)[toklen] = '\0';
	} else {
		strncpy(*returnstr, start, toklen);
		(*returnstr)[toklen] = '\0';
		start = end;
	}
	return strlen(*returnstr);
}

int set_input_stream(char *filename) {
	stream = fopen(filename, "rb");
	if (stream == NULL) return 0;
	line_number = 1;
	return 1;
}

void reset_tokenizer() {
	rewind(stream);
	line_number = 1;
	start = buffer + BUFFER_SIZE;
}
