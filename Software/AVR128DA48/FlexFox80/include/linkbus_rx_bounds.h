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
