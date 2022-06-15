CC=gcc
SRC_DIR:=src
OBJ_DIR:=obj
BIN_DIR:=.
INC_DIR:=include

EXE:=$(BIN_DIR)/c_comm

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LIB := $(OBJ:$(OBJ_DIR)/%.o=$(OBJ_DIR)/%.a)

INST_INC_DIR = C:\msys64\usr\include#This will only work for me for now, as it's hard-coded
INST_LIB_DIR = C:\msys64\usr\lib


CPPFLAGS := -Iinclude
CFLAGS := -g
LDLIBS := -lzmq

all: $(EXE)

.PHONY: all clean install

$(EXE):$(OBJ) |$(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ 

$(BIN_DIR):
	mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -Recurse ./obj/*
	rm c_comm.exe

$(OBJ_DIR)/%.a: $(OBJ_DIR)/%.o
	ar rcs $@ $^

$(INST_BIN_DIR):
	mkdir "$@\c_comm"

$(INST_LIB_DIR):
	mkdir "$@\c_comm"

install: $(LIB) | $(INST_BIN_DIR) $(INST_LIB_DIR)
	cp ".\$(INC_DIR)\c_comm.h" "$(INST_INC_DIR)\c_comm"
	cp ".\$(OBJ_DIR)\c_comm.a" "$(INST_LIB_DIR)\c_comm"
