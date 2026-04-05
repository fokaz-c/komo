#include "fsm.h"
#include "fsm_types.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef FSM_DEBUG
#include "fsm_debug.h"
#include <stdio.h>
#endif
void fsm_init(FSM_Handle *handle, FSM_State initial_state,
              const FSM_Transition *transitions, size_t transition_count,
              const FSM_StateHook *hooks, size_t hooks_count, void *ctx) {
	handle->hooks            = hooks;
	handle->ctx              = ctx;
	handle->current          = initial_state;
	handle->transition_count = transition_count;
	handle->hook_count       = hooks_count;
	handle->transitions      = transitions;
}

FSM_State fsm_state(const FSM_Handle *handle) {
	return handle->current;
}

static const FSM_StateHook *find_hook(const FSM_Handle *handle,
                                      FSM_State         state) {
	for (size_t i = 0; i < handle->hook_count; i++) {
		if (handle->hooks[i].state == state)
			return &handle->hooks[i];
	}
	return NULL;
}

bool fsm_dispatch(FSM_Handle *handle, const FSM_Event *event) {
	const FSM_Transition *tb      = handle->transitions;
	size_t                cnt     = handle->transition_count;
	FSM_State             current = handle->current;
	FSM_EventType         e       = event->type;

	for (size_t i = 0; i < cnt; i++) {
		if (tb[i].from != current || tb[i].event != e)
			continue;
		if (tb[i].guard != NULL && !tb[i].guard(event, handle->ctx))
			continue;

		const FSM_StateHook *exit_hook = find_hook(handle, current);
		if (exit_hook != NULL && exit_hook->on_exit != NULL)
			exit_hook->on_exit(event, handle->ctx);

		handle->current = tb[i].to;

#ifdef FSM_DEBUG
		printf("[%s] --%s--> [%s]\n", fsm_debug_state_name(current),
		       fsm_debug_event_name(e),
		       fsm_debug_state_name(handle->current));
#endif

		if (tb[i].action != NULL)
			tb[i].action(event, handle->ctx);

		const FSM_StateHook *entry_hook =
		        find_hook(handle, handle->current);
		if (entry_hook != NULL && entry_hook->on_entry != NULL)
			entry_hook->on_entry(event, handle->ctx);

		return true;
	}

	return false;
}

void fsm_update(FSM_Handle *handle) {
	const FSM_StateHook *hook = find_hook(handle, handle->current);
	if (hook != NULL && hook->on_update != NULL) {
		hook->on_update(NULL, handle->ctx);
	}
}
