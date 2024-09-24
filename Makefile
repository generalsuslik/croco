CC = gcc
CC_FLAGS = -g -W -Werror -Wall -std=c11 

TARGET = file_manager

SRC_DIR = src
BIN_DIR = bin

SOURCES = $(SRC_DIR)/*.c

DEL_FILE = rm -f
LIBS_DIRS = -I/.include/
LIBS = $(LIBS_DIRS) -lncurses -lc

.PHONY: clean build
all: clean build

build: $(SOURCE)
	$(CC) $(CC_FLAGS) $(LIBS) $(SOURCES) -o $(BIN_DIR)/$(TARGET)

run:
	./$(BIN_DIR)/$(TARGET)

clean:
	$(DEL_FILE) $(BIN_DIR)/*

