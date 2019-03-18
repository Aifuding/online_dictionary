#include"../../include/dictionary.h"

int do_register(int fd,MSG *msg);//注册函数
int do_login(int fd,MSG *msg);//登录函数
int do_query(int fd,MSG *msg);//查询单词函数
int do_history(int fd,MSG *msg);//历史查询函数


int main(int argc, const char *argv[])
{
	int fd,len,n=0,ret;
	struct sockaddr_in server_addr;
	MSG msg;

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

	if(connect(fd,(struct sockaddr*)&server_addr,len)<0)
	{
		perror("connect failed");
		//return -1;
	}

	while(1)//一级菜单
	{
		puts("**************************");
		puts("  1.注册  2.登录  3.退出");
		puts("**************************");
		printf("请进行选择：");
		scanf("%d",&n);

		switch(n)
		{
		case 1:
			do_register(fd,&msg);//注册函数
			break;
		case 2:
			ret = do_login(fd,&msg);//登录函数
			if(ret == 1)
				goto next;
			break;
		case 3:
			close(fd);
			puts("已成功退出，感谢您的使用！");
			return 1;
		default:
			puts("输入错误,请重新输入");
		}
	}


next:
	while(1)//二级菜单
	{
		puts("**********************************");
		puts("  1.查询单词  2.历史查询  3.退出");
		puts("**********************************");
		printf("请进行选择：");
		scanf("%d",&n);
	
		switch(n)
		{
		case 1:
			do_query(fd,&msg);//查询单词函数
			break;
		case 2:
			do_history(fd,&msg);//历史查询函数
			break;
		case 3:
			close(fd);
			puts("已成功退出，感谢您的使用！");
			return 1;
		default:
			puts("输入错误,请重新输入");
		}
	}

	return 0;
}
