# 定义编译器和编译选项  
CC=gcc  
CFLAGS=-Wall -Wextra -pedantic  
  
# 定义目标文件  
TARGETS=client1 client2 client3 #server  
  
# 定义源文件  
CLIENT_SRC=client.c sql_tcp.c
# SERVER_SRC=server.c sql_tcp.c  
  
# 定义库文件  
LIBS= -lsqlite3  
  
# 默认目标：编译所有目标  
all: $(TARGETS)  
  
# 编译 client1, client2, client3  
client1: $(CLIENT_SRC)  
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)  
  
client2: $(CLIENT_SRC)  
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)  
  
client3: $(CLIENT_SRC)  
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)  
  
# 编译 server  
# server: $(SERVER_SRC)  
# 	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)  
  
# 清理生成的文件  
clean:  
	rm -f $(TARGETS)  
	rm -f database.db
  
# phony target 以避免与可能的文件名冲突  
.PHONY: all clean