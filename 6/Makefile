all:
        make client
        make server

client:
        g++ client.cpp -o client -lzmq

server:
        g++ server.cpp -o server -lzmq

clean:
        rm -rf *.out