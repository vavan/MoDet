APP = modet

CPP_FILES := \
    main.cpp \
    url.cpp \
    gridmask.cpp \
    tool.cpp \
    process.cpp \
    detector.cpp

CFLAGS := -Wall -Wextra -ggdb `pkg-config --cflags opencv` -I./include -I./objs
LDFLAGS := `pkg-config --libs opencv` `curl-config --libs` -ggdb -L./libs -ljson -llog4cpp -lconfig++
OBJS := objs
BIN := bin
OBJ_FILES := $(addprefix $(OBJS)/,$(CPP_FILES:.cpp=.o))
MIN_VERSION = $(OBJS)/version.out
DEPENDENCY = $(OBJS)/depend 
TARGET = $(BIN)/$(APP)
CPP := g++
LD := g++
MAJ_VERSION := 0


all: $(TARGET)

debug: CFLAGS += -g -O0
debug: $(TARGET)

$(OBJS):
	mkdir $(OBJS)

$(BIN):
	mkdir $(BIN)

$(MIN_VERSION): version.txt | $(OBJS)
	@echo $$(($$(cat $<) + 1)) > $< 
	@cp $< $@
	@touch $@
	@echo Building version: $(MAJ_VERSION).$$(cat $(MIN_VERSION))


$(DEPENDENCY): $(CPP_FILES) | $(MIN_VERSION)
	rm -f $@
	$(CPP) $(CFLAGS) -MM $^ >> $@

# only include if goal is not clean mor install
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),install)
-include $(DEPENDENCY)
endif
endif

$(OBJ_FILES): objs/%.o : %.cpp | $(DEPENDENCY)
	$(CPP) $(CFLAGS) -DMIN_VERSION=$$(cat $(MIN_VERSION)) -DMAJ_VERSION=$(MAJ_VERSION) -c $< -o $@ 

$(TARGET): $(OBJ_FILES) | $(BIN)
	$(LD) $(OBJ_FILES) -o $@ $(LDFLAGS) 


clean:
	rm -rf $(BIN) $(OBJS)


install:
	cp $(TARGET) /usr/bin/
	cp $(APP).cfg /etc/
	echo "INSALLED" >> /var/log/$(APP).log
	mkdir /var/run/$(APP)



