make_folder:
	@mkdir -p bin


compile: make_folder
	@gcc src/dalloc.c -o bin/dalloc.o

start: compile
	@./bin/dalloc.o

