.PHONY: default run grade clean

default: src/ringmaster.c src/sentence.c src/question.c src/utils.c
	gcc -o ringmaster src/ringmaster.c -Wall -Wextra

run: default
	@./ringmaster

clean: 
	rm -f ringmaster

grade:
	python3 test/grader.py ./ringmaster test-cases
