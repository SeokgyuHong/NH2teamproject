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
    int fd; //���� ������
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
  
	pthread_mutex_init(&mutx, NULL); //���ؽ� �ʱ�ȭ
	serv_sock=socket(PF_INET, SOCK_STREAM, 0); //���� ����

	memset(&serv_adr, 0, sizeof(serv_adr)); //address �ʱ�ȭ
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); //���� ip �ּ� ��ȯ
	serv_adr.sin_port=htons(atoi(argv[1])); //add�Ҵ�
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) //���ε�
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1) //������� (5������ ����)
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
		sol[i] = '_'; //ó���� �迭  _�ʱ�ȭ


	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz); //�����
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock; //Ŭ���̾�Ʈ �餷�ÿö����� �߰�
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

void * handle_clnt(void * arg) //����� ���ĺ� ���� �Լ�
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	char win_msg[85] = "Congraturation You Win. You know English words better than the other player.! \n";
	char name[10]={0,};


	str_len = read(clnt_sock,name,sizeof(name));
	sprintf(clnt_name[clnt_cnt-1],"%s",name);

	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0) //Ŭ���̾�Ʈ���� �Է��� ����, ������ �޾ƿ� 
	{
		if(str_len>2)
		{
			check_full_fruit(msg); //�����Է��̸� ������ Ȯ���ϴ� �Լ�
		}
		else
		{
			used_alpha[strlen(used_alpha)+2] = '\0';  //�ܾ� �迭�� �־
			used_alpha[strlen(used_alpha)+1] = '\n';
			used_alpha[strlen(used_alpha)-1] = msg[0];
			used_alpha[strlen(used_alpha)] = ' ';
		
			printf("%s\n",used_alpha);
			
			check_fruit(msg[0]);	//����� �� ��� �ϰ� ���� �´��� Ȯ���ϰ� �ٲ��� �Լ�
		}
		
		if(check_end() == 1) //���ӳ�������
		{
			write(clnt_sock, win_msg,strlen(win_msg)); 
			end_send(clnt_sock); 
		}
		else //�����̾ȳ������� ���ݱ��� ���� ��Ȳ �� clnt�� ������
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
void end_send(int sock) //�������� �����ִ� �Լ�
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
