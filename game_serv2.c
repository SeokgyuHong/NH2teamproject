#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

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
char clnt_name[5][10];
pthread_mutex_t mutx;

typedef struct word_
{
 	char name[10];
    int len;
}word;

word data[5];
word select_data;
char subject[10];
int select_subject;
char used_alpha[50] = " *used alphabet : ";

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
    int fd; //파일 포인터
    int readbuf;
    char fruit_data[5][10] = { '\0', };
    char country_data[5][10] = { '\0', };
    char color_data[5][10] = { '\0', };
    char animal_data[5][10] = { '\0', };
    char *ptr;
    int counter = 0;

	char startgame[40];
    char filebuf[BUF_SIZE] = { '\0', };
	
	int i;
	int select_num;
	int chekc_clnt;


 	 if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL); //뮤텍스 초기화
	serv_sock=socket(PF_INET, SOCK_STREAM, 0); //소켓 생성

	memset(&serv_adr, 0, sizeof(serv_adr)); //address 초기화
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); //본인 ip 주소 반환
	serv_adr.sin_port=htons(atoi(argv[1])); //add할당
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) //바인드
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1) //소켓통신 (5개까지 가능)
		error_handling("listen() error");
	srand((int)time(NULL));
	select_subject = rand()%4;
	
	
    switch (select_subject)
    {
    case 0:
        fd = open("fruit_data.txt", O_RDONLY);
        read(fd, filebuf, BUF_SIZE);
        ptr = strtok(filebuf, " ");
        counter = 0;
        while (ptr != NULL)
        {
            strcpy(fruit_data[counter], ptr);
            counter++;
            ptr = strtok(NULL, " ");
        }
        strcpy(subject, "fruit");
        strcpy(startgame, "GameStart (subject : fruit)\n");
        for (i = 0; i < 5; i++)
        {
            strcpy(data[i].name, fruit_data[i]);
            data[i].len = strlen(fruit_data[i]);
            //		 printf("%s %d", data[i].name,data[i].len);
        }
        break;
    case 1:
        fd = open("country_data.txt", O_RDONLY);
        read(fd, filebuf, BUF_SIZE);
        ptr = strtok(filebuf, " ");
        counter = 0;
        while (ptr != NULL)
        {
            strcpy(country_data[counter], ptr);
            counter++;
            ptr = strtok(NULL, " ");
        }
        strcpy(subject, "country");
        strcpy(startgame, "GameStart (subject : country)\n");
        for (i = 0; i < 5; i++)
        {
            strcpy(data[i].name, country_data[i]);
            data[i].len = strlen(country_data[i]);
            //		 printf("%s %d", data[i].name,data[i].len);
        }
        break;
    case 2:
        fd = open("color_data.txt", O_RDONLY);
        read(fd, filebuf, BUF_SIZE);
        ptr = strtok(filebuf, " ");
        counter = 0;
        while (ptr != NULL)
        {
            strcpy(color_data[counter], ptr);
            counter++;
            ptr = strtok(NULL, " ");
        }
        strcpy(subject, "color");
        strcpy(startgame, "GameStart (subject : color)\n");
        for (i = 0; i < 5; i++)
        {
            strcpy(data[i].name, color_data[i]);
            data[i].len = strlen(color_data[i]);
            //		 printf("%s %d", data[i].name,data[i].len);
        }
        break;
    case 3:
        fd = open("animal_data.txt", O_RDONLY);
        read(fd, filebuf, BUF_SIZE);
        ptr = strtok(filebuf, " ");
        counter = 0;
        while (ptr != NULL)
        {
            strcpy(animal_data[counter], ptr);
            counter++;
            ptr = strtok(NULL, " ");
        }
        strcpy(subject, "animal");
        strcpy(startgame, "GameStart (subject : animal)\n");
        for (i = 0; i < 5; i++)
        {
            strcpy(data[i].name, animal_data[i]);
            data[i].len = strlen(animal_data[i]);
            //		 printf("%s %d", data[i].name,data[i].len);
        }
        break;



   
    }
	 
	
	
	
    select_num = rand()%5;	   
	select_data = data[select_num];
	
	for(i=0 ; i<select_data.len ; i++)
		sol[i] = '_'; //처음에 배열  _초기화


	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz); //통신전
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock; //클라이언트 들ㅇㅓ올때마다 추가
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Player %d connected \n", clnt_cnt);
		if(clnt_cnt == 2)
		{
			send_msg(startgame, 40);
			//send_msg(sol,select_data.len);
		}
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

void * handle_clnt(void * arg) //사용한 알파벳 띄우는 함수
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	char win_msg[85] = "Congraturation You Win. You know English words better than the other player.! \n";
	char name[10]={0,};


	str_len = read(clnt_sock,name,sizeof(name));
	sprintf(clnt_name[clnt_cnt-1],"%s",name);

	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0) //클라이언트에서 입력한 문장, 낱말을 받아옴 
	{
		if(str_len>2)
		{
			check_full_fruit(msg); //문장입력이면 같은지 확인하는 함수
		}
		else
		{
			used_alpha[strlen(used_alpha)+2] = '\0';  //단어 배열에 넣어서
			used_alpha[strlen(used_alpha)+1] = '\n';
			used_alpha[strlen(used_alpha)-1] = msg[0];
			used_alpha[strlen(used_alpha)] = ' ';
		
			printf("%s\n",used_alpha);
			
			check_fruit(msg[0]);	//사용한 거 출력 하고 낱말 맞는지 확인하고 바꿔줌 함수
		}
		
		if(check_end() == 1) //게임끝났을때
		{
			write(clnt_sock, win_msg,strlen(win_msg)); 
			end_send(clnt_sock); 
		}
		else //게임이안끝났으면 지금까지 낱말 상황 등 clnt로 보내줌
		{
			char temp[50];

            strcpy(temp, sol);
           
			strcat(temp,used_alpha);
			
			send_msg(temp, strlen(temp)+1);
		}
	}
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
void end_send(int sock) //끝났을때 보내주는 함수
{
	int i;
	int check_clnt;
	char msg[100] = {0,};
	pthread_mutex_lock(&mutx);
	for(i=0 ; i<clnt_cnt ; i++)
	{
		if(clnt_socks[i] == sock)
			check_clnt = i;
	}
	
	sprintf(msg,"Game end! Player %s win! Answer is %s (press q or Q to exit)\n",clnt_name[check_clnt],select_data.name); 
	for(i=0 ; i<clnt_cnt ; i++)
		write(clnt_socks[i],msg,strlen(msg));
	pthread_mutex_unlock(&mutx);
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
