CFLAGS := -g
CC := gcc
CLIENT := client
SERVER := server

all:
	$(CC) client.c transfer.c $(CFLAGS) -o $(CLIENT)
	$(CC) server.c transfer.c $(CFLAGS) -o $(SERVER)

clean:
	rm -rf $(CLIENT) $(SERVER)
