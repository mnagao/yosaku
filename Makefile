
grabkey.so:	grabkey.c
	gcc -shared -o grabkey.so grabkey.c `python-config --includes` `python-config --libs` `pkg-config --libs x11`

clean:
	rm -vf grabkey.so

