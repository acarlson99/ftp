CFLAGS = -Wall -Wextra -Werror

SERVER_DIR = cmd/server/
SERVER_OBJ = $(addprefix $(SERVER_DIR), server.o)
SERVER_BIN = $(addprefix $(SERVER_DIR), $(SERVER_NAME))
SERVER_NAME = server

CLIENT_DIR = cmd/client/
CLIENT_OBJ = $(addprefix $(CLIENT_DIR), client.o)
CLIENT_BIN = $(addprefix $(CLIENT_DIR), $(CLIENT_NAME))
CLIENT_NAME = client

all: $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME): $(SERVER_OBJ) $(SERVER_BIN)
	-ln -s $(SERVER_BIN) $(SERVER_NAME)

$(CLIENT_NAME): $(CLIENT_OBJ) $(CLIENT_BIN)
	-ln -s $(CLIENT_BIN) $(CLIENT_NAME)

clean:
	$(RM) $(SERVER_OBJ) $(CLIENT_OBJ)

fclean: clean
	$(RM) $(SERVER_BIN) $(SERVER_NAME) $(CLIENT_BIN) $(CLIENT_NAME)

re: fclean all
