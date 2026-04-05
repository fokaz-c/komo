#pragma once

#include <stdint.h>

/* opaque integer label for a state */
typedef uint32_t FSM_State;

/* opaque integer label for an event kind */
typedef uint32_t FSM_EventType;

/* what happened + optional payload */
typedef struct {
	FSM_EventType type;
	uint32_t data;
} FSM_Event;

/* return 1 to allow transition, 0 to block */
typedef int (*FSM_Guard)(const FSM_Event *event, void *ctx);

/* side effect when a transition fires */
typedef void (*FSM_Action)(const FSM_Event *event, void *ctx);
