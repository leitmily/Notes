CC := g++
CFLAGS := -Wall -g --std=c++11

OBJECTS := kmp.o
TARGET := test


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
$(OBJECTS) : %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	-rm -f $(TARGET) *.o