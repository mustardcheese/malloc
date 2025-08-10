#compiler
CC = gcc

#flags
CFLAGS = -g -Wall -Wextra -Werror -Iinclude

#dirs
SRC_DIR = src
TEST_DIR = tests
OBJ_DIR = obj

#sources
SOURCES = $(SRC_DIR)/malloc.c
TEST_SOURCES = $(TEST_DIR)/testing.c

#objects
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

#final executable
TARGET = run_tests


#source -> objs
all:$(TARGET)
$(TARGET): $(OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

#test -> objs
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

#clean
clean:
	rm -f $(TARGET) $(OBJ_DIR)/*.o
	rmdir --ignore-fail-on-non-empty $(OBJ_DIR)

.PHONY: all clean
