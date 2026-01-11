CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I header
EXEC = ocaml_vm # executable

all: $(EXEC)

$(EXEC): main.o file_reader.o stack.o virtual_machine.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main/main.c header/file_reader.h header/stack.h header/virtual_machine.h
	$(CC) $(CFLAGS) -c $<

file_reader.o: implementation/file_reader.c header/file_reader.h
	$(CC) $(CFLAGS) -c $<

stack.o: implementation/stack.c header/stack.h
	$(CC) $(CFLAGS) -c $<

virtual_machine.o: implementation/virtual_machine.c header/virtual_machine.h header/stack.h
	$(CC) $(CFLAGS) -c $<

# to clean the executable => remove ocaml_vm
clean:
	rm -f *.o $(EXEC)