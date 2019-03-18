#include"../../include/dictionary.h"

int do_register(int fd,MSG *msg)//注册函数
{
	msg->type = R;
	printf("请输入用户名:");
	scanf("%s",msg->name);
	printf("请输入密码:");
	scanf("%s",msg->data);
	
	//发送注册信息
	if((send(fd,msg,sizeof(MSG),0))< 0)
	{
		printf("send failed\n");
		return -1;
	}

	//接收注册回馈信息
	if(recv(fd,msg,sizeof(MSG),0) <0)
	{
		printf("recv failed\n");
		return -1;
	}
	puts(msg->data);
	return 0;
}

int do_login(int fd,MSG *msg)//登录函数
{
	msg->type = L;
	printf("请输入用户名:");
	scanf("%s",msg->name);
	printf("请输入密码:");
	scanf("%s",msg->data);
	
	//发送注册信息
	if((send(fd,msg,sizeof(MSG),0))< 0)
	{
		printf("send failed\n");
		return -1;
	}
	//接收回馈信息
	if(recv(fd,msg,sizeof(MSG),0) <0)
	{
		printf("recv failed\n");
		return -1;
	}
	
	if(!strncmp(msg->data,"OK",3))
	{
		puts("登录成功!");
		return 1;
	}
	puts("登录失败，请重新登录！");
return 0;
}

int do_query(int fd,MSG *msg)//查询单词函数
{
	msg->type = Q;
	puts("--------------------");
	
	while(1)
	{
		printf("请输入要查询的单词:");
		scanf("%s",msg->data);
		
		//如果输入"#"代表返回上一级
		if(!strncmp(msg->data,"#",2))
			break;
		
		send(fd,msg,sizeof(MSG),0);
		recv(fd,msg,sizeof(MSG),0);
		//打印查询到的信息
		printf("%s\n",msg->data);
	}
return 0;
}

int do_history(int fd,MSG *msg)//历史查询函数
{
	msg->type = H;
	send(fd, msg, sizeof(MSG), 0);

	while(1)
	{
		recv(fd, msg, sizeof(MSG), 0);

		if(msg->data[0] == '\0')
			break;

		//输出历史记录信息
		printf("%s\n", msg->data);
	}
	return 0;
}
