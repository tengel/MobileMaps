CC = g++

mapindexer: main.cpp Index.o Way.o Transform.o Util.h
	$(CC) -g main.cpp Index.o Way.o Transform.o -o mapindexer

%.o: %.cpp %.h
	$(CC) -c -g $<

clean:
	rm --force *.o
	rm --force mapindexer

