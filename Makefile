APP = modet
TARGET = bin/$(APP)

all:$(TARGET)

OBJ_FILES := \
    objs/main.o \
    objs/url.o \
    objs/gridmask.o \
    objs/config.o \
    objs/process.o

debug: CFLAGS += -g -O0 -Wall -Wextra
debug: $(TARGET)

objs:
	mkdir objs

bin:
	mkdir bin

version:
	echo "#define VERSION \"`date +%Y-%m-%d_%H:%M:%S`\"" > version.h

objs/config.o: version

$(OBJ_FILES): objs/%.o : %.cpp | version objs
	g++ -Wall -Wextra -ggdb `pkg-config --cflags opencv` -I./include -c $< -o $@ 

$(TARGET): $(OBJ_FILES) | bin
	g++ -ggdb `pkg-config --cflags opencv` $(OBJ_FILES) -o $@ `pkg-config --libs opencv` `curl-config --libs` -L./libs -ljson -llog4cpp -lconfig++


clean:
	rm -rf bin/ objs/


install:
	cp $(TARGET) /usr/bin/
	cp $(APP).cfg /etc



