#ifndef CLIENT_H
#define CLIENT_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<sqlite3.h>
#include<signal.h>
#include<time.h>

#define N 32
#define M 256

enum TYPE{
	R = 1,	//用户注册
	L,		//用户登录
	Q,		//查询
	H,		//历史记录
};

//定义通信双方的结构体信息
typedef struct{
	int type;//类型
	char name[N];
	char data[M];
}MSG;

#endif
