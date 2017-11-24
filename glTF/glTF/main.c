#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

static const char* test0[] =
{
	"13",
	"+14",
	"-668",
	"+0.12",
	"-0.667",
	"0.555",
	"12.44e6",
	"12.e-7",
	"15.45E+8"
};

int main(int argc, char** argv)
{
	for (int i = 0; i < _countof(test0); ++i)
	{
		gltf_tokenizer_begin(test0[i]);
		gltf_token tk = gltf_get_next_token(test0[i]);

		tk = gltf_get_next_token(test0[i]);

		gltf_tokenizer_end();
	}
	return 0;
}