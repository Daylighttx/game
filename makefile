game:*.cpp /usr/lib/libzinx.a /usr/local/lib/libprotobuf.a /usr/local/lib/libhiredis.a
	g++ -g -std=c++11 -pthread $^ -o $@ -lpthread 
