make_folder:
	@mkdir -p bin
	@mkdir -p assets

compile: make_folder
	@gcc src/green.c -Wno-deprecated -o bin/green.o 

start: compile
	@./bin/green.o
	@echo ""
	
