#ifndef EVENT_TIME_STATE_H_INCLUDED
#define EVENT_TIME_STATE_H_INCLUDED

#include <time.h>

void setEventStartEpoch(time_t value);
void setEventFinishEpoch(time_t value);
void setEventEpochs(time_t start, time_t finish);

#endif /* EVENT_TIME_STATE_H_INCLUDED */
