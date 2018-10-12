//Author: Yasir Aslam Shah
//ECEN5273
//Fall2018
//Programming Assignment 2

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
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
//macro defnition
#define MAXBUFSIZE 102400
#define MAXBUFSIZE1 10000
#define LISTENQ 8 //maximum number of client connection
char bad_request[] =
"HTTP/1.1 500 internal server error\r\n"
"Content-Type: text/html; charset = UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<body><center><h1>ERROR 500:INTERNAL SERVER </h1><br>\r\n";
//function declaration for function1
void function1(int func_var);
//main function
int main (int argc, char * argv[])
{
  int sock, a, connfd[10], n;                           //This will be our socket
  struct sockaddr_in sin,cliaddr;     //"Internet socket address structure"
  unsigned int remote_length;         //length of the sockaddr_in structure
  socklen_t clilen;
  int child_thread;
  int num=0;
  /******************
  This code populates the sockaddr_in struct with
  the information about our socket
  ******************/
  bzero(&sin,sizeof(sin));                    //zero the struct
  sin.sin_family = AF_INET;                   //address family
  sin.sin_port = htons(9111);        //htons() sets the port # to network byte order
  sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine
  //Causes the system to create a generic socket of type UDP (datagram)
  if ((sock = socket(AF_INET, SOCK_STREAM,0)) < 0)
  {
    printf("unable to create socket");
  }
  else printf("Socket Created Succesfully!\n");
  /******************
  Once we've created a socket, we must bind that socket to the
  local address and port we've supplied in the sockaddr_in struct
  ******************/
  if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    printf("unable to bind socket\n");
  }
  else printf("Binded Successfully!\n");
  //listen to the socket by creating a connection queue, then wait for clients
  listen (sock, LISTENQ);
  printf("%s\n","Server running...waiting for connections.");
  clilen = sizeof(cliaddr);
  while(1)
  {
    //acception connection using created socket, multiple accept connections are created
    //for multiple child threads
    connfd[num] = accept (sock, (struct sockaddr *) &cliaddr, &clilen);
    if (connfd[num] < 0)
    {
      perror("accept failed");
      return 1;
    }
    else
    {
      printf("%s\n","Accepted.\nReceiving ...");
      child_thread=fork();//forking threads
      if(child_thread==0)
      {
        printf("Child Thread Created\n");
        //printf("conection %d\n",connfd[num]);
        function1(connfd[num]);//calling the function1
        num++;
        //printf("num = %d",num);
        exit(1);
      }
    //if the num is ten, reset it to zero
    if(num==10)
    {
      num=0;
    }
  }
}
}
//function1 is used to recieve command and send appropiate header and file content
void function1(int func_var)
{
  //local variable
  char buffer[MAXBUFSIZE],buffer1[MAXBUFSIZE];             //a buffer to store our received message
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
  char *exten;
  int nbytes,nbytes1;
  conn=func_var;                     //number of bytes we receive in our message
  //printf("conection in func%d\n",conn);
  bzero(buffer,MAXBUFSIZE);
  recv_bytes = recv(conn, buffer,MAXBUFSIZE,0);
  //printf("cn func%d\n",recv_bytes);
  if(recv_bytes < 0)
  {
    printf("Error recieving I \n");
    shutdown(conn,SHUT_RDWR);
    close(conn);
    return;
  }
  else if(recv_bytes == 0)
  {
    printf("Error Recieving II \n");
    shutdown(conn,SHUT_RDWR);
    close(conn);
    return;
  }
  else if(recv_bytes > 0)
  {
    printf("%s","String received from client:");
    printf("\n%s\n",buffer);//command recieved from client
    strncpy(full_message,buffer,recv_bytes);
    bzero(buffer,MAXBUFSIZE);
    first =strtok(full_message," \n");//saving command in first variable
    second =strtok(NULL," \n");//saving file type in second variable
    third =strtok(NULL," \n");//saving protocol in third variable
    printf("Command is %s\n",first);
    printf("Request is %s\n",second);
    printf("Protocol is %s\n",third);
  }
  if(strncmp(first,"GET\0",4)==0)
  {
    printf("GET Recieved\n");
  }
  else
  {//in case of bad command, 500 INTERNAL ERRORis sent to client and displayed on console
    printf("\nERROR: Invalid Command Request Recieved\n");
    nbytes1 = send(conn,bad_request, strlen(bad_request), 0);
    shutdown(conn, SHUT_RDWR);//shutdown the port
    close(conn);//close the listenning connection
    return;
  }
  if((strncmp(third,"HTTP/1.1",8)==0) || (strncmp(third,"HTTP/1.0",8)==0))
  {
    printf("Received %s\n",third);
  }
  else
  {
    printf("\nERROR: Invalid Protocol Request Recieved\n");
    nbytes1 = send(conn,bad_request, strlen(bad_request), 0);
    shutdown(conn, SHUT_RDWR);
    close(conn);
    return;
  }
  char dir[MAXBUFSIZE] = "/home/shah/Documents/Netsys/PA2/www";
  strcat(dir,second);
  printf("\nRequest is %s\n",dir);
  pFile =fopen(dir,"r");
  if (pFile==NULL)
  {
    printf("\nERROR: Invalid File Request Recieved\n");
    nbytes1 = send(conn,bad_request, strlen(bad_request), 0);
    shutdown(conn, SHUT_RDWR);
    close(conn);
    return;
  }
  else
  {
    printf("File Created\n");
  }
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  fseek (pFile , 0 , SEEK_SET);
  //printf("size of file sent is %ld\n",lSize);
  position = strrchr(second,'.');
  strcpy(extension,position);
  if (strcmp(extension,".html")==0)
  {
    exten ="text/html";
    //printf("text is %s",exten);
  }
  else if(strcmp(extension,".txt")==0)
  {
    exten ="text/plain";
    //printf("plain is %s",exten);
  }
  else if(strcmp(extension,".png")==0)
  {
    exten ="image/png";
    //printf("png is %s",exten);
  }
  else if(strcmp(extension,".gif")==0)
  {
    exten ="image/gif";
    //printf("gif is %s",exten);
  }
  else if(strcmp(extension,".jpg")==0)
  {
    exten ="image/jpg";
    //printf("jpg is %s",exten);
  }
  else if(strcmp(extension,".css")==0)
  {
    exten ="text/css";
    //printf("css is %s",exten);
  }
  else if(strcmp(extension,".js")==0)
  {
    exten ="application/javascript";
    //printf("java is %s",exten);
  }
  else
  {
    printf("\nERROR: Invalid Type Request Recieved\n");
    nbytes1 = send(conn,bad_request, strlen(bad_request), 0);
    shutdown(conn, SHUT_RDWR);
    close(conn);
    return;
  }
  sprintf(head,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n",exten,lSize);
  printf("Final Header is %s\n",head);
  //printf("con value is here %d\n",conn);
  send_bytes=write(conn, head, strlen(head));
  if (send_bytes<=0)
  {
    printf("Header not Sent\n");
    //printf("header is %d\n",send_bytes);
    shutdown(conn,SHUT_RDWR);
    close(conn);
    return;
  }
  else
  {
    printf("Header Sent\n");
    //printf("header is %d\n",send_bytes);
  }
  bzero(head,sizeof(head));
  var1 = fread(buffer,1,MAXBUFSIZE,pFile);
  //printf("file read is %ld\n",var1);
  nbytes=write(conn, buffer, var1);
  //printf("sent file as %d\n",nbytes);
  bzero(buffer,sizeof(buffer));
  fclose(pFile);
  shutdown(conn,SHUT_RDWR);
  close(conn);
}
