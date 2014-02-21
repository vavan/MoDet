all:mdetect

OBJ_FILES := \
    mdetect.o \
    url.o \


$(OBJ_FILES): %.o : %.cpp
	g++ -ggdb `pkg-config --cflags opencv` -c $< 


mdetect: $(OBJ_FILES)
	g++ -ggdb `pkg-config --cflags opencv` $(OBJ_FILES) -o $@ `pkg-config --libs opencv` `curl-config --libs`


clean:
	rm -f *.o mdetect

