OUTFILE="module_sdk.so"

INC_DIR = ./module_sdk/Include

COMPILE_FLAGS = -m32 -std=c++11 -fPIC -c -O2 -w -D LINUX -I$(INC_DIR)

LUAMODULE = -D LUAMODULE $(COMPILE_FLAGS)

all: LUAMODULE

clean:
	-rm -f *~ *.o *.so

LUAMODULE: clean
	gcc $(LUAMODULE) ./module_sdk/Include/Lua_351/*.c
	g++ $(LUAMODULE) ./module_sdk/*.cpp
	g++ -m32 -O2 -fshort-wchar -shared -o $(OUTFILE) *.o
	rm *.o