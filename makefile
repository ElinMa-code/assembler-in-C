# Compiler
CC = gcc

# Compilation flags
CFLAGS = -g -pedantic -Wall -ansi

# Source files
SOURCES = charsequence.c command.c first_move.c first_pass.c functioning.c mac.c output.c second_pass.c symbols.c table.c thing.c utilities.c assembler.c

# Header files
HEADERS = charsequence.h command.h first_move.h first_pass.h functioning.h mac.h output.h second_pass.h symbols.h table.h thing.h utilities.h globals.h

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = assembler

# Default target
all: $(EXECUTABLE)

# Compile source files into object files
%.o: %.c  $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE)

# Clean up generated files
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
