#ifndef CRAWLER_H
#define CRAWLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include<errno.h>
#include<math.h>
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<event2/buffer.h>
#include<pthread.h>
#include<event2/thread.h>
#include <sys/resource.h>


#define maxEvNum 200
#define SIZECHARS 2000000
#define MAXBITS 16000000
#define maxQueLen 400000     //队列最大长度




//******************************可用事件池***************************
typedef struct{
    int head,tail;
    int s[maxEvNum+1];   //留一位标志队满
}EV_Que;

int emptyEv(EV_Que* e)
{
    if(e->head==e->tail)
        return 1;
    else
        return 0;
}

int fullEv(EV_Que* e)
{
    if((e->head-e->tail+maxEvNum+1)%(maxEvNum+1)==1)
        return 1;
    else
        return 0;
}

int popEv(EV_Que* e)
{
    if(!emptyEv(e))
    {
        int head=e->head;
        e->head=(head+1)%(maxEvNum+1);
        return e->s[head];
    }
    else
        return -1;
}

void pushEv(EV_Que* e, int i)
{
    if(!fullEv(e))
    {
        e->s[e->tail]=i;
        e->tail=(e->tail+1)%(maxEvNum+1);
    }
}
int evGetOut(EV_Que* e,int evN)
{
    int i;
    for(i=e->head;i!=e->tail;i=(i+1)%(maxEvNum+1))
    {
        if(evN==e->s[i])
        {
            e->s[i]=popEv(e);
            return 1;
        }
    }
    return 0;
}
EV_Que evQue;
//********************************************************

//事件参数结构**********************
typedef struct{
    int flag;
    char url[300];
    char HTTPRequest[2000];
}Parameter, * Para;
Parameter para[maxEvNum];

//********************************

//queue to store urls**************************************
typedef struct{
    char urls[maxQueLen][300];
    int head,tail;
}urlQue,*quePtr;

urlQue q;

int qEmpty(quePtr q)
{
    if(q->head == q->tail)
        return 1;
    else
        return 0;
}

void urlPush(quePtr q,char* url)
{
    strcpy(q->urls[q->tail], url);
    q->tail=(q->tail+1)%maxQueLen;
}

char* urlPop(quePtr q)
{
    if(!qEmpty(q))
    {
        int head=q->head;
        q->head=(head+1)%maxQueLen;
        return q->urls[head];
    }
    else
        return NULL;
}
//*********************************************************

int evNum=0;  //the number of events that is executing

typedef struct{
    char data[4000000];
    int headParsed;
    int contentLength;
    int headLength;
}Page;
Page page[maxEvNum];

//FILE *fp[maxEvNum];
//FILE* logptr;
struct timeval tv = {2,0};
struct event_base * base = NULL;
struct bufferevent * bfevent[maxEvNum]={NULL};


int filecnt=0;
char filename[100];
char filenamebuf[100];
char ipaddress[16];
//const char ipaddress[]="10.108.86.80";
//10.108.86.80
char CurrentUrl[300];
char HTTPRequest[2000];
char URLHost[1000];
unsigned char *BloomFlag;
struct sockaddr_in serveraddr;
int PairBuf[10000][2];
int PairBufLength;

FILE *fURL,*fLink;
char Buf[10000];

int addEvent(int i);
void urlDeal();
void read_callback(struct bufferevent * pBufEv, void * pArg);
void event_callback(struct bufferevent * pBufEv, short events, void * pArg);
void dealPage(int i);
int sendRequest();
int alreadyOutput(int a,int b);
#endif
