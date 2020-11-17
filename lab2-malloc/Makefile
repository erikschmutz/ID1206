make_folder:
	@mkdir -p bin
	@mkdir -p assets

compile: make_folder
	@gcc src/bench.c src/dalloc.c -o bin/bench.o
	@gcc src/view.c src/dalloc.c -o bin/view.o

start: compile
	@./bin/dalloc.o

view: compile
	@./bin/view.o

bench: compile
	
	@echo "Single value" > assets/bench-no-merge-malloc.txt			
	@./bin/bench.o 100 8 8 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 1000 8 8 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 10000 8 8 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 100000 8 8 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 1000000 8 8 >> assets/bench-no-merge-malloc.txt		

	@echo "" >> assets/bench-no-merge-malloc.txt		
	@echo "Multiple values" >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 100 8 256 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 1000 8 256 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 10000 8 256 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 100000 8 256 >> assets/bench-no-merge-malloc.txt		
	@./bin/bench.o 1000000 8 256 >> assets/bench-no-merge-malloc.txt		