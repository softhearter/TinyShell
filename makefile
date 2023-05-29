CC=gcc
CFLAGS = -g -Wall
OBJS = myShell.o
TARGET = myShell

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS) -lpthread
clean:
	rm -rf  *.o  $(TARGET)