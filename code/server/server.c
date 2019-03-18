#include"../../include/dictionary.h"
#define DATABASE  "../../data/my.db"

int do_register(int fd,MSG *msg,sqlite3 *db);//注册函数
int do_login(int fd,MSG *msg,sqlite3 *db);//登录函数
int do_query(int fd,MSG *msg,sqlite3 *db);//查询单词函数
int do_history(int fd,MSG *msg,sqlite3 *db);//历史查询函数
void do_client(int accept_fd,sqlite3 *db);

int main(int argc, const char *argv[])
{
	int fd,len,n=0,ret;
	struct sockaddr_in server_addr,accept_addr;
	MSG msg;
	sqlite3* db;//定义数据库句柄指针
	int accept_fd;//并发服务器
	pid_t pid;

	if(argc != 3)//输入参数个数判断
	{
		puts("input argc error\nplease input ip and port");
		return -1;
	}

	//建立socket
	if((fd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		puts("socket failed");
		return -1;
	}
	
	len = sizeof(struct sockaddr_in);
	memset(&server_addr,0,len);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if(bind(fd,(struct sockaddr*)&server_addr,len) < 0)
	{
		perror("bind failed");
		return -1;
	}
	
	//将套接字设为监听模式
	if(listen(fd,5) < 0)
	{
		perror("listen failed");
		return -1;
	}
	
	//打开数据库
	if(sqlite3_open(DATABASE,&db) != SQLITE_OK)
	{
		//printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	else
		puts("open database success!");
	
	signal(SIGCHLD,SIG_IGN);//处理僵尸进程
	
	while(1)
	{
		//if((accept_fd = accept(fd,(struct sockaddr*)&accept_addr,&len))<0)
		if((accept_fd = accept(fd,NULL,NULL)) < 0)
		{
			perror("accept failed");
			return -1;
		}
		
		if((pid = fork())<0)
		{
			perror("fork failed");
			return -1;			
		}
		else if(pid == 0)//子进程 处理客户端具体的消息
		{
			close(fd);
			do_client(accept_fd,db);
		}
		else//父进程 用来接收客户端的请求
		{
			close(accept_fd);
		}
	}
	return 0;
}

void do_client(int accept_fd,sqlite3 *db)
{
	MSG msg;
	while(recv(accept_fd,&msg,sizeof(MSG),0) > 0)//循环接收客户端信息，并进行type分析
	{
		printf("type = %d\n",msg.type);
		switch(msg.type)
		{
			case R://用户注册
				do_register(accept_fd,&msg,db);
				break;
			case L://用户登录
				do_login(accept_fd,&msg,db);
				break;
			case Q://查询
				do_query(accept_fd,&msg,db);
				break;
			case H://历史记录
				do_history(accept_fd,&msg,db);
				break;
			default:
				printf("非法的消息！\n");
		}
	}
	close(accept_fd);
	exit(0);
}
