#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 200
#define NAME_SIZE 20
	
void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

int total_cnt = 10;
	
char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];
int check_end = 0;

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
	
	printf("***********영어단어 맞히기 게임에 오신걸 환영합니다***********\n");
	printf("	------Rule------\n");
	printf("	1.두명의 플레이어가 필요합니다\n");
	printf("	2.한 알파벳씩 또는 정답 단어를 입력해야합니다.\n");
	printf("	3.먼저 맞히는 사람이 승리합니다\n");
	printf("Player Name : ");
	scanf("%s",n);
	fgetc(stdin);

	sprintf(name, "[%s]", n);
	sock=socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_addr, 0, sizeof(serv_addr)); // serv_Addr 소켓 구조체 초기화
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");
	
	printf("Please wait for other player\n");

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
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	
	printf("알파벳을 하나씩 입력하시오\n");
	while(1) 
	{
 
		if(total_cnt == 0)
		{
			printf("out\n");
			close(sock);
			exit(0);
		}
		else
		{
			fgets(msg, BUF_SIZE, stdin);
			total_cnt--;
            if (!((!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) && check_end == 1))
            {
                printf("remained count : %d\n", total_cnt);
            }
		
		}
		
		
		if((!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))&&check_end==1) 
		{
			close(sock);
			exit(0);
		}


		write(sock, msg, strlen(msg));
	}
	return NULL;
}
	
void * recv_msg(void * arg)   // read thread main
{
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	int str_len;
    int k = 0;
    int data_len;
	while(1)
	{
		str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
      
        data_len = 0;
        while (name_msg[data_len] != NULL)
        {
         
            if (name_msg[data_len] == '*')
                break;
            data_len++;
        }
       
		if(str_len==-1) 
			return (void*)-1;
		name_msg[str_len]=0;
        if (name_msg[0] != '_' || ((name_msg[0] >= 'a') && (name_msg[0] <= 'z')))
            check_end = 1;
    
		//fputs(name_msg, stdout);
      
        if (str_len<60)
        {
            k = 0;
            while (name_msg[k]) {
                fputc(name_msg[k], stdout);
                if (k < data_len - 1)
                {
                    printf(" ");
                }
                k++;
            }
        }
        else
        {
            k = 0;
            while (name_msg[k]) {
                fputc(name_msg[k], stdout);
        
                k++;
            }
        }
	}
	return NULL;
}
	
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
