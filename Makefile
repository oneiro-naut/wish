shellmake: src/shell.c src/stack.c src/readline.c
	gcc -o bin/wish src/shell.c src/stack.c src/readline.c