all: crc

crc: crc.c
	$(CC) -Wall -std=c99 -O3 -o crc crc.c

clean:
	rm -f crc.exe crc