CXXFLAGS=-std=c++17 -m64 -pipe -O2 -fPIC -Wall

libstm.a: context.cpp context.h tvar.h
	g++-7 $(CXXFLAGS) context.cpp -c -o context.o
	ar rcs libstm.a context.o

clean:
	rm -f *.o *.a
