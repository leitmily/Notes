CC := g++
CFLAGS := -Wall -g --std=c++11 -pthread

OBJECTS := thread.o
TARGET := test


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
$(OBJECTS) : %.o: %.cc
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	-rm -f $(TARGET) *.o