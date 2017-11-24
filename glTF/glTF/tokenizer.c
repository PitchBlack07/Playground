#include "tokenizer.h"
#include <string.h>
#include <malloc.h>

typedef enum
{
	GLTF_TOKEN_STATE_NONE,
	GLTF_TOKEN_STATE_COMMA,
	GLTF_TOKEN_STATE_COLUMN,
	GLTF_TOKEN_STATE_STRING,
	GLTF_TOKEN_STATE_STRING_ESCAPE,
	GLTF_TOKEN_STATE_DIGIT,
	GLTF_TOKEN_STATE_FRAC,
	GLTF_TOKEN_STATE_EXP,
	GLTF_TOKEN_STATE_FRAC_DIGIT,
	GLTF_TOKEN_STATE_EXP_DIGIT,
	GLTF_TOKEN_STATE_ERROR,
	GLTF_TOKEN_STATE_OK
} GLTF_TOKEN_STATE;

static const char* buffer_ptr   = NULL;
static GLTF_TOKEN_STATE tkState = GLTF_TOKEN_STATE_NONE;

bool gltf_tokenizer_begin(const char* buffer_)
{
	buffer_ptr = buffer_;
	return true;
}

void gltf_tokenizer_end()
{
	buffer_ptr = NULL;
	tkState    = GLTF_TOKEN_STATE_NONE;
}

gltf_token gltf_get_next_token()
{
	gltf_token r;
	r.type  = GLTF_UNKNOWN;
	r.begin = buffer_ptr;
	r.end   = NULL;

	GLTF_TOKEN_TYPE token = GLTF_UNKNOWN;

	do
	{
		char c = *buffer_ptr++;

		switch (c)
		{
		case '+':
		case '-':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE: tkState   = GLTF_TOKEN_STATE_DIGIT;     break;
			case GLTF_TOKEN_STATE_EXP: tkState    = GLTF_TOKEN_STATE_EXP_DIGIT; break;
			case GLTF_TOKEN_STATE_STRING: tkState = GLTF_TOKEN_STATE_STRING;    break;
			default: tkState = GLTF_TOKEN_STATE_ERROR; break;
			}
			break;

		case '.':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE:   tkState = GLTF_TOKEN_STATE_FRAC;   break;
			case GLTF_TOKEN_STATE_DIGIT:  tkState = GLTF_TOKEN_STATE_FRAC;   break;
			case GLTF_TOKEN_STATE_STRING: tkState = GLTF_TOKEN_STATE_STRING; break;
			default: tkState = GLTF_TOKEN_STATE_ERROR; break;
			}
			break;

		case ':':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE: token = GLTF_COLUMN; tkState = GLTF_TOKEN_STATE_OK;     break;
			case GLTF_TOKEN_STATE_STRING:                    tkState = GLTF_TOKEN_STATE_STRING; break;
			default: GLTF_TOKEN_STATE_ERROR; break;
			}
			break;

		case ',':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE: token = GLTF_COMMA; tkState = GLTF_TOKEN_STATE_OK;     break;
			case GLTF_TOKEN_STATE_STRING:                   tkState = GLTF_TOKEN_STATE_STRING; break;
			default: GLTF_TOKEN_STATE_ERROR; break;
			}
			break;

		case ' ':
		case '\t':
		case '\n':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_DIGIT:
			case GLTF_TOKEN_STATE_FRAC_DIGIT:
			case GLTF_TOKEN_STATE_EXP_DIGIT:
				token = GLTF_NUMBER;
				tkState = GLTF_TOKEN_STATE_OK;
				break;

			default:
				tkState = GLTF_TOKEN_STATE_NONE;
				break;
			}
			break;

		case '\0':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_DIGIT:
			case GLTF_TOKEN_STATE_FRAC_DIGIT:
			case GLTF_TOKEN_STATE_EXP_DIGIT:
				token   = GLTF_NUMBER;
				tkState = GLTF_TOKEN_STATE_OK;
				break;

			case GLTF_TOKEN_STATE_STRING:
				tkState = GLTF_TOKEN_STATE_ERROR;
				token   = GLTF_EOF;
				break;

			default:
				tkState = GLTF_TOKEN_STATE_OK;
				token   = GLTF_EOF;
				break;
			}
			break;
			
		case 0x5C:
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_STRING:
				tkState = GLTF_TOKEN_STATE_STRING_ESCAPE;
				break;

			default:
				tkState = GLTF_TOKEN_STATE_ERROR;
				break;
			}

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_DIGIT:      tkState = GLTF_TOKEN_STATE_DIGIT;      break;
			case GLTF_TOKEN_STATE_NONE:       tkState = GLTF_TOKEN_STATE_DIGIT;      break;
			case GLTF_TOKEN_STATE_FRAC:       tkState = GLTF_TOKEN_STATE_FRAC_DIGIT; break;
			case GLTF_TOKEN_STATE_FRAC_DIGIT: tkState = GLTF_TOKEN_STATE_FRAC_DIGIT; break;
			case GLTF_TOKEN_STATE_EXP:        tkState = GLTF_TOKEN_STATE_EXP_DIGIT;  break;
			case GLTF_TOKEN_STATE_EXP_DIGIT:  tkState = GLTF_TOKEN_STATE_EXP_DIGIT;  break;
			case GLTF_TOKEN_STATE_STRING:     tkState = GLTF_TOKEN_STATE_STRING;     break;
			default:                          tkState = GLTF_TOKEN_STATE_ERROR;      break;
			}
			break;

		case '{':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE: tkState   = GLTF_TOKEN_STATE_OK; token = GLTF_OBJECT_BEGIN; break;
			case GLTF_TOKEN_STATE_STRING: tkState = GLTF_TOKEN_STATE_STRING;                        break;
			default:                      tkState = GLTF_TOKEN_STATE_ERROR;                         break;
			}
			break;

		case '}':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE:   tkState = GLTF_TOKEN_STATE_OK; token = GLTF_OBJECT_END; break;
			case GLTF_TOKEN_STATE_STRING: tkState = GLTF_TOKEN_STATE_STRING;                      break;
			default:                      tkState = GLTF_TOKEN_STATE_ERROR;                       break;
			}
			break;

		case ']':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE: tkState   = GLTF_TOKEN_STATE_OK; token = GLTF_ARRAY_END; break;
			case GLTF_TOKEN_STATE_STRING: tkState = GLTF_TOKEN_STATE_STRING;                     break;
			default:                      tkState = GLTF_TOKEN_STATE_ERROR;                      break;
			}
			break;

		case '[':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_NONE: tkState   = GLTF_TOKEN_STATE_OK; token = GLTF_ARRAY_BEGIN; break;
			case GLTF_TOKEN_STATE_STRING: tkState = GLTF_TOKEN_STATE_STRING;                       break;
			default:                      tkState = GLTF_TOKEN_STATE_ERROR;                        break;
			}
			break;

		case '"':
			switch (tkState)
			{
			case GLTF_TOKEN_STATE_STRING:
				tkState = GLTF_TOKEN_STATE_OK;
				token = GLTF_STRING;
				break;

			case GLTF_TOKEN_STATE_NONE:
			case GLTF_TOKEN_STATE_STRING_ESCAPE:
				tkState = GLTF_TOKEN_STATE_STRING;
				break;

			default:
				tkState = GLTF_TOKEN_STATE_ERROR;
				break;
			}
		}

	} while (tkState != GLTF_TOKEN_STATE_OK && tkState != GLTF_TOKEN_STATE_ERROR);

	if (tkState == GLTF_TOKEN_STATE_OK)
	{
		r.type = token;
		r.end  = buffer_ptr;
	}
	else
	{
		r.type  = GLTF_ERROR;
		r.end   = NULL;
		r.begin = NULL;
	}

	return r;
}

