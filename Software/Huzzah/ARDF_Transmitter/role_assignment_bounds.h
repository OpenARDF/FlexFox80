/*
 * Dependency-free bounds for the ESP Event "role:slot" assignment format.
 * Validation intentionally matches the legacy Event boundary: a non-empty role
 * and a colon are required, while slot contents remain the caller's concern.
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
