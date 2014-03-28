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
OBJS := objs
BIN := bin
OBJ_FILES := $(addprefix $(OBJS)/,$(CPP_FILES:.cpp=.o))
VERSION = $(OBJS)/version.h 
DEPENDENCY = $(OBJS)/depend 
TARGET = $(BIN)/$(APP)
CPP := g++
LD := g++

all: $(TARGET)

debug: CFLAGS += -g -O0
debug: $(TARGET)

$(OBJS):
	mkdir $(OBJS)

$(BIN):
	mkdir $(BIN)

$(VERSION): | $(OBJS)
	echo "#define VERSION \"`date +%Y-%m-%d_%H:%M:%S`\"" > $@


$(DEPENDENCY): $(CPP_FILES) | $(VERSION)
	rm -f $@
	$(CPP) $(CFLAGS) -MM $^ >> $@

# only include if goal is not clean mor install
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),install)
-include $(DEPENDENCY)
endif
endif

$(OBJ_FILES): objs/%.o : %.cpp | $(DEPENDENCY)
	$(CPP) $(CFLAGS) -c $< -o $@ 

$(TARGET): $(OBJ_FILES) | $(BIN)
	$(LD) $(OBJ_FILES) -o $@ $(LDFLAGS) 


clean:
	rm -rf $(BIN) $(OBJS)


install:
	cp $(TARGET) /usr/bin/
	cp $(APP).cfg /etc/
	echo "INSALLED" >> /var/log/$(APP).log
	mkdir /var/run/$(APP)



