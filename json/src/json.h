#pragma once

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