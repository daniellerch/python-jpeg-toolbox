default:
	gcc -Wall -fPIC -shared -o jpeg.so jpeg.c -lpython3.7m -ljpeg
clean:
	rm -f *.so 
