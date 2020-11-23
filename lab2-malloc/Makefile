bench_single =                  \
		$(1) 100 8 4096 25 	> $(2);	\
		$(1) 1000 8 4096 25 >> $(2);	\
		$(1) 2000 8 4096 25 >> $(2);	\
		$(1) 3000 8 4096 25 >> $(2);	\
		$(1) 4000 8 4096 25 >> $(2);	\
		$(1) 5000 8 4096 25 >> $(2);	\
		$(1) 6000 8 4096 25 >> $(2);	\
		$(1) 7000 8 4096 25 >> $(2);	\
		$(1) 8000 8 4096 25 >> $(2);	\

bench_single_same_value =                  \
		$(1) 100 8 4096 0 	> $(2);	\
		$(1) 1000 8 4096 0 >> $(2);	\
		$(1) 2000 8 4096 0 >> $(2);	\
		$(1) 3000 8 4096 0 >> $(2);	\
		$(1) 4000 8 4096 0 >> $(2);	\
		$(1) 5000 8 4096 0 >> $(2);	\
		$(1) 6000 8 4096 0 >> $(2);	\
		$(1) 7000 8 4096 0 >> $(2);	\
		$(1) 8000 8 4096 0 >> $(2);	\


make_folder:
	@mkdir -p bin
	@mkdir -p assets


compile: make_folder
	@gcc src/bench.c tests/no-merge/dalloc.c -o bin/bench-no-merge.o
	@gcc src/bench.c tests/merge/dalloc.c -o bin/bench-merge.o
	@gcc src/bench.c src/dalloc.c -o bin/bench.o
	@gcc src/view.c src/dalloc.c -o bin/view.o 


start: compile
	@./bin/view.o
	

bench: compile
	@$(call bench_single,bin/bench-no-merge.o,assets/bench-no-merge.csv)
	@$(call bench_single,bin/bench-merge.o,assets/bench-merge.csv)
	@$(call bench_single,bin/bench.o,assets/bench-no-ptr.csv)

	@$(call bench_single_same_value,bin/bench-no-merge.o,assets/bench-no-merge-same.csv)
	@$(call bench_single_same_value,bin/bench-merge.o,assets/bench-merge-same.csv)
	@$(call bench_single_same_value,bin/bench.o,assets/bench-no-ptr-same.csv)



plot: bench
	@gnuplot -p docs/bench.gnu 
	@gnuplot -p docs/bench-same.gnu  
	@gnuplot -p docs/bench-no-ptr.gnu  


report: plot
	@pdflatex -interaction nonstopmode -halt-on-error -file-line-error docs/report.tex > report.log
	@rm report.aux
	@rm report.log