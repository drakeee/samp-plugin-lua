OUTFILE="lua_samp.so"

INC_DIR = ./lua_samp/Include

COMPILE_FLAGS = -m32 -std=c++11 -fPIC -c -O2 -w -D LINUX -I$(INC_DIR)

LUASAMP = -D LUASAMP $(COMPILE_FLAGS)

all: LUASAMP

clean:
	-rm -f *~ *.o *.so

LUASAMP: clean
	gcc $(LUASAMP) ./lua_samp/Include/Lua_351/*.c
	gcc $(LUASAMP) ./lua_samp/Include/sampgdk/*.c
	g++ $(LUASAMP) ./lua_samp/Include/SDK/*.cpp
	g++ $(LUASAMP) ./lua_samp/Include/TinyXML/tinyxml2.cpp
	g++ $(LUASAMP) ./lua_samp/*.cpp
	g++ -m32 -O2 -fshort-wchar -shared -o $(OUTFILE) *.o
	rm *.o
