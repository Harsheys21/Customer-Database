CC = clang


CFLAGS = -Wall -Wpedantic -Werror -Wextra -g


all: businessdb

businessdb: businessdb.c
	$(CC) $(CFLAGS) businessdb.c -o businessdb

clean:
	rm -f businessdb

format:
	clang-format -i -style=file *.[ch]
