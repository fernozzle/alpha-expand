OBJS = alpha-expand.o lodepng.o
TARGET = bin/alpha-expand
RES = alpha-expand.res

all: $(TARGET) cleanobjs

$(TARGET): $(OBJS) $(RES)
	$(CC) $^ -o $(TARGET)

%.o: src/%.c
	$(CC) $^ -c -o $@ -std=c99

$(RES): alpha-expand.rc
	windres $^ -O coff -o $@

clean:
	rm -f $(TARGET)

cleanobjs:
	rm -f $(OBJS) $(RES)
