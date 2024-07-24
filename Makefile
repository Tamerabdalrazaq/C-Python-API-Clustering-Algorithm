symnmf: symnmf.o symnmf.h
	@echo "Building symnmf"
	@gcc -o symnmf symnmf.o -lm

symnmf.o: C/symnmf.c
	@echo "Compiling C/symnmf.c"
	@gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c C/symnmf.c -o symnmf.o

clean:
	@echo "Cleaning up"
	@rm -f *.o symnmf
