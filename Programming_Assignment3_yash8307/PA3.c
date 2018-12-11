//Author: Yasir Aslam Shah
//ECEN5273
//Fall2018
//Programming Assignment 3

//header defnition
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <string.h>
#include <openssl/md5.h>
#include <time.h>

//macro defnition
//#define TIMEOUT_CACHE 50
#define MAXBUFSIZE 102400//buffer size
#define MAXBUFSIZE1 100000
#define LISTENQ 8 //maximum number of client connection
uint32_t blocked_website_cnt = 0;
static int msec_read,msec_write;
static int sec_read,sec_write;
/*uint32_t cache_count = 0;
  typedef struct cache_file
  {
  char cache_list[500];
  }cached;
  cached listA;*/
typedef struct blocked_file
{
  char blocked_list[10][20];
}blocked;
struct timeval start_time_val;
blocked blacklist;
int TIMEOUT_CACHE;
char TIMEOUT_CACHE1[200];

char bad_request[] =
"HTTP/1.1 500 internal server error\r\n"
"Content-Type: text/html; charset = UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<body><center><h1>ERROR 500:INTERNAL SERVER </h1><br>\r\n";
char Command_error[MAXBUFSIZE1] = "<html><body><H1>ERROR 400 BAD REQUEST: INVALID METHOD </H1></body></html>";
char Protocol_error[MAXBUFSIZE1] =  "<html><body><H1>ERROR 400 BAD REQUEST: INVALID HTTP VERSION</H1></body></html>";
char blocked_reqeust[MAXBUFSIZE1] = "<html><body><H1>ERROR 403 FORBIDDEN </H1></body></html>";
char website_reqeust[MAXBUFSIZE1] = "<html><body><H1>ERROR 404 Not Found error message </H1></body></html>";


//function declaration for function1
int connfd[10];
int sock;

//void function to implement webproxy
void function1(int func_var);
//main function

int main (int argc, char *argv[])
{
  // signal(SIGINT, sig_handler);
  int a, n;                           //This will be our socket
  struct sockaddr_in sin,cliaddr;     //"Internet socket address structure"
  unsigned int remote_length;         //length of the sockaddr_in structure
  socklen_t clilen;
  int child_thread;
  int num=0;

  if (argc < 3)
  {
    printf ("USAGE:  <port>\n");
    exit(1);
  }

  strcpy(TIMEOUT_CACHE1,argv[2]);
  printf("\n***%d***\n",TIMEOUT_CACHE);
  TIMEOUT_CACHE=atoi(TIMEOUT_CACHE1);
  gettimeofday(&start_time_val, (struct timezone *)0);

  FILE *fd;
  fd =fopen("block_website", "r");
  if(fd == NULL)
  {
    printf("\nError in opening blocked file\n");
    exit(1);
  }

  char buffer2[1000];
  uint8_t i = 0;
  while(!feof(fd))
  {
    fgets(buffer2,200,fd);
    strcpy(blacklist.blocked_list[i],buffer2);
    i++;

  }
  blocked_website_cnt = i-1;
  for(int j =0 ;j<blocked_website_cnt ; j++)
  {
    printf("\n%ld %s\n",strlen(blacklist.blocked_list[j]),blacklist.blocked_list[j]);
  }
  fclose(fd);
  /******************
    This code populates the sockaddr_in struct with
    the information about our socket
   ******************/
  struct addrinfo hints, *res, *p;
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  int conn_count = 0;

  if(getaddrinfo(NULL, argv[1], &hints, &res) != 0)
  {
    printf("\nERROR: getaddrinfo\n");
    exit(1);
  }

  for( p = res ; p!= NULL; p = p->ai_next)
  {
    if ((sock = socket(p->ai_family, p->ai_socktype, 0)) == -1)
    {
      //printf("unable to create socket");
      continue;
    }
    /******************
      Once we've created a socket, we must bind that socket to the
      local address and port we've supplied in the sockaddr_in struct
     ******************/
  //else printf("Binded Successfully!\n");
   int optval=1;
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int))== -1)
   {
   	printf("unable to setsocketopt\n");
   	exit(1);
   }
    if (bind(sock, p->ai_addr, p->ai_addrlen) == 0)
    {
      break;
    }
  }
  if(p == NULL)
  {
    printf("\nERROR: socket() or bind() failed\n");
    exit(1);
  }

  //listen to the socket by creating a connection queue, then wait for clients
  if ((listen (sock, LISTENQ)) != 0)
  {
    printf("\nError: listen() failed\n");
    exit(1);
  }
  printf("%s\n","Server running...waiting for connections.");

  while(1)
  {
    //acception connection using created socket, multiple accept connections are created
    //for multiple child threads
    clilen = sizeof(cliaddr);
    connfd[num] = accept (sock, (struct sockaddr *)&cliaddr, &clilen);
    if (connfd[num] < 0)
    {
      perror("accept failed");
      return 1;
    }
    else
    {
      printf("\nSuccessful connection. Connections currently %d\n",conn_count);
      conn_count++;
    }
    printf("%s\n","Accepted.\nReceiving ...");
    child_thread=fork();//forking threads
    if(child_thread==0)//threads created
    {
      printf("Child Thread Created\n");
      function1(connfd[num]);//calling the function1
      num++;
      exit(1);
    }
    else
    {
      close(connfd[num]);
    }
    //if the num is ten, reset it to zero
    if(num==10)
    {
      num=0;
    }
  }
}

//function1 is used to implement webproxy
void function1(int func_var)
{
  //local variable
  char buffer[MAXBUFSIZE],buffer0[MAXBUFSIZE];             //a buffer to store our received message
  size_t var1;
  char full_message[MAXBUFSIZE];
  char* first;
  char* second;
  char* third;
  int conn;
  int recv_bytes=0;
  FILE *pFile;
  int send_bytes=0;
  size_t lSize;
  char head[MAXBUFSIZE];
  char *position;
  char extension[20];
  int nbytes,nbytes1;
  char website[MAXBUFSIZE];
  char web[MAXBUFSIZE];
  char *Requested_url;
  conn=func_var;
  char sys_call1[150];
  int flag;
  int host_file_descritor = 0;
  struct hostent *host_connection;
  struct sockaddr_in host_address;
  int address_length = 0;
  char MainDir[MAXBUFSIZE];
  memset(MainDir, '\0', sizeof(MainDir));
  char SubDir[MAXBUFSIZE];
  memset(SubDir,'\0',sizeof(SubDir));              //number of bytes we receive in our message
  char md5sum_val[MAXBUFSIZE];
  char buffer3[1000];
  bzero(buffer3,1000);
  char buffer4[10000];
  bzero(buffer4,10000);
  char buffer9[10000];
  bzero(buffer9,10000);
  char file_name[500];
  struct timeval current_time_val;
  struct timeval current1_time_val;
  int outer_flag;
  char *addressA;
  char string_cacheA[200][200];
  int sec,msec;
  memset(md5sum_val, '\0' , sizeof(md5sum_val));
  printf("function1");
  bzero(buffer,MAXBUFSIZE);
  recv_bytes = read(conn, buffer,MAXBUFSIZE);
  printf("length %d \n",recv_bytes);
  int j=0,i=0;
  //for loop to eliminate the null parameters
  for(i=0;i<recv_bytes;i++)
  {
    if(buffer[i] !='\0')//incase of a null parameter
    {
      buffer0[j]=buffer[i];
      j++;
    }
  }
  buffer0[j]='\0';
  //printf("cn func%d\n",recv_bytes);
  if(recv_bytes < 0)
  {
    printf("Error recieving I \n");
    shutdown(conn,SHUT_RDWR);//closing the connection
    close(conn);
    return;
  }
  else if(recv_bytes == 0)
  {
    printf("Error Recieving II \n");
    //shutdown(conn,SHUT_RDWR);//closing the connection
    //close(conn);
    return;
  }
  else if(recv_bytes > 0)
  {
    printf("%s","String received from client:");
    printf("\n%s\n",buffer);//command recieved from client
    strncpy(full_message,buffer0,recv_bytes);
    bzero(buffer,MAXBUFSIZE);//clearing the buffer
    first =strtok(full_message," \n");//saving command in first variable
    second =strtok(NULL," \n");//saving file type in second variable
    third =strtok(NULL," \n");//saving protocol in third variable
    printf("Command is %s\n",first);
    printf("Request is %s\n\n",second);
    printf("Protocol is %s %ld\n",third,strlen(third));
    if(third != NULL)
    {
      third[8] = '\0';
    }
    //incase of GET REQUEST
    if(strncmp(first,"GET\0",4)==0)
    {
      printf("GET Recieved\n");
    }
    else
    {//in case of bad command, 500 INTERNAL ERRORis sent to client and displayed on console
      printf("\nERROR: Invalid Command Request Recieved\n");
      nbytes1 = send(conn,bad_request, strlen(Command_error), 0);
      shutdown(conn, SHUT_RDWR);//shutdown the port
      close(conn);//close the listenning connection
      return;
    }
    //HTTP request
    if((strncmp(third,"HTTP/1.1",8)==0) || (strncmp(third,"HTTP/1.0",8)==0))
    {
      printf("Received %s\n",third);
      char *website = strstr(second,"//");
      website = website + 2;
      int website_length = strlen(website);

      for(int i=0;i<website_length;i++)
      {
        if(website[i] == '/')
        {
          break;
        }
        web[i] = website[i];
      }
      //make directory 'CACHE' to store cache files
      if(getcwd(MainDir, sizeof(MainDir)) != NULL)
      {
        printf("\nPresent working directory is %s\n",MainDir);
        sprintf(SubDir,"%s/cache/",MainDir);
        printf("\nWorking directory is %s\n",SubDir);
        char sys_call[150];
        char system_call[1000] = "mkdir -p";
        sprintf(sys_call, "mkdir -p %s", SubDir);
        printf("\nsyscall is %s\n",sys_call);
        system(sys_call);
        printf("\nsystem call done\n");
      }
      //implementing MD5 hashing to encrypt cache file name
      printf("\nDirectory name with cache is %s\n",SubDir);
      MD5_CTX md5_context;
      MD5_Init(&md5_context);
      MD5_Update (&md5_context, second, strlen(second));
      MD5_Final (md5sum_val, &md5_context);

      for (i = 0; i< MD5_DIGEST_LENGTH; i++)
      {
        sprintf(&buffer3[2*i],"%02x", md5sum_val[i]);
      }
      //parsing website and URL
      sprintf(file_name,"%s.html",buffer3);
      sprintf(SubDir,"%s%s",SubDir,file_name);
      printf("\nFile name with current working directory is %s\n",SubDir);
      printf("\nWebsite + / = %s\n",website);
      printf("\nPath recieved = %s\n",second);
      Requested_url = strstr(website,"/");
      printf("\nWeb is %s %ld\n",web,strlen(web));
      printf("\nPath to url is %s\n",Requested_url);
      //blocking websites as per user
      for(int j =0 ;j<blocked_website_cnt ; j++)
      {
        if(strncmp(web,blacklist.blocked_list[j],strlen(web)) == 0)
        {
          printf("\nERROR:Blocked Website\n");
          write(conn,blocked_reqeust,strlen(blocked_reqeust));
          return;
        }
      }
      host_address.sin_family = AF_INET;
      host_address.sin_port = htons(80);

      outer_flag=1;
      while(outer_flag)
      {
        if(access(SubDir, F_OK) != -1)//if directory is present
        {
          flag=1;
          //////printf("\n\n\n*****************cache file available**********************\n\n\n");
          FILE *pFile3;
          pFile3=fopen("hi.txt","r");
          char buffer4[10000];
          char string_cache[200][200];
          i=0;
          while(!feof(pFile3))
          {
            fgets(buffer4,200,pFile3);
            strncpy(string_cache[i],buffer4,strlen(buffer4));
            i++;
          }
          int cache_count = i-1;
          int length=0;
          int length1=0;
          int length2=0;
          char * stringA;
          char * stringB;
          int value;
          int flag_for=1;
          int time_difference;
          while (flag_for)
          {
            for(int j =cache_count ;j>=0 ; j--)
            {
              char * stringA =strtok(string_cache[j]," ");
              char * stringB =strtok(NULL ," ");
              int length2=strlen(SubDir);

              if(stringA !=NULL && stringB != NULL)
              {
                if((value =strncmp(stringA,SubDir,length2))==0)//match if the cache file is presest as per path requested
                {
                  int previous_time = atoi(stringB);
                  int x=0;
                  int timeou =0;

                  ////printf("\nFile saved at %d seconds\n",previous_time);
                  gettimeofday(&current_time_val, (struct timezone *)0);
                  sec_read=(int)(current_time_val.tv_sec - start_time_val.tv_sec);
                  ///printf("\nCurrent time is %d seconds\n",sec_read);
                  time_difference = sec_read - previous_time;
                  ////printf("\nTime diff is %d seconds\n",time_difference);
                  if(time_difference >= TIMEOUT_CACHE)//if timeout is greater thaa the time difference between time_saved and time reqiested
                  {
                    printf("\n\n\n*****************TIMEOUT**********************\n\n\n");
                    sprintf(sys_call1, "rm -f %s", SubDir);
                    //printf("\nsyscall is %s\n",sys_call1);
                    system(sys_call1);
                    printf("\n**************Previous Cache File Deleted**************\n");
                    printf("\t\tDeleted File %s\t at Time is %d  as diff is %d\n\n",SubDir,sec_read,time_difference);//delete the cache file

                    flag=0;
                    outer_flag=0;

                  }
                  else
                  {
                    flag =1;
                    printf("\n*****no timeout*********\n");
                  }
                  flag_for=0;
                  j=-1;
                }
              }
            }
          }
          fclose(pFile3);
          while(flag)//in case timeout is not expired, hence retrieve file from cache
          {      pFile = fopen(SubDir, "r");
            if(pFile != NULL)
            {
              int len= sizeof(pFile);
              do
              {
                nbytes = fread(buffer4, 1, len, pFile);
                send(conn,buffer4,nbytes,0);
                bzero(buffer4,10000);
              }while(nbytes);
              printf("\n\n\n**************STORED CACHE FILE SENT*********************\n\n\n");
              printf("\t\tRestored File %s\t at Time is %d  as diff is %d\n",SubDir,sec_read,time_difference);
              fclose(pFile);
              close(conn);
              return;
            }
            else
            {
              printf("\n\n\n*****************cache FILE CANNOT BE FOUND****************\n\n\n");
              flag=0;
            }
          }
        }
        else//create cache file for requested path
        {
          //get website and ipaddress
          address_length =  sizeof(host_address);
          /*addressA=inet_ntoa(host_address.sin_addr);
            printf("********addrress %s********\n", addressA);*/
          FILE *pFile9;
          int k=0;
          pFile9=fopen("iplist.txt","r");//open iplist file to read websiteand Ip address
          if(pFile9 != NULL)
          {
            while(!feof(pFile9))
            {
              //////printf("\n\n\n*****************listed file available**********************\n\n\n");
              fgets(buffer9,200,pFile9);
              strncpy(string_cacheA[k],buffer9,strlen(buffer9));
              //////printf("\nstring from buffer is %s\n",string_cacheA[k]);
              k++;
            }
            //////printf("\n\n\n*****************Scanning for Ip**********************\n\n\n");

            int countA = k-1;
            int value;
            for(int j =0 ;j<countA ; j++)
            {
              char * strA =strtok(string_cacheA[j]," ");
              char * strB =strtok(NULL ," ");
              //////printf("\n\tStr is %s\n",strA);//website available
              //////printf("\n\tStr B is  %s\n",strB);//ip address
              //////printf("\n\tweb is  %s\n",web);//website requested
              //compare website and string from iplist file
              if((strA !=NULL) && (strB != NULL) && (web != NULL))
              {
                //////printf("\n\n\n****************string A and String B not null**********************\n\n\n");
                if((value =strncmp(strA,web,strlen(web)))==0)
                  //fetching ipaddress from file
                {
                  //////printf("\n\n\n*****************String A and web is same**********************\n\n\n");
                  host_address.sin_addr.s_addr = inet_addr(strB);
                  addressA=strB;

                  //////printf("\n\n\n*****************ip address fetched from stringB**********************\n\n\n");
                  //fclose(pFile9);
                  break;
                }
                else//fetching ipaddress from gethostbyname
                {
                  //////printf("\n\n\n*****************String A and web not same**********************\n\n\n");
                  bzero(&host_address,sizeof(host_address));
                  host_connection = gethostbyname(web);
                  if(!host_connection)
                  {
                    printf("\nERROR:Not found Website\n");
                    write(conn,website_reqeust,strlen(website_reqeust));
                    exit(1);
                  }
                  memcpy(&host_address.sin_addr, host_connection->h_addr, host_connection->h_length);
                  //////printf("\n\n\n*****************fetched ip address from web2**********************\n\n\n");
                  address_length =  sizeof(host_address);
                  addressA=inet_ntoa(host_address.sin_addr);
                  //////printf("********addrress web2 %s********\n", addressA);
                }
              }
              else//fetching ipaddress from gethostbyname
              {
                //////printf("\n\n\n*****************String A and web are null**********************\n\n\n");
                //bzero(&host_address,sizeof(host_address));
                host_connection = gethostbyname(web);
                if(!host_connection)
                {
                  printf("\nERROR:Not found Website\n");
                  write(conn,website_reqeust,strlen(website_reqeust));
                  exit(1);
                }
                memcpy(&host_address.sin_addr, host_connection->h_addr, host_connection->h_length);
                //////printf("\n\n\n*****************fetched ip address from web0**********************\n\n\n");
                address_length =  sizeof(host_address);
                addressA=inet_ntoa(host_address.sin_addr);
                //////printf("********addrress web0 %s********\n", addressA);
              }
            }
            fclose(pFile9);
          }
          else//fetching ipaddress from gethostbyname
          {
            //////printf("\n\n\n*****************list file not present**********************\n\n\n");
            //bzero(&host_address,sizeof(host_address));
            host_connection = gethostbyname(web);
            if(!host_connection)
            {
              printf("\nERROR:Not found Website\n");
              write(conn,website_reqeust,strlen(website_reqeust));
              exit(1);
            }
            memcpy(&host_address.sin_addr, host_connection->h_addr, host_connection->h_length);
            //////printf("\n\n\n*****************fetched ip address from web1**********************\n\n\n");
            address_length =  sizeof(host_address);
            addressA=inet_ntoa(host_address.sin_addr);
            //////printf("********addrress web1 %s********\n", addressA);
          }

          char SubDir2[500];
          bzero(SubDir2,500);
          char *Timed1="iplist.txt";
          FILE *pFile4;
          pFile4=fopen(Timed1,"ab");
          sprintf(SubDir2,"%s %s\n",web,addressA);
          fwrite(SubDir2,1,strlen(SubDir2),pFile4);
          fclose(pFile4);
          printf("\n\n\n*****************FILE Created**********************\n\n\n");
          //bzero(&host_address,sizeof(host_address));
          if(Requested_url != 0)
          {
            sprintf(buffer,"GET %s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",Requested_url,third,web);
            /////printf("1 Buffer content is %s \n\n",buffer);
          }
          else
          {
            sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",third,web);
            /////printf("2 Buffer content is %s \n\n",buffer);
          }
          host_address.sin_family = AF_INET;
          host_address.sin_port = htons(80);
          addressA=inet_ntoa(host_address.sin_addr);
          for(int j =0 ;j<blocked_website_cnt ; j++)
          {
            if(strncmp(addressA,blacklist.blocked_list[j],strlen(addressA)) == 0)
            {
              printf("\nERROR:Blocked Website\n");
              write(conn,blocked_reqeust,strlen(blocked_reqeust));
              return;
            }
          }
          printf("\n\naddress value is: %s\n\n",addressA);
          host_file_descritor = socket(AF_INET,SOCK_STREAM,0);
          if(host_file_descritor < 0)
          {
            printf("\nERROR: socket()\n");
            return;
          }
          int hel=1;
          setsockopt(host_file_descritor,SOL_SOCKET,SO_REUSEADDR, &hel, 4);

          int socket_val = connect(host_file_descritor, (struct sockaddr*)&host_address, sizeof(host_address));
          if(socket_val < 0)
          {
            printf("\nERROR: connect()\n");
            close(host_file_descritor);
          }
          //////printf("\nsend bytes\n");
          nbytes = send(host_file_descritor, buffer, sizeof(buffer),0);
          //////printf("\nsend bytes is %d\n",nbytes);

          if(nbytes < 0)
          {
            printf("ERROR: Send failed");
            fclose(pFile);
            return;
          }
          printf("\nSERVER SENDING FILE\n");
          char time_write[50];
          //itoa(sec_write,*time_write,10);
          //printf("\nstring is %s\n",time_write);
          char SubDir1[500];
          bzero(SubDir1,500);

          ////printf("\n*******File name with current working directory is %s\n",SubDir);
          pFile = fopen(SubDir, "ab");
          while(nbytes > 0)
          {
            memset(buffer0,'\0',sizeof(buffer0));
            nbytes = recv(host_file_descritor,buffer0,sizeof(buffer0),0);
            if(nbytes > 0)
            {
              ////printf("Number of bytes is %d\n\n\n",nbytes);
              send(conn,buffer0,nbytes,0);
              fwrite(buffer0,1,nbytes,pFile);
              //printf("")
              char *Timed= "hi.txt";
              FILE *pFile2;
              pFile2=fopen(Timed,"a");
              gettimeofday(&current_time_val, (struct timezone *)0);
              ////printf("Write time is sec=%d ,msec =%d\n",(int)(current_time_val.tv_sec - start_time_val.tv_sec),(int)(current_time_val.tv_usec - start_time_val.tv_usec));
              sec_write=(int)(current_time_val.tv_sec - start_time_val.tv_sec);
              sprintf(SubDir1,"%s %d\n",SubDir,sec_write);
              printf("\t\tCreated File %s\t at Time is %d\n",SubDir,sec_write);
              fwrite(SubDir1,1,strlen(SubDir1),pFile2);//write filename ans timestamp
              fclose(pFile2);
            }

          }
          msec_write=(int)(current_time_val.tv_usec - start_time_val.tv_usec)/1000;
          fclose(pFile);
        }
      }




    }
  }
  shutdown(host_file_descritor,SHUT_RDWR);
  shutdown(conn,SHUT_RDWR);

}
