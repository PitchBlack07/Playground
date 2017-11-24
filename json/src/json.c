#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
//struct
//{
//	const char* source;
//	const char* current;
//	int         line;
//	int         pos;
//} tokenizer;
//
//enum JSON_TOKEN_TYPE
//{
//	JSON_TOKEN_TYPE_UNKNOWN,
//	JSON_TOKEN_TYPE_OBJECT_BEGIN,
//	JSON_TOKEN_TYPE_OBJECT_END,
//	JSON_TOKEN_TYPE_ARRAY_BEGIN,
//	JSON_TOKEN_TYPE_ARRAY_END,
//	JSON_TOKEN_TYPE_SEPARATOR,
//	JSON_TOKEN_TYPE_COMMA,
//	JSON_TOKEN_TYPE_STRING,
//	JSON_TOKEN_TYPE_NUMBER,
//	JSON_TOKEN_TYPE_NULL,
//	JSON_TOKEN_TYPE_FALSE,
//	JSON_TOKEN_TYPE_TRUE,
//	JSON_TOKEN_TYPE_EOF
//};
//
//struct json_token
//{
//	const char*          source;
//
//	enum JSON_TOKEN_TYPE type;
//	int                  line;
//	int                  pos;
//	int                  len;
//};
//
//static void tokenizer_init(const char* str_)
//{
//	memset(&tokenizer, 0, sizeof(tokenizer));
//	tokenizer.current = str_;
//	tokenizer.source  = str_;
//}
//
//static void tokenizer_advance_line()
//{
//	tokenizer.current++;
//	tokenizer.line++;
//
//	tokenizer.pos = 0;
//}
//
//static void tokenizer_advance_pos()
//{
//	tokenizer.current++;
//	tokenizer.pos++;
//}
//
//static int make_simple_token(struct json_token* token_, enum JSON_TOKEN_TYPE type_)
//{
//	token_->source = tokenizer.current;
//	token_->len    = 1;
//	token_->line   = tokenizer.line;
//	token_->pos    = tokenizer.pos;
//	token_->type   = type_;
//
//	tokenizer_advance_pos();
//
//	return 1;
//}
//
//static int is_whitespace(char c_)
//{
//	return c_ == ' ' || c_ == '\n' || c_ == '\t' || c_ == '\r';
//}
//
//static int is_letter(char c_)
//{
//	return ('A' <= c_ && c_ <= 'Z') || ('a' <= c_ && c_ <= 'z');
//}
//
//static int make_number_token(struct json_token* token_)
//{
//	const char* start = tokenizer.current;
//	token_->source    = tokenizer.current;
//	token_->line      = tokenizer.line;
//	token_->pos       = tokenizer.pos;
//	token_->type      = JSON_TOKEN_TYPE_NUMBER;
//
//	if (*tokenizer.current == '-') {
//		tokenizer_advance_pos();
//	}
//
//	if (*tokenizer.current == '0') {
//		tokenizer_advance_pos();
//		if (*tokenizer.current == '.') {
//			tokenizer_advance_pos();
//			goto read_fraction;
//		}
//	}
//
//	if (*tokenizer.current < '1' || *tokenizer.current > '9')
//	{
//		// error
//		token_->type = JSON_TOKEN_TYPE_UNKNOWN;
//		return 0;
//	}
//
//	do {
//		tokenizer_advance_pos();
//	} while (*tokenizer.current >= '0' && *tokenizer.current <= '9');
//
//	if (*tokenizer.current == '.') {
//		tokenizer_advance_pos();
//		goto read_fraction;
//	}
//	else if (*tokenizer.current == 'e' || *tokenizer.current == 'E') {
//		tokenizer_advance_pos();
//		goto read_exponent;
//	}
//	else {
//		token_->len = tokenizer.current - token_->source;
//		return token_->len;
//	}
//
//read_fraction:
//	while (*tokenizer.current && *tokenizer.current >= '0' && *tokenizer.current <= '9')
//		tokenizer_advance_pos();
//
//	if (tokenizer.current == 0) {
//		token_->type = JSON_TOKEN_TYPE_EOF;
//		return 0;
//	}
//
//
//
//read_exponent:
//}
//
//int get_next_token(struct json_token* token_)
//{
//	while (1)
//	{
//		switch (*tokenizer.current)
//		{
//		case '{': return make_simple_token(token_, JSON_TOKEN_TYPE_OBJECT_BEGIN);
//		case '}': return make_simple_token(token_, JSON_TOKEN_TYPE_OBJECT_END);
//		case '[': return make_simple_token(token_, JSON_TOKEN_TYPE_ARRAY_BEGIN);
//		case ']': return make_simple_token(token_, JSON_TOKEN_TYPE_ARRAY_END);
//		case ':': return make_simple_token(token_, JSON_TOKEN_TYPE_SEPARATOR);
//		case ',': return make_simple_token(token_, JSON_TOKEN_TYPE_COMMA);
//		case '"':
//		{
//			token_->pos = tokenizer.pos + 1;
//			token_->source = tokenizer.current + 1;
//			token_->line = tokenizer.line;
//
//			do {
//				do {
//					tokenizer_advance_pos();
//				} while (*tokenizer.current && *tokenizer.current != '"');
//			} while (*tokenizer.current && *(tokenizer.current - 1) == '\\');
//
//			if (*tokenizer.current) {
//				token_->len = tokenizer.current - token_->source;
//				token_->type = JSON_TOKEN_TYPE_STRING;
//
//				tokenizer_advance_pos();
//				return token_->len;
//			}
//			else {
//				token_->type = JSON_TOKEN_TYPE_EOF;
//				return 0;
//			}
//		}
//
//		case 'n':
//			token_->source = tokenizer.current;
//			token_->pos    = tokenizer.pos;
//			token_->line   = tokenizer.line;
//			token_->len    = 4;
//			token_->type   = JSON_TOKEN_TYPE_NULL;
//
//			for (int i = 0; i < 4 && *tokenizer.current == "null"[i]; ++i) 
//				tokenizer_advance_pos();
//
//			if (!is_letter(*tokenizer.current)) {
//				return 4;
//			}
//			else {
//				token_->type = JSON_TOKEN_TYPE_UNKNOWN;
//				return 0;
//			}
//
//		case 'f':
//			token_->source = tokenizer.current;
//			token_->pos    = tokenizer.pos;
//			token_->line   = tokenizer.line;
//			token_->len    = 5;
//			token_->type   = JSON_TOKEN_TYPE_NULL;
//
//			for (int i = 0; i < 5 && *tokenizer.current == "false"[i]; ++i) 
//				tokenizer_advance_pos();
//
//			if (!is_letter(*tokenizer.current)) {
//				return 5;
//			}
//			else {
//				token_->type = JSON_TOKEN_TYPE_UNKNOWN;
//				return 0;
//			}
//
//		case 't':
//			token_->source = tokenizer.current;
//			token_->pos    = tokenizer.pos;
//			token_->line   = tokenizer.line;
//			token_->len    = 4;
//			token_->type   = JSON_TOKEN_TYPE_NULL;
//
//			for (int i = 0; i < 4 && *tokenizer.current == "true"[i]; ++i) 
//				tokenizer_advance_pos();
//
//			if (!is_letter(*tokenizer.current)) {
//				return 4;
//			}
//			else {
//				token_->type = JSON_TOKEN_TYPE_UNKNOWN;
//				return 0;
//			}
//
//		case '-':
//		case '0':
//		case '1':
//		case '2':
//		case '3':
//		case '4':
//		case '5':
//		case '6':
//		case '7':
//		case '8':
//		case '9':
//			return make_number_token(token_);
//
//		case '\t':
//		case ' ':  
//			tokenizer_advance_pos();   continue;
//
//		case '\n': tokenizer_advance_line(); continue;
//		case '\r': 
//			tokenizer_advance_line();
//
//			if (*tokenizer.current == '\n') 
//				tokenizer.current++;
//
//			continue;
//
//		case 0:
//			token_->pos  = tokenizer.pos;
//			token_->line = tokenizer.line;
//			token_->type = JSON_TOKEN_TYPE_EOF;
//			return 1;
//
//		default:
//			return 0;
//		}
//	}
//}
//
//struct json_object* json_parse(const char* str_)
//{
//	struct json_token t;
//	tokenizer_init(str_);
//
//	while (get_next_token(&t))
//	{
//		fprintf(stderr, "Token read");
//		int stop = 10;
//	}
//	return NULL;
//}

void jtokenizer_init(const char* str_, struct jtokenizer* tokenizer_)
{
	tokenizer_->buffer = str_;
	tokenizer_->column = 0;
	tokenizer_->line   = 0;
}

static int make_simple_token2(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_, enum jtoken token_)
{
	tokenOut_->token  = token_;
	tokenOut_->length = 1;
	tokenOut_->value  = tokenizer_->buffer++;

	return 0;
}

static int parse_keyword_false(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_)
{
	assert(*tokenizer_->buffer == 'f');

	if (*(++tokenizer_->buffer) != 'a') return -1;
	if (*(++tokenizer_->buffer) != 'l') return -1;
	if (*(++tokenizer_->buffer) != 's') return -1;
	if (*(++tokenizer_->buffer) != 'e') return -1;

	tokenOut_->length = 5;
	tokenOut_->value  = (++tokenizer_->buffer) - 5;
	tokenOut_->token  = JTOKEN_FALSE;

	return 0;
}

static int parse_keyword_true(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_)
{
	assert(*tokenizer_->buffer == 't');

	if (*(++tokenizer_->buffer) != 'r') return -1;
	if (*(++tokenizer_->buffer) != 'u') return -1;
	if (*(++tokenizer_->buffer) != 'e') return -1;

	tokenOut_->length = 4;
	tokenOut_->value = (++tokenizer_->buffer) - 4;
	tokenOut_->token = JTOKEN_TRUE;

	return 0;
}

static int parse_keyword_null(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_)
{
	assert(*tokenizer_->buffer == 'n');

	if (*(++tokenizer_->buffer) != 'u') return -1;
	if (*(++tokenizer_->buffer) != 'l') return -1;
	if (*(++tokenizer_->buffer) != 'l') return -1;

	tokenOut_->length = 4;
	tokenOut_->value = (++tokenizer_->buffer) - 4;
	tokenOut_->token = JTOKEN_NULL;

	return 0;
}

static void skip_whitespace(struct jtokenizer* tokenizer_)
{
	char c;
	do {
		c = *(++tokenizer_->buffer);
	} while (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

static int parse_string(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_)
{
	const char* buffer = tokenizer_->buffer;
	assert(*buffer == '\"');

	do {
		do {
			buffer++;
		} while (*buffer != '\"');
	} while (*(buffer - 1) == '\\');

	tokenOut_->length  = buffer - tokenizer_->buffer - 1;
	tokenOut_->token   = JTOKEN_STRING;
	tokenOut_->value   = tokenizer_->buffer + 1;
	tokenizer_->buffer = buffer + 1;
	
	return 0;
}

static int parse_number(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_)
{
	const char* c     = tokenizer_->buffer;
	const char* begin = c;

	if (*c == '-') c++;

	// leading zero only for fractionals
	if (*c == '0') {
		c++;
		if (*c == '.') {
			goto parse_fractional;
		}
		else {
			return -1;
		}
	}

	if (*c >= '1' && *c <= '9') {
		do {
			c++;
		} while (*c >= '0' && *c <= '9');
	}

	switch (*c)
	{
	case '.': goto parse_fractional;
	case 'e': goto parse_exponent;
	case 'E': goto parse_exponent;

	default:
		tokenOut_->length  = c - begin;
		tokenOut_->token   = JTOKEN_NUMBER;
		tokenOut_->value   = begin;
		tokenizer_->buffer = c;

		return 0;
	}

parse_fractional:
	do
	{
		c++;
	} while (*c >= '0' && *c <= '9');

	if (*c != 'e' && *c != 'E') {
		tokenOut_->length  = c - begin;
		tokenOut_->token   = JTOKEN_NUMBER;
		tokenOut_->value   = begin;
		tokenizer_->buffer = c;

		return 0;
	}

parse_exponent:
	if (*(++c) == '+' || *c == '-') {
		c++;
	}

	if (*c >= '0' && *c <= '9') {

		do {
			c++;
		} while (*c >= '0' && *c <= '9');

		tokenOut_->length  = c - begin;
		tokenOut_->token   = JTOKEN_NUMBER;
		tokenOut_->value   = begin;
		tokenizer_->buffer = c;

		return 0;
	}

	return -1;
}

int get_next_jtoken_desc(struct jtokenizer* tokenizer_, struct jtoken_desc* tokenOut_)
{
	while (1) {
		switch (*tokenizer_->buffer)
		{
		case '{': return make_simple_token2(tokenizer_, tokenOut_, JTOKEN_OBJECT_BEGIN);
		case '}': return make_simple_token2(tokenizer_, tokenOut_, JTOKEN_OBJECT_END);
		case ':': return make_simple_token2(tokenizer_, tokenOut_, JTOKEN_COLUMN);
		case ',': return make_simple_token2(tokenizer_, tokenOut_, JTOKEN_COMMA);
		case '[': return make_simple_token2(tokenizer_, tokenOut_, JTOKEN_ARRAY_BEGIN);
		case ']': return make_simple_token2(tokenizer_, tokenOut_, JTOKEN_ARRAY_END);
			
			// Whitespace
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			skip_whitespace(tokenizer_);
			continue;

		case 'f': return parse_keyword_false(tokenizer_, tokenOut_);
		case 't': return parse_keyword_true(tokenizer_, tokenOut_);
		case 'n': return parse_keyword_null(tokenizer_, tokenOut_);
		case '\"': return parse_string(tokenizer_, tokenOut_);

		case '-':
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
			return parse_number(tokenizer_, tokenOut_);

		default:
			break;
		}

		return -1;
	}

	return -1;
}

