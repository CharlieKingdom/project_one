# 定义编译器和编译选项  
CC=gcc  
CFLAGS=-Wall -Wextra -pedantic  
  
# 定义目标文件  
TARGETS=server  
  
# 定义源文件  
SERVER_SRC=server.c sql_tcp.c  
  
# 定义库文件  
LIBS=-lpthread -lsqlite3  
  
# 默认目标：编译所有目标  
all: $(TARGETS)   
  
# 编译 server  
server: $(SERVER_SRC)  
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)  
  
# 清理生成的文件  
clean:  
	rm -f $(TARGETS)  
	rm -f database.db
  
# phony target 以避免与可能的文件名冲突  
.PHONY: all clean