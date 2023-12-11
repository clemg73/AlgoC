COBJS ?= client.o bmp.o couleur.o json.o validateur.o
SOBJS ?= serveur.o json.o
TOBJS ?= testes.o validateur.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client
TEST = testes

all: $(SERVER) $(CLIENT) $(TEST)

$(SERVER): $(SOBJS)
	gcc -o $(SERVER) $(SOBJS) -lm

$(CLIENT): $(COBJS)
	gcc -o $(CLIENT) $(COBJS)

$(TEST): $(TOBJS)
	gcc -o $(TEST) $(TOBJS)

.c.o: 
	gcc -Wall -Werror -ggdb3 -c $*.c

