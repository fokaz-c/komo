#include "event_queue.h"
#include "fsm.h"
#include "fsm_debug.h"
#include <stdio.h>

typedef enum { IDLE, ALARMING, SNOOZED } State;
typedef enum { ALARM_TRIGGER, SNOOZE, DISMISS } Event;

typedef struct {
	int snooze_count;
} AlarmCtx;

static void on_alarm_entry(const FSM_Event *e, void *ctx) {
	(void)e;
	(void)ctx;
	printf("alarm started\n");
}

static void on_alarm_exit(const FSM_Event *e, void *ctx) {
	(void)e;
	(void)ctx;
	printf("alarm stopped\n");
}

static void action_snooze(const FSM_Event *e, void *ctx) {
	(void)e;
	AlarmCtx *a = ctx;
	a->snooze_count++;
	printf("snoozed %d time(s)\n", a->snooze_count);
}

static const FSM_Transition transitions[] = {
        {IDLE, ALARM_TRIGGER, NULL, NULL, ALARMING},
        {ALARMING, SNOOZE, NULL, action_snooze, SNOOZED},
        {ALARMING, DISMISS, NULL, NULL, IDLE},
        {SNOOZED, ALARM_TRIGGER, NULL, NULL, ALARMING},
        {SNOOZED, DISMISS, NULL, NULL, IDLE},
};

static const FSM_StateHook hooks[] = {
        {ALARMING, on_alarm_entry, on_alarm_exit, NULL},
};

#ifdef FSM_DEBUG
static const FSM_NameEntry state_names[] = {
        {IDLE, "IDLE"},
        {ALARMING, "ALARMING"},
        {SNOOZED, "SNOOZED"},
};
static const FSM_NameEntry event_names[] = {
        {ALARM_TRIGGER, "ALARM_TRIGGER"},
        {SNOOZE, "SNOOZE"},
        {DISMISS, "DISMISS"},
};
#endif

int main(void) {
	AlarmCtx        ctx = {.snooze_count = 0};
	FSM_Handle      handle;
	FSM_Event_Queue queue;

	fsm_init(&handle, IDLE, transitions,
	         sizeof(transitions) / sizeof(transitions[0]), hooks,
	         sizeof(hooks) / sizeof(hooks[0]), &ctx);
	eq_init(&queue);

#ifdef FSM_DEBUG
	fsm_debug_register_states(state_names,
	                          sizeof(state_names) / sizeof(state_names[0]));
	fsm_debug_register_events(event_names,
	                          sizeof(event_names) / sizeof(event_names[0]));
#endif

	FSM_Event ev;
	ev = (FSM_Event){ALARM_TRIGGER, 0};
	eq_push(&queue, &ev);
	ev = (FSM_Event){SNOOZE, 0};
	eq_push(&queue, &ev);
	ev = (FSM_Event){ALARM_TRIGGER, 0};
	eq_push(&queue, &ev);
	ev = (FSM_Event){DISMISS, 0};
	eq_push(&queue, &ev);

	const FSM_Event *next;
	while ((next = eq_pop(&queue)) != NULL)
		fsm_dispatch(&handle, next);

	return 0;
}
