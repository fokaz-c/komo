#include "fsm_debug.h"

#ifdef FSM_DEBUG

#include <stddef.h>

static const FSM_NameEntry *state_entries;
static size_t               state_count;

static const FSM_NameEntry *event_entries;
static size_t               event_count;

void fsm_debug_register_states(const FSM_NameEntry *entries, size_t count) {
	state_entries = entries;
	state_count   = count;
}

void fsm_debug_register_events(const FSM_NameEntry *entries, size_t count) {
	event_entries = entries;
	event_count   = count;
}

const char *fsm_debug_state_name(FSM_State state) {
	for (size_t i = 0; i < state_count; i++) {
		if (state_entries[i].id == state)
			return state_entries[i].name;
	}
	return "Unknown";
}

const char *fsm_debug_event_name(FSM_EventType event) {
	for (size_t i = 0; i < event_count; i++) {
		if (event_entries[i].id == event)
			return event_entries[i].name;
	}
	return "Unknown";
}

#endif
