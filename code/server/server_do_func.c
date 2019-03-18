#include"../../include/dictionary.h"
#define WORD_FILE "../../data/word.txt"

int do_register(int fd,MSG *msg,sqlite3 *db)//注册函数
{
	char *errmsg;
	char sql[M];
	int ret;
	
	sprintf(sql, "insert into user values('%s', %s);",msg->name,msg->data);
	puts(sql);
	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);//插入注册信息
	if(ret != SQLITE_OK)
	{
		printf("%s\n", errmsg);
		strcpy(msg->data,errmsg);
	}
	else
	{
		puts("客户端注册成功！");
		strcpy(msg->data,"OK!");
	}

	ret = send(fd,msg,sizeof(MSG),0);
	if(ret < 0)
	{
		puts("send error");
		exit(0);
	}
}

int do_login(int fd,MSG *msg,sqlite3 *db)//登录函数
{
	//判断msg信息是否在数据库中存在
	char *errmsg;
	char sql[M];
	int nrow,ncloumn,ret;
	char **p_result;
	
	sprintf(sql, "select * from user where name = '%s' and pass = '%s';",msg->name,msg->data);
	puts(sql);
	ret = sqlite3_get_table(db,sql,&p_result,&nrow,&ncloumn,&errmsg);
	if(ret != SQLITE_OK)
	{
		puts(errmsg);
		return -1;
	}
	if(nrow == 1)//查询到的个数 
	{
		//查询成功，数据库中存在此用户
		strcpy(msg->data,"OK");
	}
	else
	{
		strcpy(msg->data,"用户名或密码错误!");
	}
	
	ret = send(fd,msg,sizeof(MSG),0);
	if(ret < 0)
	{
		puts("send error");
		exit(0);
	}
}

//在文件中进行单词查找
static int do_search_word(int fd,MSG* msg)
{
	int ret;
	FILE *fp;
	char temp[M];
	
	fp = fopen(WORD_FILE,"r");
	if(fp == NULL)
	{
		perror("fopen error");
		strcpy(msg->data,"单词文件'word.txt'打开失败！");
		send(fd,msg,sizeof(MSG),0);
		return -1;
	}
	
	while(fgets(temp,M,fp))
	{
		ret = strncmp(temp,msg->data,strlen(msg->data));
		if(ret < 0)
			continue;
		if(ret > 0 || (ret == 0 && temp[strlen(msg->data)] != ' '))
			break;
#if 0		
		//找到了单词
		p = temp + strlen(msg->data);
		while(*p == ' ')//跳过空格
		{
			p++;
		}
#endif
		strcpy(msg->data,temp);
		fclose(fp);
		return 1;
	}
	fclose(fp);
	return 0;
}

static int get_date(char *date)
{
	time_t t;
	struct tm *tp;

	time(&t);

	//进行时间格式转换
	tp = localtime(&t);

	sprintf(date, "%d-%d-%d %d:%d:%d", tp->tm_year + 1900, tp->tm_mon+1, tp->tm_mday, 
			tp->tm_hour, tp->tm_min , tp->tm_sec);
	printf("get date:%s\n", date);

	return 0;
}

int do_query(int fd,MSG *msg,sqlite3 *db)//查询单词函数
{
	char date[M],word[M],sql[M];
	char *errmsg;
	int found;//查询结果 ==1 代表查询到

	strcpy(word,msg->data);
	found = do_search_word(fd, msg);//查询单词
	if(found == 1)
	{
		get_date(date);//获取系统时间
		
		//将记录存到数据库中
        sprintf(sql, "insert into record values('%s', '%s', '%s')", msg->name, date, word);
		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			puts(errmsg);
			return -1;
		}
		else
		{
			puts("数据库存放成功！");
		}			
	}
	else
	{
		strcpy(msg->data, "没有找到这个单词！");	
	}
	send(fd, msg, sizeof(MSG), 0);
	return 0;
}

//查询回调函数
static int history_callback(void* arg,int f_num,char** f_value,char** f_name)
{
	// record  , name  , date  , word 
	int fd;
	MSG msg;

	fd = *((int *)arg);

	sprintf(msg.data, "%s , %s", f_value[1], f_value[2]);

	send(fd, &msg, sizeof(MSG), 0);

	return 0;
}

int do_history(int fd,MSG *msg,sqlite3 *db)//历史查询函数
{
	char sql[128] = {};
	char *errmsg;

	sprintf(sql, "select * from record where name = '%s'", msg->name);
	//查询数据库
	if(sqlite3_exec(db, sql, history_callback,(void *)&fd, &errmsg)!= SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Query record done.\n");
	}
	// 所有的记录查询发送完毕之后，给客户端发出一个结束信息
	msg->data[0] = '\0';

	send(fd, msg, sizeof(MSG), 0);
return 0;
}
