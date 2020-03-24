PYTHON_FLAGS=`python3-config --includes --libs --cflags`
default:
	gcc -Wall -fPIC -shared -o jpeg_toolbox.so jpeg_toolbox_extension.c $(PYTHON_FLAGS) -L /usr/lib/x86_64-linux-gnu/ -ljpeg 
