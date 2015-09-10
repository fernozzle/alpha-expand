OBJS = alpha-expand.o lodepng.o
TARGET = bin/alpha-expand

all: $(TARGET) cleanobjs

$(TARGET): $(OBJS)
	$(CC) $^ -o $(TARGET)

%.o: src/%.c
	$(CC) $^ -c -o $@ -std=c99

clean:
	rm -f $(TARGET)

cleanobjs:
	rm -f *.o
