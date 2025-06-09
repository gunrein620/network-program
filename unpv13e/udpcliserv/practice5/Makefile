# Makefile for Practice 5.4 UDP Echo Client/Server

CC = gcc
CFLAGS = -I$(HOME)/WS/network-program/unpv13e/lib
LIBDIR = $(HOME)/WS/network-program/unpv13e/lib

CLIENT_SRC = udpcli01.c
SERVER_SRC = udpserv01.c

CLIENT_BIN = p54udpechocli
SERVER_BIN = p54udpechosrv

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR)/dg_cli.o $(LIBDIR)/wrapsock.o $(LIBDIR)/error.o $(LIBDIR)/wrapstdio.o $(LIBDIR)/wraplib.o

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR)/dg_echo.o $(LIBDIR)/wrapsock.o $(LIBDIR)/error.o

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)
