#ifndef TOKENIZER_FILE
#define TOKENIZER_FILE

int line_number;

//Tells the tokenizer what file to tokenize
int set_input_stream(char *filename);
//**token should be a pointer to a NULL token
//next_token will allocate space for the token string
//Returns the length of the token
//Returns 0 if there was an error with malloc
int next_token(char **token);
void reset_tokenizer();

#endif
