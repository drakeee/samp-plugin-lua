OUTFILE="mta_mysql.so"

INC_DIR = -I./ -I./include -I./include/mysql -I./luaimports

COMPILE_FLAGS = -m32 -std=c++11 -fPIC -c -O2 -w -D LINUX $(INC_DIR)

MTAMYSQL = -D MTAMYSQL $(COMPILE_FLAGS)

all: MTAMYSQL

clean:
	-rm -f *~ *.o *.so

MTAMYSQL: clean
	gcc $(MTAMYSQL) ./luaimports/*.c
	g++ $(MTAMYSQL) ./src/*.cpp
	g++ -m32 -O2 -fshort-wchar -fPIC -shared -static -o $(OUTFILE) `mysql_config --cflags --libs` *.o
	rm *.o
