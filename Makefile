
PYTHON_FLAGS=`python3-config --includes --libs --cflags`
default:
	gcc -Wall -fPIC -shared -o jpeg.so jpeg.c $(PYTHON_FLAGS) -ljpeg 
clean:
	rm -f *.so 
