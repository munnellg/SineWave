OBJS = $(wildcard src/*.c)

CC = clang

CFLAGS = -Wall

LDFLAGS = -lSDL2 -lm

TARGET = sinwave 

all : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)
