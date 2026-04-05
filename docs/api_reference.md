# API Reference

This document covers every public type, struct, and function in fsm-engine.
For a working example, see `examples/alarm/`.

---

## types.h

The primitives everything else builds on. Every other header includes this.

### FSM_State

```c
typedef uint32_t FSM_State;
```

An integer label for a state. You define your states as an enum or a set of
`#define` constants and cast them to `FSM_State`. The engine never interprets
the value, it only compares them for equality.

### FSM_EventType

```c
typedef uint32_t FSM_EventType;
```

Same idea as `FSM_State` but for events. Define your event kinds as an enum
and the engine will match them against transition rows.

### FSM_Event

```c
typedef struct {
    FSM_EventType type;
    uint32_t      data;
} FSM_Event;
```

The thing that gets passed around at runtime. `type` is which event happened.
`data` is optional payload -- a button index, a sensor value, whatever the
producer wants to attach. If you don't need it, ignore it.

### FSM_Guard

```c
typedef int (*FSM_Guard)(const FSM_Event *event, void *ctx);
```

A function pointer for conditional transitions. Return 1 to allow the
transition, 0 to block it. The engine calls this before doing anything else.
If you set a transition's guard to NULL, the transition is always allowed.

### FSM_Action

```c
typedef void (*FSM_Action)(const FSM_Event *event, void *ctx);
```

A function pointer for side effects. Called when a transition fires, after the
state has been updated. This is where you do real work -- toggle outputs, update
counters, write to registers. The `ctx` pointer is whatever your application
passed to `fsm_init`.

---

## fsm.h

### FSM_Transition

```c
typedef struct {
    FSM_State     from;
    FSM_EventType event;
    FSM_Guard     guard;
    FSM_Action    action;
    FSM_State     to;
} FSM_Transition;
```

One row in the transition table. Reads left to right: from this state, when
this event arrives, if this guard passes, do this action, go to this state.

`guard` and `action` can both be NULL. A NULL guard means always allowed. A
NULL action means no side effect.

You define your FSM as a static array of these and pass it to `fsm_init`.
The engine never modifies the table.

### FSM_StateHook

```c
typedef struct {
    FSM_State  state;
    FSM_Action on_entry;
    FSM_Action on_exit;
    FSM_Action on_update;
} FSM_StateHook;
```

Per-state callbacks. `on_entry` fires every time the FSM enters this state,
regardless of which transition brought it here. `on_exit` fires every time
it leaves, regardless of where it's going. `on_update` fires when you call
`fsm_update` and the FSM is currently in this state.

Use hooks for behavior that belongs to being in a state, not to a specific
transition. Starting a timer on entry, stopping a buzzer on exit.

All three can be NULL.

### FSM_Handle

```c
typedef struct {
    FSM_State             current;
    const FSM_Transition *transitions;
    size_t                transition_count;
    const FSM_StateHook  *hooks;
    size_t                hook_count;
    void                 *ctx;
} FSM_Handle;
```

The runtime handle. Keep one of these alive for the lifetime of your FSM.
Declare it on the stack or as a static -- never heap allocate it.

You do not fill this yourself. Pass it to `fsm_init` and let the engine
populate it.

### fsm_init

```c
void fsm_init(FSM_Handle *handle, FSM_State initial_state,
              const FSM_Transition *transitions, size_t transition_count,
              const FSM_StateHook *hooks, size_t hook_count,
              void *ctx);
```

Initializes the handle. Call this once before anything else. `initial_state`
is where the FSM starts. `hooks` and `hook_count` can be NULL and 0 if you
have no per-state callbacks.

`ctx` is an opaque pointer the engine passes through to every guard, action,
and hook. Cast it to your application state struct inside those functions.

### fsm_dispatch

```c
bool fsm_dispatch(FSM_Handle *handle, const FSM_Event *event);
```

The engine. Walks the transition table looking for a row that matches the
current state and event type. If it finds one and the guard passes, it fires
the transition in this order: on_exit, state update, action, on_entry. Returns
true if a transition fired, false if nothing matched.

If no transition matches, the event is silently ignored. This is intentional.

### fsm_state

```c
FSM_State fsm_state(const FSM_Handle *handle);
```

Returns the current state. Read-only, does not modify the handle.

### fsm_update

```c
void fsm_update(FSM_Handle *handle);
```

Calls `on_update` for the current state if one is registered. Call this on a
periodic tick for states that need continuous behavior -- polling a sensor,
updating a display, running a PID loop.

---

## event_queue.h

A fixed-size ring buffer for decoupling event producers from the dispatch loop.
Producers push events in, one consumer pops them out and calls `fsm_dispatch`.

### EVENT_QUEUE_SIZE

```c
#define EVENT_QUEUE_SIZE 32
```

Capacity of the queue. Must be a power of 2. Enforced at compile time with
`_Static_assert`. Change this if you need more or fewer slots.

### FSM_Event_Queue

```c
typedef struct {
    FSM_Event event[EVENT_QUEUE_SIZE];
    size_t    read_idx;
    size_t    write_idx;
    size_t    count;
} FSM_Event_Queue;
```

The queue struct. Declare it on the stack or as a static. Do not touch the
fields directly, use the functions below.

### eq_init

```c
FSM_Event_Queue *eq_init(FSM_Event_Queue *eq);
```

Zeroes the queue. Call once before use. Returns the pointer passed in so you
can chain it if needed.

### eq_push

```c
bool eq_push(FSM_Event_Queue *eq, const FSM_Event *e);
```

Copies the event into the queue. Returns false if the queue is full, true
otherwise. Non-blocking.

### eq_pop

```c
const FSM_Event *eq_pop(FSM_Event_Queue *eq);
```

Returns a pointer to the next event in the queue, or NULL if empty. The pointer
points directly into the queue buffer. It is valid until the next `eq_push`
overwrites that slot. Consume it immediately, do not store the pointer.

**Porting note:** `event_queue` is not ISR-safe. On STM32, if you push from an
ISR and pop from a task, wrap push and pop with `__disable_irq()` and
`__enable_irq()`, or replace with a lock-free SPSC queue. The interface stays
the same either way.

---

## fsm_debug.h

Optional name registry for human-readable transition logs. Enable by defining
`FSM_DEBUG` at compile time. When not defined, every call in this module
compiles to nothing.

```
cmake -DCMAKE_C_FLAGS="-DFSM_DEBUG" ..
```

### FSM_NameEntry

```c
typedef struct {
    uint32_t    id;
    const char *name;
} FSM_NameEntry;
```

A single id-to-name mapping. Build a static array of these for your states and
another for your events, then pass them to the register functions below.

### fsm_debug_register_states

```c
void fsm_debug_register_states(const FSM_NameEntry *entries, size_t count);
```

Registers state names. Call once at startup before any dispatch.

### fsm_debug_register_events

```c
void fsm_debug_register_events(const FSM_NameEntry *entries, size_t count);
```

Registers event names. Call once at startup before any dispatch.

### fsm_debug_state_name

```c
const char *fsm_debug_state_name(FSM_State state);
```

Returns the name registered for this state, or "?" if not found.

### fsm_debug_event_name

```c
const char *fsm_debug_event_name(FSM_EventType event);
```

Returns the name registered for this event type, or "?" if not found.

When `FSM_DEBUG` is enabled, `fsm_dispatch` calls these automatically and
prints every transition in this format:

```
[IDLE] --BUTTON_PRESS--> [ALARMING]
```
