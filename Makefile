APP = modet

CPP_FILES := \
    main.cpp \
    url.cpp \
    gridmask.cpp \
    tool.cpp \
    process.cpp \
    detector.cpp

CFLAGS := -Wall -Wextra -ggdb -O3 `pkg-config --cflags opencv` -I./include -I./objs
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
MIN_VERSION := $$(cat version.txt)

all: $(TARGET)

debug: CFLAGS += -g -O0
debug: $(TARGET)

$(OBJS):
	mkdir $(OBJS)

$(BIN):
	mkdir $(BIN)

$(DEPENDENCY): $(CPP_FILES) | $(OBJS)
	rm -f $@
	$(CPP) $(CFLAGS) -MM $^ >> $@

# only include if goal is not clean mor install
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),install)
-include $(DEPENDENCY)
endif
endif

$(OBJ_FILES): objs/%.o : %.cpp | $(DEPENDENCY)
	$(CPP) $(CFLAGS) -DMIN_VERSION=$(MIN_VERSION) -DMAJ_VERSION=$(MAJ_VERSION) -c $< -o $@ 

$(TARGET): $(OBJ_FILES) | $(BIN)
	@echo Build version: $(MAJ_VERSION).$(MIN_VERSION)
	$(LD) $(OBJ_FILES) -o $@ $(LDFLAGS) 

deb: $(TARGET)
	mkdir -p modet/DEBIAN
	mkdir -p modet/etc
	mkdir -p modet/usr/bin
	mkdir -p package
	cp $(TARGET) ./modet/usr/bin
	cp modet.cfg ./modet/etc
	cp control ./modet/DEBIAN
	echo 'Version:' $(MIN_VERSION) >> ./modet/DEBIAN/control
	dpkg-deb -b modet
	mv modet.deb package

clean:
	rm -rf $(BIN) $(OBJS) modet

install:
	cp $(TARGET) /usr/bin/
	cp $(APP).cfg /etc/
	echo "INSALLED" >> /var/log/$(APP).log
	mkdir -p /var/run/$(APP)



