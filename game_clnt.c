#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
	
#define BUF_SIZE 100
#define NAME_SIZE 20
	
void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

int total_cnt = 10;
	
char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];
	
int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void * thread_return;
	char n[NAME_SIZE];
	char m[NAME_SIZE+BUF_SIZE];
	int len;

	if(argc!=3) {
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	 }
	

	scanf("%s",n);
	fgetc(stdin);

	sprintf(name, "[%s]", n);
	sock=socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");
	


	write(sock,name,strlen(name));
	len=read(sock, m, NAME_SIZE+BUF_SIZE-1);
	m[len] = 0;
	fputs(m,stdout);
	fputc('\n',stdout);
	fflush(stdout);
	
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);

	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	close(sock);  
	return 0;
}
	
void * send_msg(void * arg)   // send thread main
{
}
	
void * recv_msg(void * arg)   // read thread main
{

}
	
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
