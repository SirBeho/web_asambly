
CFLAGS = -s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall', 'setValue']" -s "EXPORTED_FUNCTIONS=['_malloc', '_free', '_main']" -pthread -s PTHREAD_POOL_SIZE=5 -s ASYNCIFY=1

build: 
	emcc -o index.html main.c --shell-file plantilla.html $(CFLAGS) 

run: 
	emrun --browser chrome index.html

all: 
	make build & make run