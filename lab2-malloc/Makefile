make_folder:
	@mkdir -p bin
	@mkdir -p bin1
	@mkdir -p bin2
	@mkdir -p assets


compile: make_folder
	@gcc src-no-merge/bench.c src-no-merge/dalloc.c -o bin/bench-no-merge.o
	@gcc src-no-merge/view.c src-no-merge/dalloc.c -o bin/view-no-merge.o 
	@gcc src-merge/bench.c src-merge/dalloc.c -o bin/bench-merge.o
	@gcc src-merge/view.c src-merge/dalloc.c -o bin/view-merge.o 
	@gcc src/bench.c src/dalloc.c -o bin/bench.o
	@gcc src/view.c src/dalloc.c -o bin/view.o 


start: compile
	@./bin/view.o
	
	  

bench: compile
	
	@echo "" > assets/bench.txt		
	@echo "Multiple values no merge" >> assets/bench.txt		
	@./bin/bench-no-merge.o 100 8 256 >> assets/bench.txt	
	@./bin/bench-no-merge.o 100 8 256 >> assets/bench.txt	
	@./bin/bench-no-merge.o 1000 8 256 >> assets/bench.txt		
	@./bin/bench-no-merge.o 10000 8 256 >> assets/bench.txt		
	@./bin/bench-no-merge.o 100000 8 256 >> assets/bench.txt		
	@./bin/bench-no-merge.o 1000000 8 256 >> assets/bench.txt	

	@echo "" >> assets/bench.txt		
	@echo "Multiple values merge" >> assets/bench.txt		
	@./bin/bench-merge.o 100 8 256 >> assets/bench.txt
	@./bin/bench-merge.o 100 8 256 >> assets/bench.txt			
	@./bin/bench-merge.o 1000 8 256 >> assets/bench.txt		
	@./bin/bench-merge.o 10000 8 256 >> assets/bench.txt		
	@./bin/bench-merge.o 100000 8 256 >> assets/bench.txt		
	@./bin/bench-merge.o 1000000 8 256 >> assets/bench.txt	

	@echo "" >> assets/bench.txt		
	@echo "Multiple values merge optimized" >> assets/bench.txt		
	@./bin/bench.o 100 8 256 >> assets/bench.txt	
	@./bin/bench.o 100 8 256 >> assets/bench.txt	
	@./bin/bench.o 1000 8 256 >> assets/bench.txt		
	@./bin/bench.o 10000 8 256 >> assets/bench.txt		
	@./bin/bench.o 100000 8 256 >> assets/bench.txt		
	@./bin/bench.o 1000000 8 256 >> assets/bench.txt				