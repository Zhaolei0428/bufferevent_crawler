#include"crawler.h"

int GetBitCount(unsigned int hash1)
{
    int CharCount=hash1/8;
    int CharLocation=hash1%8;

    if(((BloomFlag[CharCount]>>(7-CharLocation))%2==1)||((BloomFlag[CharCount]>>(7-CharLocation))%2==-1))
        return 1;
    else
        return 0;
}

int SetBitCount(unsigned int hash1)
{
    int CharCount=hash1/8;
    int CharLocation=hash1%8;

    BloomFlag[CharCount]=BloomFlag[CharCount]|(1<<(7-CharLocation));

    return 1;
}

unsigned int HFLPHash(char url[],unsigned int length)
{
    unsigned int n=0;
    int i;
    char *b=(char *)&n;
    for(i=0;i<strlen(url);i++)
    {
        b[i%4]^=url[i];
    }

    return (n%length);
}

unsigned int StrHash(char url[],unsigned int length)
{
    register unsigned int h;
    register unsigned char *p;

    for(h=0,p=(unsigned char *)url;*p;p++)
    {
        h=31*h+*p;
    }

    return h;
}

unsigned int DJBHash(char url[],unsigned int length)
{
    unsigned int hash1=5381;
    unsigned int i=0;

    for(i=0;i<length;i++)
    {
        hash1=((hash1<<5)+hash1)+url[i];
    }

    return hash1;

}

unsigned int DEKHash(char url[],unsigned int length)
{
    unsigned int hash1=length;
    unsigned int i=0;

    for(i=0;i<length;i++)
    {
        hash1=((hash1<<5)^(hash1>>27))^(url[i]);
    }

    return hash1;
}

unsigned int BPHash(char url[],unsigned int length)
{
    unsigned int hash1=0;
    unsigned int i=0;

    for(i=0;i<length;url++,i++)
    {
        hash1=hash1<<7^(*url);
    }

    return hash1;
}

unsigned int SDBMHash(char url[],int length)
{
    unsigned int hash1=0;
    unsigned int i=0;

    for(i=0;i<length;url++,i++)
    {
        hash1=(*url)+(hash1<<6)+(hash1<<16)-hash1;
    }

    return hash1;
}

unsigned int RSHash(char url[],int length)
{
    unsigned int b=378551;
    unsigned int a=63689;
    unsigned int hash1=0;
    unsigned int i=0;

    for(i=0;i<length;url++,i++)
    {
        hash1=hash1*a+(*url);
        a=a*b;
    }

    return hash1;
}

unsigned int PJWHash(char url[],int length)
{
    const unsigned int BitsInUnsignedInt=(unsigned int)(sizeof(unsigned int)*8);
    const unsigned int ThreeQuarters=(unsigned int)((BitsInUnsignedInt*3)/4);
    const unsigned int OneEighth=(unsigned int)(BitsInUnsignedInt/8);
    const unsigned int HighBits=(unsigned int)(0xFFFFFFFF)<<(BitsInUnsignedInt-OneEighth);
    unsigned int hash1=0;
    unsigned int test=0;
    unsigned int i=0;

    for(i=0;i<length;url++,i++)
    {
        hash1=(hash1<<OneEighth)+(*url);
        if((test=hash1&HighBits)!=0)
        {
            hash1=((hash1^(test>>ThreeQuarters))&(~HighBits));
        }
    }

    return hash1;
}

unsigned int ELFHash(char url[],int length)
{
    unsigned int hash1=0;
    unsigned int x=0;
    unsigned int i=0;

    for(i=0;i<length;url++,i++)
    {
        hash1=(hash1<<4)+(*url);
        if((x=hash1&0xF0000000L)!=0)
        {
            hash1^=(x>>24);
        }
        hash1=hash1&(~x);
    }

    return hash1;
}

int bloomfilter(char url[])
{
    int cnt=0;
    int HashBuf;

    HashBuf=HFLPHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    HashBuf=StrHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    HashBuf=DJBHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    HashBuf=DEKHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    HashBuf=BPHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    HashBuf=SDBMHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    HashBuf=RSHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }
    HashBuf=PJWHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    HashBuf=ELFHash(url,strlen(url))%MAXBITS;
    if(GetBitCount(HashBuf)==1)
    {
        cnt++;
    }
    else
    {
        SetBitCount(HashBuf);
    }

    if(cnt<9)
        return 0;
    else
        return 1;
}


int URLsearch(char *CurrentPage,char *URLSearching)
{
    int state;
    int i,j=0,n;
    int flag;
    int URLSearching_Num=0;
    char CurrentChar;
    char URLBuf[1000];
    //char *SearchedURL;
    char URLhttp[21]="http://news.sohu.com";

    //Start to analyze
    //FSM

    while(URLSearching_Num<q.tail)
    {
        if(strcmp(URLSearching,q.urls[URLSearching_Num])==0)
        {
            break;
        }
        URLSearching_Num++;
    }

    state=0;
    PairBufLength=0;
    for(i=0;CurrentPage[i]!='\0';i++)
    {
        CurrentChar=CurrentPage[i];
        switch(state)
        {
            case 0:if(CurrentChar=='<')
                   {
                       state=1;
                       break;
                   }
                   else
                   {
                       state=0;
                       j=0;
                       break;
                   }
            case 1:if(CurrentChar=='a')
                   {
                       state=2;
                       break;
                   }
                   else
                   {
                       state=0;
                       j=0;
                       break;
                   }
            case 2:if(CurrentChar=='h')
                   {
                       state=3;
                       break;
                   }
                   else if(CurrentChar=='>')
                   {
                       state=0;
                       j=0;
                       break;
                   }
                   else
                   {
                       state=2;
                       break;
                   }
            case 3:if(CurrentChar=='r')
                   {
                       state=4;
                       break;
                   }
                   else if(CurrentChar=='>')
                   {
                       state=0;
                       j=0;
                       break;
                   }
                   else
                   {
                       state=2;
                       break;
                   }
            case 4:if(CurrentChar=='e')
                   {
                       state=5;
                       break;
                   }
                   else if(CurrentChar=='>')
                   {
                       state=0;
                       j=0;
                       break;
                   }
                   else
                   {
                       state=2;
                       break;
                   }
            case 5:if(CurrentChar=='f')
                   {
                       state=6;
                       break;
                   }
                   else if(CurrentChar=='>')
                   {
                       state=0;
                       j=0;
                       break;
                   }
                   else
                   {
                       state=2;
                       break;
                   }
            case 6:if(CurrentChar=='=')
                   {
                       state=7;
                       break;
                   }
                   else if(CurrentChar=='>')
                   {
                       state=0;
                       j=0;
                       break;
                   }
                   else
                   {
                       state=2;
                       break;
                   }
            case 7:if(CurrentChar=='"')
                   {
                       state=10;
                       break;
                   }
                   else if(CurrentChar==' ')
                   {
                       state=7;
                       break;
                   }
                   else
                   {
                       state=0;
                       j=0;
                       break;
                   }
            case 10:if((CurrentChar=='"')||(CurrentChar=='||')||(CurrentChar=='>')||(CurrentChar=='#'))
                    {
                        state=0;
                        j=0;
                        break;
                    }
                    else if(CurrentChar=='/')
                    {
                        state=8;
                        URLBuf[j]=CurrentChar;
                        j++;
                        break;
                    }
                    else
                    {
                        state=10;
                        URLBuf[j]=CurrentChar;
                        j++;
                        break;
                    }
            case 8:if(CurrentChar=='"')
                   {
                       state=9;
                       //j++;
                       break;
                   }
                   else if(CurrentChar=='>')
                   {
                       state=0;
                       j=0;
                       break;
                   }
                   else if(CurrentChar=='\n'||CurrentChar=='\r'||CurrentChar=='#')
                   {
                       state=9;
                       break;
                   }
                   else
                   {
                       state=8;
                       URLBuf[j]=CurrentChar;
                       j++;
                       break;
                   }
            case 9:URLBuf[j]='\0';      //char urlhttp[20]="http://news.sohu.com";
                   state=0;
                   for(n=0;n<=19;n++)
                   {
                       if(URLBuf[n]!=URLhttp[n])
                       {
                           break;
                       }
                   }
                   if(n==20)
                   {
                       flag=bloomfilter(URLBuf);
                       if(flag==0)
                       {
                           urlPush(&q,URLBuf);
                           PairBuf[PairBufLength][0]=q.tail-1;
                           PairBuf[PairBufLength][1]=URLSearching_Num;
                           PairBufLength++;
                           //output
                           fprintf(fURL,"%s %d\n",URLBuf,q.tail-1);
                           fprintf(fLink,"%d %d\n",q.tail-1,URLSearching_Num);
                       }
                       else
                       {
                           int i=1;
                           while(i<q.tail)
                           {
                               if(strcmp(URLBuf,q.urls[i])==0)
                               {
                                   if(alreadyOutput(i,URLSearching_Num)==1)
                                   {
                                       break;
                                   }
                                   else
                                   {
                                       PairBuf[PairBufLength][0]=i;
                                       PairBuf[PairBufLength][1]=URLSearching_Num;
                                       PairBufLength++;
                                       //output
                                       fprintf(fLink,"%d %d\n",i,URLSearching_Num);
                                       break;
                                   }
                               }
                               i++;
                           }
                       }
                       state=0;
                       j=0;
                   }
                   break;
        }
    }

    return 1;
}

int alreadyOutput(int a,int b)
{
    int i;

    for(i=0;i<=PairBufLength;i++)
    {
        if(PairBuf[i][0]==a&&PairBuf[i][1]==b)  //搜索到已经输出过
        {
            return 1;
        }
    }

    return 0;
}

int JudgeURL(char *url)
{
    if(url[4]==':')  //http
    {
        return 1;
    }
    else    //https
    {
        return 0;
    }
}

int ParseURL(char currenturl[])
{
    int i=20;
    int j=0;

    if(currenturl[20]=='/'&&currenturl[21]!='\0')
    {
        while(currenturl[i]!='\0')
        {
            URLHost[j]=currenturl[i];
            i++;
            j++;
        }
        URLHost[j]='\0';
        return 1;
    }
    else
        return 0;

}




int main(int argc,char **argv)
{
    //Open the output files
    fURL=fopen(argv[3],"w");
    if(fURL==NULL)
    {
        printf("URLs.txt open error!\n");
        return 0;
    }
    fLink=fopen("./Links.txt","w");
    if(fLink==NULL)
    {
        printf("Links.txt open error!\n");
        return 0;
    }

    //Initialize the Buf
    for(int i=0;i<=999;i++)
    {
        PairBuf[i][0]=0;
        PairBuf[i][1]=0;
    }
    PairBufLength=0;

    bzero(page,sizeof(page));
    bzero(&evQue,sizeof(evQue));
    memset(&q,0,sizeof(q));

    //Initialize the Bloom Filter
    BloomFlag=(unsigned char *)malloc(SIZECHARS);
    int num=0;
    while(num<SIZECHARS)
    {
        BloomFlag[num]=0;
        //BloomFlag[num]=BloomFlag[num]<<2;
        num++;
    }

    //init server
    strcpy(ipaddress,argv[1]);
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr=inet_addr(ipaddress);

    //创建事件驱动句柄
    base = event_base_new();

    //初始化事件池
    int i=0;
    for(i=0;i<maxEvNum;i++)
    {
        if(addEvent(i)==0)
            printf("addEvent failed!\n");
    }

    strcpy(CurrentUrl,"http://news.sohu.com");
    urlPush(&q,CurrentUrl);

    fputs(CurrentUrl,fURL);
    fputs(" ",fURL);
    fprintf(fURL,"%d",q.tail-1);
    fputs("\n",fURL);

    event_base_loop(base,EVLOOP_NO_EXIT_ON_EMPTY);

    event_base_free(base);

    fclose(fURL);
    fclose(fLink);

    //Reinput the file
    fURL=fopen(argv[3],"a");
    fLink=fopen("./Links.txt","r");
    if(fURL==NULL)
    {
        printf("Reinput URLs.txt open error!\n");
        return 0;
    }
    if(fLink==NULL)
    {
        printf("Reinput Links.txt open error!\n");
        return 0;
    }
    fputs("\n",fURL);
    while(fgets(Buf,10000,fLink)!=NULL)
    {
        fputs(Buf,fURL);
    }
    fclose(fURL);
    fclose(fLink);

    printf("The program exit safely!\n");

    return 0;
}

int isCompleted(int i)
{
    if(page[i].headParsed==0)
    {
        char Line[100];
        char version[100];
        char statusCode[100];
        int j=0,k=0;
        while(j<strlen(page[i].data) && j<99)
        {
            Line[k]=page[i].data[j];
            j++;
            if(Line[k]=='\n')
            {
                Line[k+1]='\0';
                sscanf(Line,"%s",version);
                if(strcmp(version,"HTTP/1.1")!=0)
                    return 2;
                sscanf(Line,"%s %s",version,statusCode);
                if(strcmp(statusCode,"200")!=0)
                {
                    //printf("%s\n",statusCode);

                    return 2;    //不符合要求，不再接收
                }
                break;
            }
            k++;
        }
        if(j==99)
            return 2;

        char name[100];
        int value;
        k=0;
        while(j<strlen(page[i].data))
        {
            Line[k]=page[i].data[j];
            j++;
            if(Line[k]=='\n')
            {
                Line[k+1]='\0';
                if(strcmp(Line,"\r\n")==0)
                {
                    page[i].headLength=j;
                    page[i].headParsed=1;
                    break;
                }
                sscanf(Line,"%s",name);
                //printf("%s\n",name);
                if(strcmp(name,"Content-Length:")==0)
                {
                    sscanf(Line,"%s %d",name,&value);
                    page[i].contentLength=value;
                    //printf("%d\n",value);
                }
                k=-1;
            }
            k++;
        }
        if(page[i].contentLength==0)
            return 2;
    }
    if(page[i].headParsed==1)
    {
        if(((int)strlen(page[i].data)-page[i].headLength)==page[i].contentLength)
            return 1;
        //printf("%d %d \n",strlen(page[i].data)-page[i].headLength,page[i].contentLength);
    }

    return 0;
}

//读回调处理
void read_callback(struct bufferevent * pBufEv, void * pArg)
{
    Para pa=(Para)pArg;
    int i=pa->flag;
    char buf[4100];
    bzero(buf,sizeof(buf));
    int len = bufferevent_read(pBufEv,buf,4100);
    strcat(page[i].data,buf);


    //判断接收结束
    int flag=isCompleted(i);
    if(flag==1)
    {
        dealPage(i);
        bzero(&page[i],sizeof(page[i]));
        //还回事件池
        evNum--;
        pushEv(&evQue,i);
        while(!qEmpty(&q) && !emptyEv(&evQue))
            sendRequest();
        if(evNum==0 && qEmpty(&q))
            event_base_loopbreak(base);
    }
    else if(flag==2)
    {
        //fprintf(logptr,"Event %d 404 not found!\n",i);
        //flush the bufferevent's buffer
        bzero(&page[i],sizeof(page[i]));
        //还回事件池
        evNum--;
        if(evNum==0 && qEmpty(&q))
            event_base_loopbreak(base);
        bufferevent_free(pBufEv);
        addEvent(i);
    }

    return ;
}

//事件回调处理
void event_callback(struct bufferevent * pBufEv, short events, void * pArg)
{

    Para pa=(Para)pArg;
    int i=pa->flag;
    //sockect connected
    if(events & BEV_EVENT_CONNECTED)
    {
        pushEv(&evQue,i);
        //fprintf(logptr,"event %d connected!\n",i);
        if(!qEmpty(&q))
            sendRequest();
    }
    //socket disconnected
    else if(events & BEV_EVENT_EOF)
    {
        //printf("Event %d disconnected!\n",i);
        if(evGetOut(&evQue,i)==0)
        {
            evNum--;
            bzero(&page[i],sizeof(page[i]));
            //urlPush(&q,pa->url);
            //fprintf(logptr,"Event %d disconnected and inread!\n",i);
        }

        if(evNum==0 && qEmpty(&q))
            event_base_loopbreak(base);
        bufferevent_free(pBufEv);
        addEvent(i);
    }
    //错误
    else if(events & BEV_EVENT_ERROR)
    {
        //fprintf(logptr,"event %d error!\n",i);   //log
        bufferevent_free(pBufEv);
        addEvent(i);
    }
    //read_timeout
    else if(events & ( BEV_EVENT_TIMEOUT | BEV_EVENT_READING))
    {
        //fprintf(logptr,"event %d Timeout!\n",i);   //log
        printf("Queue length:%d evNum:%d  --Timeout\n",(q.tail+maxQueLen-q.head)%maxQueLen,evNum);

        if(evGetOut(&evQue,i)==0)
        {
            evNum--;
            bzero(&page[i],sizeof(page[i]));
        }
        pushEv(&evQue,i);
        while(!qEmpty(&q) && !emptyEv(&evQue))
            sendRequest();
        if(evNum==0 && qEmpty(&q))
            event_base_loopbreak(base);
    }

    return ;
}

int addEvent(int i)
{
    //创建socket类型的bufferevent
    bfevent[i] = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    para[i].flag=i;
    //设置回调函数, 及回调函数的参数
    bufferevent_setcb(bfevent[i], read_callback, NULL, event_callback, &para[i]);
    bufferevent_set_timeouts(bfevent[i], &tv, NULL);
    //连接服务器
    if( bufferevent_socket_connect(bfevent[i], (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        printf("connect error\n");
        //fprintf(logptr,"connect error\n");
        return 0;
    }

    return 1;
}

void dealPage(int i)
{
    //Prepare to write the page.
    filecnt++;
    filename[0]='\0';
    filenamebuf[0]='\0';
    strcpy(filename,"./Pages/");
    sprintf(filenamebuf,"%d",filecnt);
    strcat(filename,filenamebuf);
    strcat(filename,".txt");

    //Write the page into disk.
/*    fp[i]=fopen(filename,"w");
    if(fp[i]==NULL)
    {
        printf("Can't create file!\n");
        return;
    }
    fputs(para[i].url,fp[i]);
    fputs("\n",fp[i]);
    fputs(para[i].HTTPRequest,fp[i]);
    fputs("\n",fp[i]);
    fputs(page[i].data,fp[i]);
    fclose(fp[i]);
*/
    //Analysis the page just crawled.
    URLsearch(page[i].data,para[i].url);

    printf("Queue length:%d evNum:%d\n",(q.tail+maxQueLen-q.head)%maxQueLen,evNum);                           //log
    //fprintf(logptr,"Queue length:%d     evNum:%d   evQueLen:%d Event:%d \n",(q.tail+maxQueLen-q.head)%maxQueLen,evNum,(evQue.tail+maxEvNum+1-evQue.head)%(maxEvNum+1),i);    //log

    return ;
}

int sendRequest()
{
    int flag;
    int toDoEv;
    strcpy(CurrentUrl,urlPop(&q));
    URLHost[0]='\0';
    flag=ParseURL(CurrentUrl);
    //Judge the URL: http or https
    if(JudgeURL(CurrentUrl)==0)
    {
        return 0;
    }
    //Make HTTPRequest
    HTTPRequest[0]='\0';
    strcat(HTTPRequest,"GET ");
    if(flag==0)
    {
        strcat(HTTPRequest,"/ HTTP/1.1\r\nHost:news.sohu.com");
    }
    else
    {
        strcat(HTTPRequest,URLHost);
        strcat(HTTPRequest," HTTP/1.1\r\nHost:news.sohu.com");
    }
    strcat(HTTPRequest,"\r\n");
    strcat(HTTPRequest,"Connection:Keep-Alive\r\n\r\n");
    toDoEv = popEv(&evQue);
    strcpy(para[toDoEv].HTTPRequest,HTTPRequest);
    strcpy(para[toDoEv].url,CurrentUrl);
    bufferevent_enable(bfevent[toDoEv], EV_READ);
    bufferevent_write(bfevent[toDoEv],HTTPRequest,(int)strlen(HTTPRequest));
    evNum++;
    return 1;
}
