#pragma once

#include "fsm_types.h"
#include <stdbool.h>
#include <stddef.h>

#define EVENT_QUEUE_SIZE 32

typedef struct {
	FSM_Event event[EVENT_QUEUE_SIZE];
	size_t    read_idx;
	size_t    write_idx;
	size_t    count;
} FSM_Event_Queue;

FSM_Event_Queue *eq_init(FSM_Event_Queue *eq);
bool             eq_push(FSM_Event_Queue *eq, const FSM_Event *e);

/* returns pointer into the buffer, valid until the next eq_push. consume
 * immediately. */
const FSM_Event *eq_pop(FSM_Event_Queue *eq);

_Static_assert((EVENT_QUEUE_SIZE & (EVENT_QUEUE_SIZE - 1)) == 0,
               "EVENT_QUEUE_SIZE must be a power of 2");
