# Examples

This directory contains sample c++ implementations for a TCP server and client implementation.

###Building and Running
The examples can be built via `cli` or with `cmake`
###CLI
From the `examples` folder do the following:
```
mkdir -p build
cd build/
```
#####Server:
```
g++ -pthread ../non_block_server.cpp -o server.o
./server.o
```
#####Client:
```
g++ -pthread ../non_blocking_client.cpp -o client.o
./client.o
```

###Cmake
From the `examples` folder do the following:
```
mkdir -p build
cd build/
cmake ../
cmake --build ./
```

And running the server and client executables are the same.

## Testing
The `server.o` can be tested with unix's netcat, `nc`.
In one terminal start the server:
```
./server.o
```
and in another terminal:
```
nc localhost 8080
test
>test (<- sent from server)
```

The server will receive the messages and send them back appended with ">" at the beginning.


