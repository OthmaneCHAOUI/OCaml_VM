# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
INCLUDES = -I header

# Directories
HDR_DIR = header
SRC_DIR = implementation
MAIN_DIR = main

# Sources
SRCS = \
	$(SRC_DIR)/stack.c \
	$(SRC_DIR)/file_reader.c \
	$(SRC_DIR)/virtual_machine.c \
	$(MAIN_DIR)/main.c

# Output
TARGET = vm_ocaml

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Clean
clean:
	rm -f $(TARGET)

# Rebuild
re: clean all


