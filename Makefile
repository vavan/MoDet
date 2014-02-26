TARGET = bin/modet

all:$(TARGET)

OBJ_FILES := \
    bin/main.o \
    bin/url.o \
    bin/gridmask.o \
    bin/config.o \
    bin/process.o

debug: CFLAGS += -g -O0 -Wall -Wextra
debug: $(TARGET)

bin:
	mkdir bin

$(OBJ_FILES): bin/%.o : %.cpp | bin
	g++ -ggdb `pkg-config --cflags opencv` -I./include -c $< -o $@ 


$(TARGET): $(OBJ_FILES)
	g++ -ggdb `pkg-config --cflags opencv` $(OBJ_FILES) -o $@ `pkg-config --libs opencv` `curl-config --libs` -L./libs -ljson -llog4cpp -lconfig++


clean:
	rm -rf bin/


deploy:
	rm -rf tmp
	mkdir tmp
	cp $(TARGET) tmp/
	cp modet.cfg tmp/
	cd tmp && tar -czf modet.tgz *
	scp tmp/modet.tgz vtymoshchuk@71.96.94.69:/home/vtymoshchuk/



