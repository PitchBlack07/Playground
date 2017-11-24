#pragma once
#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	GLTF_UNKNOWN,
	GLTF_ERROR,
	GLTF_OBJECT_BEGIN,
	GLTF_OBJECT_END,
	GLTF_ARRAY_BEGIN,
	GLTF_ARRAY_END,
	GLTF_STRING,
	GLTF_NUMBER,
	GLTF_COMMA,
	GLTF_COLUMN,
	GLTF_EOF
} GLTF_TOKEN_TYPE;

typedef struct
{
	GLTF_TOKEN_TYPE type;
	const char*     begin;
	const char*     end;
} gltf_token;

bool gltf_tokenizer_begin(const char* buffer_);
gltf_token gltf_get_next_token();
void gltf_tokenizer_end();

#endif