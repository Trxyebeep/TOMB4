#include "../tomb4/pch.h"
#include "function_stubs.h"

void other_log(char* format, ...)
{
	va_list arglist;
	char buffer[4096];

	va_start(arglist, format);
	vsprintf(buffer, format, arglist);
	Log(0, buffer);
}

void S_ExitSystem(const char* string)
{
	Log(0, "**** %s ****", string);
}

long GetRandomControl()
{
	rand_1 = 0x41C64E6D * rand_1 + 12345;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

long GetRandomDraw()
{
	rand_2 = 0x41C64E6D * rand_2 + 12345;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed)
{
	rand_2 = seed;
}

void init_game_malloc()
{
	malloc_buffer = (char*)MALLOC(MALLOC_SIZE);
	malloc_size = MALLOC_SIZE;
	malloc_ptr = malloc_buffer;
	malloc_free = MALLOC_SIZE;
	malloc_used = 0;
}

void* game_malloc(long size)
{
	char* ptr;

	size = (size + 3) & -4;

	if (size > malloc_free)
	{
		Log(0, "OUT OF MEMORY");
		return 0;
	}
	else
	{
		ptr = malloc_ptr;
		malloc_free -= size;
		malloc_used += size;
		malloc_ptr += size;
		memset(ptr, 0, size);
		return ptr;
	}
}

void inject_funcstubs(bool replace)
{
	INJECT(0x00490E80, other_log, replace);
	INJECT(0x00490EC0, S_ExitSystem, replace);
	INJECT(0x00490EE0, GetRandomControl, replace);
	INJECT(0x00490F20, SeedRandomControl, replace);
	INJECT(0x00490F00, GetRandomDraw, replace);
	INJECT(0x00490F30, SeedRandomDraw, replace);
	INJECT(0x00490F40, init_game_malloc, replace);
	INJECT(0x00490F80, game_malloc, replace);
}
