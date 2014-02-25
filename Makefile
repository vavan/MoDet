all:mdetect

OBJ_FILES := \
    mdetect.o \
    url.o \
    gridmask.o \
    base.o
    


$(OBJ_FILES): %.o : %.cpp
	g++ -ggdb `pkg-config --cflags opencv` -I./include -c $< 


mdetect: $(OBJ_FILES)
	g++ -ggdb `pkg-config --cflags opencv` $(OBJ_FILES) -o $@ `pkg-config --libs opencv` `curl-config --libs` -L./libs -ljson -llog4cpp


clean:
	rm -f *.o mdetect


deploy:
	scp ./mdetect vtymoshchuk@71.96.94.69:/home/vtymoshchuk/


