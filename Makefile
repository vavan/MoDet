APP = modet

CPP_FILES := \
    main.cpp \
    url.cpp \
    gridmask.cpp \
    config.cpp \
    process.cpp \
    detector.cpp

CFLAGS := -Wall -Wextra -ggdb `pkg-config --cflags opencv` -I./include -I./objs
LDFLAGS := `pkg-config --libs opencv` `curl-config --libs` -ggdb -L./libs -ljson -llog4cpp -lconfig++

OBJ_FILES := $(CPP_FILES:.cpp=.o)
TARGET = bin/$(APP)
VPATH = ./objs
CPP := g++
LD := g++

all: $(TARGET)

debug: CFLAGS += -g -O0
debug: $(TARGET)

objs:
	mkdir objs

bin:
	mkdir bin

version.h: | objs
	echo "#define VERSION \"`date +%Y-%m-%d_%H:%M:%S`\"" > objs/version.h


objs/depend: $(CPP_FILES) | version.h
	rm -f objs/depend
	$(CPP) $(CFLAGS) -MM $^ >>  objs/depend

# only include if goal is not clean mor install
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),install)
-include objs/depend
endif
endif

$(OBJ_FILES): %.o : %.cpp | objs/depend
	$(CPP) $(CFLAGS) -c $< -o objs/$@ 

$(TARGET): $(OBJ_FILES) | bin
	$(LD) $(addprefix objs/,$(OBJ_FILES)) -o $@ $(LDFLAGS) 


clean:
	rm -rf bin/ objs/


install:
	cp $(TARGET) /usr/bin/
	cp $(APP).cfg /etc
	echo "INSALLED" >> /var/log/$(APP).log
	mkdir /var/run/$(APP)



