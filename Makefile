all:
	cc -c main.c cif-format.o cif-format.h -o bin/cif-reader
	 
cif-format: cif-format.c cif-format.h
	cc -c cif-format.c cif-format.h -o objects/cif-format.o

clean:
	rm -r ./bin ./objects