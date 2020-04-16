build: so-cpp

so-cpp: so-cpp.o hashmap.o linked_list.o
	gcc -Wall so-cpp.o hashmap.o linked_list.o -o so-cpp

so-cpp.o: so-cpp.c
	gcc -Wall so-cpp.c -c -o so-cpp.o

hashmap.o: hashmap.c hashmap.h
	gcc -Wall -c hashmap.c -o hashmap.o

linked_list.o: linked_list.c linked_list.h
	gcc -Wall -c linked_list.c -o linked_list.o

.PHONY:
clean:
	rm -f *.o so-cpp
