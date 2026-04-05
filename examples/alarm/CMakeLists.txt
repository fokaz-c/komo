add_executable(example_alarm main.c)

target_link_libraries(example_alarm PRIVATE fsm)

target_compile_options(example_alarm PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)
