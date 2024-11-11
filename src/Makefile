CC = g++
CFLAGS = 
TARGET = voicevox_engine_cpp
SRCDIR = ./
SRCS = $(wildcard $(SRCDIR)/**/*.cc)
SRCS += $(wildcard $(SRCDIR)/*.cc)
OBJS = $(SRCS:.cc=.o)
LIBDIR =
LIBS =
INCDIR = -I./libraries
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBDIR) $(LIBS)
# $(OBJS): $(SRCS)
# 	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)
all: clean $(OBJS) $(TARGET)
clean:
	-rm -f $(OBJS) $(TARGET) *.d
