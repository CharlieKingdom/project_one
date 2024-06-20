#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sql_tcp.h"

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

int listen_fd = -1;
extern struct Data ServerData;

static int set_pthread_detach_attr(pthread_attr_t *attr);
static void *client_msg_proc(void *arg);

/*
** 按Ctrl+C退出server程序时，执行下面操作关闭监听套接字
*/
void signal_handler(int arg)
{
	CloseTable(); // 关闭数据库
	printf("close listen_fd(signal = %d)\n", arg);
	close(listen_fd);
	exit(0);
}

int main(int argc, const char *argv[])
{
	int new_fd = -1;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t saddrlen = sizeof(server);
	socklen_t caddrlen = sizeof(client);

	signal(SIGINT, signal_handler);

	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		printf("socket error!\n");
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (bind(listen_fd, (struct sockaddr *)&server, saddrlen) < 0)
	{
		printf("bind error!\n");
		return -1;
	}

	if (listen(listen_fd, 5) < 0)
	{
		printf("listen error!\n");
		return -1;
	}

	CreatBase();
	CreatTable();

	while (1)
	{
		int new_client_fd;					 // 更改变量名为 new_client_fd 以避免混淆
		socklen_t caddrlen = sizeof(client); // 确保每次循环开始时都设置 caddrlen

		new_client_fd = accept(listen_fd, (struct sockaddr *)&client, &caddrlen);
		if (new_client_fd < 0)
		{
			perror("accept error");
			continue; // 如果 accept 失败，跳过当前迭代，而不是退出整个函数
		}
		printf("new client connected(%d). IP:%s, port:%u\n", new_client_fd, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		int ret = 0;
		pthread_t tid = 0;
		pthread_attr_t thread_attr;

		ret = pthread_attr_init(&thread_attr); // 确保初始化线程属性
		if (ret != 0)
		{
			printf("error:pthread_attr_init failed!\n");
			close(new_client_fd); // 关闭新创建的客户端文件描述符
			continue;			  // 跳过当前迭代
		}

		ret = set_pthread_detach_attr(&thread_attr);
		if (ret == -1)
		{
			printf("error:set_pthread_detach_attr failed!\n");
			pthread_attr_destroy(&thread_attr); // 如果设置失败，销毁线程属性
			close(new_client_fd);				// 关闭新创建的客户端文件描述符
			continue;							// 跳过当前迭代
		}

		// 创建一个新的 int 变量来存储文件描述符的副本
		int *new_fd_ptr = malloc(sizeof(int));
		if (new_fd_ptr == NULL)
		{
			printf("error:malloc failed!\n");
			pthread_attr_destroy(&thread_attr); // 销毁线程属性
			close(new_client_fd);				// 关闭新创建的客户端文件描述符
			continue;							// 跳过当前迭代
		}
		*new_fd_ptr = new_client_fd; // 复制文件描述符到新的变量

		if (pthread_create(&tid, &thread_attr, client_msg_proc, (void *)new_fd_ptr) != 0)
		{
			perror("pthread_create");
			free(new_fd_ptr);					// 释放内存
			pthread_attr_destroy(&thread_attr); // 销毁线程属性
			close(new_client_fd);				// 关闭新创建的客户端文件描述符
			continue;							// 跳过当前迭代
		}

		ret = pthread_attr_destroy(&thread_attr);
		if (ret != 0)
		{
			printf("error:pthread_attr_destroy failed!\n");
			// 注意：此时线程已经创建，所以即使销毁属性失败，也不会影响线程的执行
			// 但为了清理，我们还是应该检查并报告这个错误
		}
	}

	close(listen_fd);

	return 0;
}

/*
** 线程回调函数，处理接收客户端的消息
*/
static void *client_msg_proc(void *arg)
{
	int linkfd = *(int *)arg;
	size_t rsize;
	char rbuf[200];

	while (1)
	{
		//memset(rbuf, 0, sizeof(rbuf)); // 使用memset代替strcpy来清零缓冲区

		rsize = read(linkfd, rbuf, sizeof(rbuf) - 1); // 减去1以留出空间给字符串的null终止符
		if (rsize < 0)
		{
			perror("read error"); // 使用perror打印更详细的错误消息
			close(linkfd);		  // 关闭文件描述符
			pthread_exit(NULL);	  // 退出线程
		}
		else if (rsize == 0)
		{
			printf("client (fd = %d) is closed!\n", linkfd);
			close(linkfd);
			pthread_exit(NULL);
		}
		else
		{
			char buf[512];
			memset(buf, 0, sizeof(buf));
			get_time(buf);

			rbuf[rsize] = '\0'; // 确保字符串正确终止
			printf("recv from link=%d:%s\n", linkfd, rbuf);

			ServerData.id = linkfd;
			strcpy(ServerData.dept, rbuf);
			strcpy(ServerData.time, buf);
			InsertData(&ServerData);

			write(linkfd, rbuf, rsize); // 只发送实际读取到的数据
		}
	}
}

/*
** 设置线程的分离属性
** 设置该属性后，线程结束后，由系统自动回收线程资源
*/
static int set_pthread_detach_attr(pthread_attr_t *attr)
{
	int ret = 0;

	ret = pthread_attr_init(attr);
	if (ret != 0)
	{
		printf("error:pthread_attr_init failed!\n");
		return -1;
	}

	ret = pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
	if (ret != 0)
	{
		printf("error:pthread_attr_setdetachstate failed!\n");
		return -1;
	}
	return 0;
}