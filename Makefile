CFLAGS = -Wall -Wextra -Werror -I include/

SERVER_DIR = cmd/server/
SERVER_OBJ = $(addprefix $(SERVER_DIR), server.o signal.o command.o)
SERVER_BIN = $(addprefix $(SERVER_DIR), $(SERVER_NAME))
SERVER_NAME = server

CLIENT_DIR = cmd/client/
CLIENT_OBJ = $(addprefix $(CLIENT_DIR), client.o fnv.o make_request.o)
CLIENT_BIN = $(addprefix $(CLIENT_DIR), $(CLIENT_NAME))
CLIENT_NAME = client

all: $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME): $(SERVER_BIN)
	-ln -s $(SERVER_BIN) $(SERVER_NAME)

$(SERVER_BIN): $(SERVER_OBJ)

$(CLIENT_NAME): $(CLIENT_BIN)
	-ln -s $(CLIENT_BIN) $(CLIENT_NAME)

$(CLIENT_BIN): $(CLIENT_OBJ)

.PHONY: clean
clean:
	$(RM) $(SERVER_OBJ) $(CLIENT_OBJ)

.PHONY: fclean
fclean: clean
	$(RM) $(SERVER_BIN) $(SERVER_NAME) $(CLIENT_BIN) $(CLIENT_NAME)

.PHONY: re
re: fclean all

.PHONY: format
debug: CFLAGS += -g
debug: re

.PHONY: format
format:
	clang-format -i $(CLIENT_OBJ:.o=.c) $(SERVER_OBJ:.o=.c) include/*.h
