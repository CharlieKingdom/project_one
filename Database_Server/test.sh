gcc -o client1 client.c -lpthread
gcc -o client2 client.c -lpthread
gcc -o client3 client.c -lpthread
gcc -o server server.c sql_tcp.c -lpthread -lsqlite3