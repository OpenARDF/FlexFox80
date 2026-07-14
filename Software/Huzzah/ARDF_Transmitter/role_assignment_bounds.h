/*
 *  MIT License
 *
 *  Copyright (c) 2026 Digital Confections LLC
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*
 * Dependency-free parsing of ESP8266 event-role assignment prefixes.
 */

#ifndef ROLE_ASSIGNMENT_BOUNDS_H_INCLUDED
#define ROLE_ASSIGNMENT_BOUNDS_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
	size_t roleBegin;
	size_t roleEnd;
	size_t slotBegin;
} RoleAssignmentBounds;

static inline bool roleAssignmentBounds(const char *assignment, RoleAssignmentBounds *bounds)
{
	if((assignment == NULL) || (bounds == NULL))
	{
		return false;
	}

	for(size_t index = 0; assignment[index] != '\0'; index++)
	{
		if(assignment[index] == ':')
		{
			if(index == 0)
			{
				return false;
			}

			bounds->roleBegin = 0;
			bounds->roleEnd = index;
			bounds->slotBegin = index + 1;
			return true;
		}
	}

	return false;
}

#endif /* ROLE_ASSIGNMENT_BOUNDS_H_INCLUDED */
