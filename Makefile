all : hyr.o test

hyr.o : steam_account.c steam_account.h list.h
	gcc steam_account.c -std=c11 -c -o hyr.o

test : main.c hyr.o
	gcc main.c hyr.o -std=c11 -lpthread -o test
