CC=g++-7
CFLAGS=-std=c++17 -O3 -Wall
LIBS=-lpthread -L. -lstm
MAKE=make

list: libstm.a List.h main.cpp
	$(CC) $(CFLAGS) main.cpp List.h -o list $(LIBS)

libstm.a:
	cd stm && $(MAKE)
	mv stm/libstm.a libstm.a


stm: libstm.a

clean:
	rm -f list libstm.a
	cd stm && $(MAKE) clean
