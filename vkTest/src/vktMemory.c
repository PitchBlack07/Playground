
#include <vktMemory.h>

#ifndef NDEBUG
#include <crtdbg.h>
#endif
#include <stdlib.h>

void* vktalloc(size_t size_, const char* file_, int line_)
{
#ifndef NDEBUG
	return _malloc_dbg(size_, _NORMAL_BLOCK, file_, line_);
#else
	return malloc(size_);
#endif
}

void vktfree(void* ptr_)
{
#ifndef NDEBUG
	_free_dbg(ptr_, _NORMAL_BLOCK);
#else
	free(ptr_, _NORMAL_BLOCK);
#endif
}