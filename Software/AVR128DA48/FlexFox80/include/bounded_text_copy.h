#ifndef BOUNDED_TEXT_COPY_H_
#define BOUNDED_TEXT_COPY_H_

#include <stddef.h>
#include <string.h>

inline bool copy_text_to_buffer(char* destination, size_t capacity, const char* text)
{
	if(!destination || !text || !capacity)
	{
		return false;
	}

	const size_t length = strlen(text);
	if(length >= capacity)
	{
		return false;
	}

	memcpy(destination, text, length + 1);
	return true;
}

#endif /* BOUNDED_TEXT_COPY_H_ */
