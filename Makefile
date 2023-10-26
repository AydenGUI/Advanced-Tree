CFLAGS = -g -Wall -Wno-unused-variable -pedantic

CC = gcc

Zall: tree

myfind: tree.o
	$(CC) $(CFLAGS) $@.o -o $@

# generic
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:

	rm -f core *.o a.out
	rm -rf *.dSYM
	rm -f tree
