CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I$(HEADER_DIR)
EXEC = $(BIN_DIR)/ocaml_vm

# Répertoires
SRC_DIR     = src
OBJ_DIR     = obj
BIN_DIR     = bin
HEADER_DIR  = $(SRC_DIR)/header
IMPL_DIR    = $(SRC_DIR)/implementation
MAIN_DIR    = $(SRC_DIR)/main

# Fichiers sources
SRCS = $(MAIN_DIR)/main.c \
       $(IMPL_DIR)/file_reader.c \
       $(IMPL_DIR)/stack.c \
       $(IMPL_DIR)/virtual_machine.c

# Fichiers objets
OBJS = $(OBJ_DIR)/main.o \
       $(OBJ_DIR)/file_reader.o \
       $(OBJ_DIR)/stack.o \
       $(OBJ_DIR)/virtual_machine.o

# En-têtes
HEADERS = $(HEADER_DIR)/file_reader.h \
          $(HEADER_DIR)/stack.h \
          $(HEADER_DIR)/virtual_machine.h

all: $(EXEC)

# Création des répertoires si nécessaire (order-only prerequisites)
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Édition de liens
$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation des fichiers objets
$(OBJ_DIR)/main.o: $(MAIN_DIR)/main.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/file_reader.o: $(IMPL_DIR)/file_reader.c $(HEADER_DIR)/file_reader.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/stack.o: $(IMPL_DIR)/stack.c $(HEADER_DIR)/stack.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/virtual_machine.o: $(IMPL_DIR)/virtual_machine.c $(HEADER_DIR)/virtual_machine.h $(HEADER_DIR)/stack.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJ_DIR)/*.o $(EXEC)
	-rmdir $(OBJ_DIR) $(BIN_DIR) 2>/dev/null || true

.PHONY: all clean