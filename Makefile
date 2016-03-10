CFLAGS = -std=c99 -Wall

all: zxpre.exe

%.exe: %main.c
	gcc $(CFLAGS)	$< -o $@

clean:
	rm zxpre.exe