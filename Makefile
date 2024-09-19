CC:=gcc
SRC_DIR:=./src
INC_DIR:=./inc
BIN_DIR:=./bin
OBJ_DIR:=./obj
CFLAGS:=-std=c11 -O2 -Wall -I $(INC_DIR)

SRC:=$(wildcard $(SRC_DIR)/*.c)
OBJ:=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

debug: CFLAGS +=-g -D _DEBUG

debug: build
release: build

build: $(OBJ)
	$(CC) $(OBJ) -o $(BIN_DIR)/snake $(LFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(INC) -c $^ -o $@ $(CFLAGS)