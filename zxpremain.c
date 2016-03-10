#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>

#define ITR_LEN 256

void write_err(char * my_log){
	fprintf(stderr, "%s", my_log);
}

char* get_line_before_semicol(char* line, int * check_mark) {

	int quotation_counter = 0;
	for (int i = 0; i < line[i] != '\0'; ++i){
		if (line[i] == '"'){quotation_counter++;}
	}

	if (quotation_counter != 0 && quotation_counter != 2) {
		* check_mark = 1;
		write_err("Unclosed string.\n");
		return NULL;
	}

	int i = 0;
	int quoter = 0;
	bool is_empty = true;
	for (; line[i] != '\0'; ++i)
	{

		if (line[i] != ' ' && line[i] != '\n' && line[i] != ';')
		{
			is_empty = false;
		}else if (line[i] == '\n')
		{
			line[i] = ' ';
		}

		if (line[i] == ';' && (quoter == 0 || quoter == 2))
		{
			line[i] = '\0';
			break;
		}

		if (line[i] == '"')
		{
			quoter++;
		}
	}


	if (is_empty)
	{
		return NULL;
	}else{
		return line;
	}
}

char* get_label_instruction(char* line,  char * label) {

	if (line == NULL)
	{
		return NULL;
	}

	int colon_counter = 0;
	for (int i = 0; i < line[i] != '\0'; ++i){
		if (line[i] == ':'){colon_counter++;}
	}

	if (colon_counter == 0)
	{
		return line;
	}else if(colon_counter == 1){
		char * token;
		token = strtok (line, " :");

		int i = 0;
		for (; i < 8; ++i) {
			if(token[i] != '\0'){
				label[i] = token[i];
			}else{
				break;
			}
		}
		label[i] = ':'; i++;
		for (; i < 9; ++i) {
			label[i] = ' ';
		}

		label[9] = '\0';

		token = strtok (NULL,"");
		return token;
	}else{
		write_err("Double label.\n");
		exit(1);	
	}
}

char* get_mnemonic(char* line, char * instruction) {

	if (line == NULL)
	{
		return NULL;
	}

	char * token;
	token = strtok (line, " ");
	strcpy(instruction,token);

	token = strtok (NULL, "");

	bool is_empty = true;

	if (token == NULL)
	{
		return NULL;
	}

	for (int i = 0; token[i] != '\0'; ++i) {
		if (token[i] != ' ' && token[i] != '\n')
		{
			is_empty = false;
		}
	}
	if (is_empty)
	{
		return NULL;
	}else{
		return token;
	}
}

char* get_quotation(char* line,  char * instruction) {

	if (line == NULL)
	{
		return NULL;
	}

	int quotation_counter = 0;
	for (int i = 0; i < line[i] != '\0'; ++i){
		if (line[i] == '"'){quotation_counter++;}
	}

	if (quotation_counter == 0) {
		return line;
	}else if(quotation_counter == 2){
		char * token;
		token = strtok (line, "\"");

		int i = 0;
		for (; instruction[i] != '\0'; ++i){}
		instruction[i] = ' '; i++;
		instruction[i] = '"';

		int j = 0;
		for (; token[j] != '\0'; ++j) {
				instruction[i + j + 1] = token[j];
		}
		instruction[i + j + 1] = '"';instruction[i + j + 2] = '\0';

		return NULL;
	}else{
		write_err("ERROR: problem with quotation\n");
		exit(1);	
	}
}

char* get_operations(char* line,  char * instruction) {

	if (line == NULL)
	{
		return NULL;
	}

	char * token;
	token = strtok (line, " ");
	if (token != NULL)
	{
		strcat(instruction," ");
	}
	while(token != NULL){

		strcat(instruction,token);
		token = strtok (NULL, " ");
	}

	return NULL;
}

void print_output_line(char * label, char * instruction){
	if (label[0] == '\0' && instruction[0] == '\0')
	{
		// printf("-- empty --\n");
		return;
	}

	char out[1024] = {0};
	strcat(out,label);
	strcat(out,instruction);

	if (label[0] == '\0')
	{
		printf("         %s\n", out);
	}else{
		printf("%s\n", out);
	}
	
}

int preprocess_get_line ( char read_in_line [], char label [] , char instruction [] ){

	/************************************************
		process lines
	************************************************/
	int a = 0;
	int * check_mark = &a;

	// printf("00#%s#", read_in_line);

	// 1. remove comment, semicolom, and empty line
	read_in_line = get_line_before_semicol(read_in_line, check_mark);
	// printf("label#%s#  instruction#%s#\n", label, instruction);
	// printf("1 #%s#\n\n", read_in_line);

	// 2. detect label 
	read_in_line = get_label_instruction(read_in_line, label);
	// printf("label#%s#  instruction#%s#\n", label, instruction);
	// printf("2 #%s#\n\n", read_in_line);

	// 3. find mnemonic
	read_in_line = get_mnemonic(read_in_line, instruction);
	// printf("label#%s#  instruction#%s#\n", label, instruction);
	// printf("3 #%s#\n\n", read_in_line);

	// 4. find quotation
	read_in_line = get_quotation(read_in_line, instruction);
	// printf("label#%s#  instruction#%s#\n", label, instruction);
	// printf("4 #%s#\n\n", read_in_line);

	// 5. put rest segments together, one comma is also a segment
	read_in_line = get_operations(read_in_line, instruction);
	// printf("label#%s#  instruction#%s#\n", label, instruction);

	print_output_line(label, instruction);

	return * check_mark;
}


int main(int argc, char *argv[])
{

	/************************************************
		open file
	************************************************/
    int needs_closing = 0;
    
    FILE *fp;
    
    if (argc == 1)
    {
        fp = stdin;
        needs_closing = 0;
    }
    else
    {
        char *input_filename = argv[1];
        /* Only the open the file when necessary! */
        fp = fopen(input_filename, "r");
        /* Always check success of fopen! */
        if(fp == NULL)
        {
            fprintf(stderr, "Unable to open \"%s\"\n", input_filename);
            return 1;
        }
        /* We'll remember to close it later... */
        needs_closing = 1;
    }


	/************************************************
		pre-processing
	************************************************/
    char read_in_line[ITR_LEN];
    int return_val = 1;
    while(fgets(read_in_line, ITR_LEN, fp) != NULL)
    {
        char label[9] = {0};
        char instruction[ITR_LEN] = {0};
        return_val = preprocess_get_line(read_in_line, label, instruction);
		// return 0;
    }

	/************************************************
		close file
	************************************************/
    /* Remember to close (I often forget this in examples!) */
    if(needs_closing)
    {
        fclose(fp);
    }
    
    return return_val;
}











