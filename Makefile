CC=gcc
SRC_DIR:=src
OBJ_DIR:=obj
BIN_DIR:=.
INC_DIR:=include

EXE:=$(BIN_DIR)/c_comm

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude
CFLAGS := -g
LDLIBS := -lzmq

all: $(EXE)

.PHONY: all clean

$(EXE):$(OBJ) |$(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ 

$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)
