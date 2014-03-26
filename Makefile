APP = modet
TARGET = bin/$(APP)

all:$(TARGET)

VPATH := objs/

OBJ_FILES := \
    main.o \
    url.o \
    gridmask.o \
    config.o \
    process.o \
    detector.o

CPP_FILES := $(OBJ_FILES:.o=.cpp)

CFLAGS := -Wall -Wextra -ggdb `pkg-config --cflags opencv` -I./include

debug: CFLAGS += -g -O0
debug: $(TARGET)

objs:
	mkdir objs

bin:
	mkdir bin

version:
	echo "#define VERSION \"`date +%Y-%m-%d_%H:%M:%S`\"" > version.h

config.o: version

.depend: $(CPP_FILES) | objs
	rm -f objs/.depend
	g++ $(CFLAGS) -MM $^ >>  objs/.depend;

-include .depend

$(OBJ_FILES): %.o : %.cpp | .depend
	g++ $(CFLAGS) -c $< -o objs/$@ 

$(TARGET): $(OBJ_FILES) | bin
	g++ -ggdb `pkg-config --cflags opencv` $(addprefix objs/,$(OBJ_FILES)) -o $@ `pkg-config --libs opencv` `curl-config --libs` -L./libs -ljson -llog4cpp -lconfig++


clean:
	rm -rf bin/ objs/


install:
	cp $(TARGET) /usr/bin/
	cp $(APP).cfg /etc
	echo "INSALLED" >> /var/log/$(APP).log
	mkdir /var/run/$(APP)



