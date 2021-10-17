all:
	gcc -Wall -fpic -c src/myArray.c -o obj/myArray.o 
	gcc -Wall -fpic -c src/myThread.c -o obj/myThread.o 
	gcc -o lib/libmyOwnthread.so obj/myArray.o obj/myThread.o -shared

matrix_mult:
	gcc test/matrixMultiplication.c lib/libmyOwnthread.so -lpthread -o bin/matrixMultiplication
	./bin/matrixMultiplication > ./output/matResult.txt

boundedBuffer:
	gcc test/producerConsumer.c lib/libmyOwnthread.so -lpthread -o bin/producerConsumer
	./bin/producerConsumer $(N) $(M) > ./output/prodCons.txt

clean:
	rm -f ./obj/*
	rm -f ./bin/*
	rm -f ./lib/*
	rm -f ./output/*