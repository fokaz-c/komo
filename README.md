# fsm-engine

A table-driven finite state machine engine in C, built for embedded systems.

---

## What it is

A small, generic C library for defining and running state machines as data rather than code. You describe your system's states, events, and transitions in a table. The engine runs them.

---

## What it solves
Embedded systems are full of states. It is always in one or another condition - IDLE, RUNNING, ERROR, etc. The standard way is to handle this is a switch-case statement. It works with a few states, but once the number of states increases it just becomes harder to follow.

This engine makes state logic explicit, auditable, and reusable. Every valid transition is a row in a table. You can read the table and immediately answer: *what does this system do when it's in state X and event Y arrives?* No tracing through code. No mental simulation.

---

## Who it's for

Firmware engineers working on devices with non-trivial control flow - menus, alarm sequences, communication protocols, dispense cycles, anything with more than a handful of states. Also useful as a learning project for anyone who wants to understand how FSMs are structured before writing them in a real RTOS environment.

---

## Philosophy

**Logic belongs in data, not code.**
When your FSM lives in a `switch` statement, it exists only in the programmer's head. When it lives in a table, it exists on paper. You can audit it, print it, and hand it to someone who has never seen the codebase.

**The engine should know nothing about your application.**
The library has no opinion about what your states mean or what your actions do. It only knows how to walk a table and call a function pointer. This is what makes it reusable, the engine is written once, your application is just data on top.

**Behavior should be guaranteed by structure, not discipline.**
If stopping the buzzer when leaving the alarm state is important, it should be impossible to forget, not dependent on every future developer remembering to do it. Entry and exit hooks enforce invariants at the engine level so transition actions only handle what is unique about that specific transition.

**Silence is a valid response.**
If an event arrives that the current state has no transition for, the engine ignores it. States are defined by what they respond to, not by exhaustively rejecting everything they don't care about.
