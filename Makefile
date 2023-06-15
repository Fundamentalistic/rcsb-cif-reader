all: cif-format
	cc -g main.c objects/cif-format.o cif-format.h -o bin/cif-reader
	 
cif-format: cif-format.c cif-format.h
	cc -g -c cif-format.c -o objects/cif-format.o

clean:
	rm -r ./bin ./objects
