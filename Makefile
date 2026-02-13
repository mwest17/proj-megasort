all: bin/split bin/sort bin/merge

bin/split: src/split.c
	gcc -o bin/split src/split.c

bin/sort: src/sort.c
	gcc -o bin/sort src/sort.c

bin/merge: src/merge.c
	gcc -o bin/merge src/merge.c
