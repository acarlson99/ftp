CFLAGS = -Wall -Wextra -Werror -I include/

SERVER_DIR = cmd/server/
SERVER_OBJ = $(addprefix $(SERVER_DIR), server.o)
SERVER_BIN = $(addprefix $(SERVER_DIR), $(SERVER_NAME))
SERVER_NAME = server

CLIENT_DIR = cmd/client/
CLIENT_OBJ = $(addprefix $(CLIENT_DIR), client.o fnv.o handle_request.o)
CLIENT_BIN = $(addprefix $(CLIENT_DIR), $(CLIENT_NAME))
CLIENT_NAME = client

all: $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME): $(SERVER_BIN)
	-ln -s $(SERVER_BIN) $(SERVER_NAME)

$(SERVER_BIN): $(SERVER_OBJ)

$(CLIENT_NAME): $(CLIENT_BIN)
	-ln -s $(CLIENT_BIN) $(CLIENT_NAME)

$(CLIENT_BIN): $(CLIENT_OBJ)

clean:
	$(RM) $(SERVER_OBJ) $(CLIENT_OBJ)

fclean: clean
	$(RM) $(SERVER_BIN) $(SERVER_NAME) $(CLIENT_BIN) $(CLIENT_NAME)

re: fclean all

debug: CFLAGS += -g
debug: re
