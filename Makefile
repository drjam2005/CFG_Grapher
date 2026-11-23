LIBS= -lX11 -lm -L./vendor/raylib/lib/ -l:libraylib.a 


all: ./build/main

./build/main: ./src/main.c ./src/plug/plug_helper.c ./build/libplug.so | ./build/
	cc  -rdynamic -o ./build/main ./src/main.c ./src/plug/plug_helper.c -DRELOAD $(LIBS)

./build/libplug.so: ./src/plug/plug.c | ./build/
	cc  -o ./build/libplug.so ./src/plug/plug.c -shared -fPIC $(LIBS)

./build/:
	mkdir -p build

clean:
	rm -rf ./build/
