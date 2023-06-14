#!/bin/bash

if [ ! -d "./objects" ]; then
	mkdir objects	
	echo "Directory objects was created"
fi
if [ ! -d "./bin" ]; then
	mkdir bin
	echo "bin directory was created"
fi

echo "Running compilation"
make
echo "Compilation is completed"
echo "executed file: "bin/cif-reader
