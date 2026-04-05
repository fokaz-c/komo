#include "event_queue.h"
#include "fsm_types.h"
#include <stdbool.h>

FSM_Event_Queue *eq_init(FSM_Event_Queue *eq) {
	eq->count     = 0;
	eq->read_idx  = 0;
	eq->write_idx = 0;
	return eq;
}

bool eq_push(FSM_Event_Queue *eq, const FSM_Event *e) {
	if (eq->count == EVENT_QUEUE_SIZE) {
		return false;
	}

	eq->event[eq->write_idx] = *e;
	eq->write_idx            = (eq->write_idx + 1) & (EVENT_QUEUE_SIZE - 1);
	eq->count++;

	return true;
}

const FSM_Event *eq_pop(FSM_Event_Queue *eq) {
	if (eq->count == 0) {
		return NULL;
	}

	const FSM_Event *e = &eq->event[eq->read_idx];
	eq->read_idx       = (eq->read_idx + 1) & (EVENT_QUEUE_SIZE - 1);
	eq->count--;

	return e;
}
