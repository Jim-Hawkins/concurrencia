#!/bin/bash

echo Compiling...
make
if [ $? -eq 0 ] 
then
	echo First test: pruebas file with 1 to 5 producers and queue of length 5 to 10

	for prod in {1..5}
		do
		for len in {5..10}
			do
			echo Test with $prod producers and queue of length $len
			./calculator pruebas $prod $len
			echo
		done
	done
fi
exit 0

