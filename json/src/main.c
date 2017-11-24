#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "json.h"

char* load_json_file_from_cmd_line(int argc_, const char** cmdline_)
{
	FILE* file    = NULL;
	long int size = 0;
	char* content = NULL;

	if (argc_ < 2) {
		return NULL;
	}

	if (fopen_s(&file, cmdline_[1], "rb") != 0) {
		fprintf(stderr, "Error cannot open file %s\n", cmdline_[1]);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file); 
	fseek(file, 0, SEEK_SET);
	
	content = malloc(sizeof(char) * (size+1));
	if (content) {
		size_t r   = fread(content, sizeof(char), size, file);
		content[r] = 0;
	}
	else{
		fprintf(stderr, "Out of memory!\n");
	}

	fclose(file);

	return content;
}

int main(int argc, char** argv)
{
	const char* jText = NULL;

	if (argc < 2) {
		fprintf(stderr, "Usage: json <filename>\n");
		return 0;
	}

	jText = load_json_file_from_cmd_line(argc, argv);
	if (jText == NULL) {
		return -1;
	}

	struct jtokenizer tokenizer;
	jtokenizer_init(jText, &tokenizer);

	struct jtoken_desc jdesc;
	while (get_next_jtoken_desc(&tokenizer, &jdesc) == 0)
	{
		int stop = 10;
	}
	
	//json_parse(jText);

	free((void*)jText);
}