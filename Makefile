CC = g++
TARGET = voicevox_engine_cpp
SRCDIR = src
SRCS = $(wildcard $(SRCDIR)/**/*.cc)
OBJS = $(SRCS:.cc=.o)
LIBDIR =
LIBS =
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBDIR) $(LIBS)
all: clean $(OBJS) $(TARGET)
clean:
	-rm -f $(OBJS) $(TARGET) *.d
