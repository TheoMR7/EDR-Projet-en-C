CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -O2 -I./include
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj
BIN     = esgi_edr

SRCS    = $(SRC_DIR)/main.c $(SRC_DIR)/state_table.c
OBJS    = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(BIN)

re: fclean all

.PHONY: all clean fclean re
