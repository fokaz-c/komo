#pragma once

#include "fsm_types.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint32_t    id;
	const char *name;
} FSM_NameEntry;

#ifdef FSM_DEBUG
void fsm_debug_register_states(const FSM_NameEntry *entries, size_t count);
void fsm_debug_register_events(const FSM_NameEntry *entries, size_t count);
const char *fsm_debug_state_name(FSM_State state);
const char *fsm_debug_event_name(FSM_EventType event);
#else
#define fsm_debug_register_states(e, c)
#define fsm_debug_register_events(e, c)
#define fsm_debug_state_name(s)  ""
#define fsm_debug_event_name(ev) ""
#endif
