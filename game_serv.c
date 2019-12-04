#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);
void end_send(int);

int clnt_cnt=0;
int fruit_len_cnt = 0;
char sol[10]={0,};
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

typedef struct fruit_
{
 	char name[10];
    int len;
}fruit;

fruit data[5];
fruit select_data;
char subject[10];
int select_subject;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	char fruit_data[5][10]={"banana","apple","melon","mango","grape"};
	char startgame[40];
	
	int i;
	int select_num;
	int chekc_clnt;


 	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	strcpy(subject,"fruit");
	strcpy(startgame,"GameStart (subject : fruit)\n");

	for(i=0 ; i<5 ; i++)
	{
		strcpy(data[i].name,fruit_data[i]);
		data[i].len = strlen(fruit_data[i]);
	
	}
			
	srand((int)time(NULL));
	
    select_num = rand()%5;	   
	select_data = data[select_num];
	
	for(i=0 ; i<select_data.len ; i++)
		sol[i] = '_';
	sol[select_data.len] = '\n';

	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);

	}
	close(serv_sock);
	return 0;
}
	
void check_fruit(char alpa)
{
	int i;

	for(i=0 ; i<select_data.len ; i++)
	{
		if(select_data.name[i] == alpa)
			sol[i] = alpa;
	}
}

int check_full_fruit(char data[])
{
	data[strlen(data)-1] = '\0';
	
	if(!strcmp(select_data.name,data))
	{
		strcpy(sol,select_data.name);
		return 1;
	}
	
	return -1;
}

int check_end(void)
{
	int i;

	for(i=0 ; i<select_data.len ; i++)
	{
		if(sol[i] == '_')
			return -1;
	}

	return 1;

}

void * handle_clnt(void * arg)
{
}

void end_send(int sock)
{
	
}

void send_msg(char * msg, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg,len);
	pthread_mutex_unlock(&mutx);
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
