CC = gcc
CC_FLAGS = -g -W -Werror -Wall -Wpedantic -std=c11 

TARGET = croco

SRC_DIR = src
BIN_DIR = bin

SOURCES = $(SRC_DIR)/*.c

DEL_FILE = rm -f
MKDIR_P = mkdir -p
LIBS_DIRS = -I/.include/
LIBS = $(LIBS_DIRS) -lncurses -lc

.PHONY: clean build
all: clean build

build: $(SOURCE)
	$(CC) $(CC_FLAGS) $(LIBS) $(SOURCES) -o $(BIN_DIR)/$(TARGET)

run:
	./$(BIN_DIR)/$(TARGET)

clean:
	${MKDIR_P} ${BIN_DIR}
	$(DEL_FILE) $(BIN_DIR)/*

