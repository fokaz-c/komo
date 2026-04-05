#pragma once

#include "fsm_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	FSM_State     from;
	FSM_EventType event;
	FSM_Guard     guard;
	FSM_Action    action;
	FSM_State     to;
} FSM_Transition;

typedef struct {
	FSM_State  state;
	FSM_Action on_entry;
	FSM_Action on_exit;
	FSM_Action on_update;
} FSM_StateHook;

typedef struct {
	FSM_State             current;
	const FSM_Transition *transitions;
	size_t                transition_count;
	const FSM_StateHook  *hooks;
	size_t                hook_count;
	void                 *ctx;
} FSM_Handle;

void      fsm_init(FSM_Handle *handle, FSM_State initial_state,
                   const FSM_Transition *transitions, size_t transitons_count,
                   const FSM_StateHook *hooks, size_t hooks_count, void *ctx);
bool      fsm_dispatch(FSM_Handle *handle, const FSM_Event *event);
FSM_State fsm_state(const FSM_Handle *handle);
void      fsm_update(FSM_Handle *handle);
