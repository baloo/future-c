CFLAGS:=-g

udpfut: hashmap.o master.o main.o
	gcc -o $@ $^
