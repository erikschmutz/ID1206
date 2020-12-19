make_folder:
	@mkdir -p bin
	@mkdir -p assets

compile: make_folder
	@gcc src/green.c src/test.c -Wno-deprecated -o bin/test.o

start: compile
	@./bin/green.o
	@echo ""
	
test: compile
	@./bin/test.o
	@echo ""
	
