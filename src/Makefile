CC := g++
CFLAGS := -g  -fexec-charset=UTF8 -std=c++11
TARGET := main
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %cpp,%o,$(SRCS))
all:$(TARGET)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -lpthread
$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

.PHONY:clean	
clean: 
	rm -rf  *.o 
	
