#pragma once
#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

#include <stdint.h>

struct json_string
{
	char  buffer[32 - sizeof(char*) - sizeof(int)];
	char* string;
	int   size;
};

struct json_value
{
	void* tmp;
};

struct json_object
{
	struct json_string name;
	struct json_value  value;
};

struct json_object* json_parse(const char* str_);

enum jtoken
{
	JTOKEN_UNKNOWN,
	JTOKEN_OBJECT_BEGIN,
	JTOKEN_OBJECT_END,
	JTOKEN_COLUMN,
	JTOKEN_COMMA,
	JTOKEN_STRING,
	JTOKEN_NUMBER,
	JTOKEN_NULL,
	JTOKEN_FALSE,
	JTOKEN_TRUE,
	JTOKEN_ARRAY_BEGIN,
	JTOKEN_ARRAY_END
};

struct jtoken_desc
{
	enum jtoken token;
	const char* value;
	uint32_t    length;
};

struct jtokenizer
{
	const char*  buffer;
	int          line;
	int          column;
};

void jtokenizer_init(const char* str_, struct jtokenizer* tokenizer_);

int get_next_jtoken_desc(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_);

#endif