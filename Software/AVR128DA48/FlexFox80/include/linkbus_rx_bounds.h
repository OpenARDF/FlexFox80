/*
 *  MIT License
 *
 *  Copyright (c) 2026 DigitalConfections
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
 * Dependency-free Linkbus receive-field bounds shared with host tests.
 */

#ifndef LINKBUS_RX_BOUNDS_H_
#define LINKBUS_RX_BOUNDS_H_

#include <stdint.h>

inline bool linkbus_rx_id_can_append(uint8_t id_length, uint8_t maximum_id_length)
{
	return id_length < maximum_id_length;
}

inline uint32_t linkbus_rx_id_append(uint32_t id, uint8_t character)
{
	return (id << 8) | character;
}

inline bool linkbus_rx_field_can_terminate(
	uint8_t field_index,
	uint8_t field_length,
	uint8_t maximum_fields,
	uint8_t field_capacity)
{
	return field_index > 0 && field_index <= maximum_fields &&
		field_capacity > 0 && field_length < field_capacity;
}

inline bool linkbus_rx_can_start_next_field(uint8_t field_index, uint8_t maximum_fields)
{
	return field_index < maximum_fields;
}

inline bool linkbus_rx_field_can_append(
	uint8_t field_index,
	uint8_t field_length,
	uint8_t maximum_fields,
	uint8_t field_capacity)
{
	return field_index > 0 && field_index <= maximum_fields &&
		field_capacity > 0 && field_length < (field_capacity - 1);
}

#endif /* LINKBUS_RX_BOUNDS_H_ */
