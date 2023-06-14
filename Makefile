all: cif-format
	cc main.c objects/cif-format.o cif-format.h -o bin/cif-reader
	 
cif-format: cif-format.c cif-format.h
	cc -c cif-format.c -o objects/cif-format.o

clean:
	rm -r ./bin ./objects
