/*Author:Yasir Aslam Shah
 * Project Assignment 1
 * Network System
 * ECEN 5273
 * */
//server
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

#define MAXBUFSIZE 100
#define MAXBUFSIZE1 10000
#define buffsize 1
int main (int argc, char * argv[])
{

int sock,a;                           //This will be our socket
struct sockaddr_in sin, remote;     //"Internet socket address structure"
unsigned int remote_length;         //length of the sockaddr_in structure
int nbytes,nbytes1;                        //number of bytes we receive in our message
char buffer[MAXBUFSIZE],buffer1[MAXBUFSIZE];             //a buffer to store our received message
if (argc != 2)
{
    printf ("USAGE:  <port>\n");
    exit(1);
}

        /******************
          This code populates the sockaddr_in struct with
          the information about our socket
         ******************/
        bzero(&sin,sizeof(sin));                    //zero the struct
        sin.sin_family = AF_INET;                   //address family
        sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
        sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


        //Causes the system to create a generic socket of type UDP (datagram)
        if ((sock = socket(AF_INET, SOCK_DGRAM,0)) < 0)
        {
                printf("unable to create socket");
        }
		else printf("Socket Created Succesfully!\n");
        
	/******************
          Once we've created a socket, we must bind that socket to the 
          local address and port we've supplied in the sockaddr_in struct
         ******************/
        perror("ERROR = ");
        if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        {
                printf("unable to bind socket\n");
        }
else printf("Binded Successfully!\n");

struct timeval timeout;
timeout.tv_sec=5;
timeout.tv_usec=0;
if(setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout))<0)
  error("setsocketopt failed\n");

//in value=1;


while(1)
{      
	bzero(buffer1,sizeof(buffer1));
	remote_length = sizeof(remote);
	FILE * pFile;
	nbytes = recvfrom(sock,(char *)buffer1,MAXBUFSIZE,0, (const struct sockaddr *)&remote,&remote_length);
	printf("\nThe client say %s\n", buffer1);
	char file[10];
	int value=1;
	if (strncmp("get",buffer1,3)==0)
	{
		strcpy(file,buffer1+4);
		printf("File:%s\n",file);
		//remote_length = sizeof(remote);
		//waits for an incoming message
		bzero(buffer,sizeof(buffer));
		size_t lSize;
		char f_size[MAXBUFSIZE1];
		size_t var1;
		int abc,def;
		char buff1[buffsize];     
		pFile =fopen(file,"r");
		if (pFile==NULL) 
		{
			printf("\nFile Not Found\n");
		  lSize = 0;
		  sprintf(f_size,"%ld",lSize);
      nbytes = sendto(sock,f_size,strlen(f_size),0,(const struct sockaddr *)&remote,remote_length);
      continue;
		}
		else
		{
			printf("File Created\n");
		}
		/*file size is sent so that the packets are sent in packets of 100 bytes*/	
		// obtain file size :
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		fseek (pFile , 0 , SEEK_SET);
		//printf("File size= %ld \n",lSize);
		
		size_t loops = lSize/99;
		size_t rem = lSize - (loops*99);
		//printf("loops are %ld\n",loops);
		//printf("rem are %ld \n",rem);
		
		sprintf(f_size,"%ld",lSize);
		//printf("\n%d\n", strlen(f_size));
		nbytes = sendto(sock,f_size,strlen(f_size),0,(const struct sockaddr *)&remote,remote_length);
		size_t i,l,count=0;
/*for loop is run for loops so that packets are sent in 100 bytes*/
		for(i=0;i<=loops;)
		{
			//count++;
			if(i==loops)
			{
				l=99-rem;//l is set to 99-rem so that the last packet has exact bytes
			}
			else
			{ 
				l=0;
			}
	
			while(abc)
			{
			
				var1 = fread (buffer,1,(99-l),pFile);
				buffer[99-l] = '1';	
				int flag1=1;
				while(flag1)
				{
					nbytes = sendto(sock,buffer,(100-l),0,(const struct sockaddr *)&remote,sizeof(remote));

					nbytes1 = recvfrom(sock,(char *)buff1,buffsize,0, (const struct sockaddr *)&remote,&remote_length);
					if(nbytes1>0)//to check if ack or nack is recieved 
					{
						char y=buff1[0];
						bzero(buff1,1);
    					if(y == '1')//ack is recieved as y is equal to 1
						{
							flag1=0;
							abc=0;
						}
						else//nack is recieved 
						{ 
							printf("Nack\n");
							exit(1);
						}
						  
					}
					else
					{
						flag1=1;
					}
				}
				
				i++;
			}
	  
			abc=1;
			count++;
			if(i==loops)//for last loop, the size is calculated as per value of l
			{
				l=99-rem;
			}
			else if(i > loops)
			{
				def = 0;
			}
			else
			{
				l=0;
			}
			while(def)
			{
				var1 = fread (buffer,1,(99-l),pFile);
				buffer[99-l] = '2';        
				int flag1=1;
				while(flag1)
				{
					nbytes = sendto(sock,buffer,(100-l),0,(const struct sockaddr *)&remote,sizeof(remote));
					nbytes1 = recvfrom(sock,(char *)buff1,buffsize,0, (const struct sockaddr *)&remote,&remote_length);
					if(nbytes1>0)//checking ack and nack 
					{
						char y= buff1[0];
						bzero(buff1,1);
						//ack   
						if(y=='2')//ack if y is equal to 2
						{
							flag1=0;
							def=0;
						}
						else 
						{
							//qq// printf("4 nack\n");//nack is recieved
						}
					}    
					else
					{
						flag1=1;
					}
				}   
				
				i++;
			}
		
			//printf("\nafter abc and def, i values is %d\n",i);
			def=1;
			count++;
			bzero(buffer,sizeof(buffer));			
	//printf("count = %ld\n",i);
    }
		
		printf("file sent\n");
		fclose(pFile);
	}
	//int value=1;
  /*command: put <filename>.Put is used to recieve file from client.If no file is present the server and client is notified*/
	else if((value= strncmp(buffer1,"put",3))==0)
	{
		//to recieve a file from server
		//printf("puts ho raha hai\n");
		
		// Blocks till bytes are received
		struct sockaddr_in from_addr;
		char file_name[20]="Sent_";
		strcat(file_name,(buffer1+4));
		
		int addr_length = sizeof(struct sockaddr);
		bzero(buffer,sizeof(buffer));
		remote_length = sizeof(remote);
		size_t var2;
	//  remote_length = sizeof(remote);

		pFile=fopen(file_name,"w+");//creating a new file as Sent_filename
		bzero(buffer,MAXBUFSIZE);
		//printf("\nHello\n");
		nbytes = recvfrom(sock,(char *)buffer,MAXBUFSIZE,0, (const struct sockaddr *)&remote,&remote_length);
    size_t f_size = atoi(buffer);
    if(f_size == 0)
    {
      printf("No file on client side\n");
      continue;
    }
    /*file size calculation for loops*/
		//printf("file size = %ld\n",f_size);
		size_t loops = f_size/99;
		size_t rem = f_size - (loops*99);
		//printf("loops are %ld\n",loops);
		//printf("rem are %ld \n",rem);
		size_t i,l,count=0;
		char a[2];
		bzero(a,2);
		
		char packet_count ='1';
		//bzero(buffer,MAXBUFSIZE);
    //
    ///for loops to recieve packets from client **/
		for(i=0;i<=loops;i++)
		{
			//printf("eye ki %d\n",i);
			bzero(buffer,MAXBUFSIZE);
			if(i==loops)//for last loop, l is euqla to the resdiue
			{
				l=99-rem;
			}
			else
			{ 
				l=0;
			}
		
			int flag=1;
			while(flag)
			{
				nbytes = recvfrom(sock,(char *)buffer,(100-l),0,(const struct sockaddr *)&remote,&remote_length);
				a[0] =  buffer[99-l];
				if(nbytes > 0)//checking if ack or nack is recieved
				{
					if(a[0] == packet_count)
					{
						//printf("\na[0]= %c\n",a[0]);
						flag =0;
						nbytes1 = sendto(sock,a,strlen(a),0,(const struct sockaddr *)&remote,sizeof(remote));
						var2 =fwrite(buffer,1,(99-l),pFile);
						if(packet_count == '1')//comparing the last byte with 1 for an ACk
						{
							packet_count ='2';
						}
						else//for nack
						{
							packet_count='1';
						}
					}	 
					else
					{
						nbytes1= sendto(sock,a,strlen(a),0,(const struct sockaddre *)&remote,sizeof(remote));
						flag=1;
					}
				}
				else
				{
					flag=1;
				}
			}

			count++;
		//	printf("count = %ld\n",count);
		}
	   
		printf("file recieved\n");
		fclose(pFile);
	}
  /*command: ls is used to list out the files in a file and send that file to client*/
	else if (strncmp(buffer1,"ls",2)==0)
	{
    //printf("\nInside ls\n");
		size_t nbytes = 0;
		size_t nbytes1 = 0;
		char buffer2[MAXBUFSIZE];
		bzero(buffer2,MAXBUFSIZE);
		char list[40];
		strcpy(list,"ls_op.txt");
    //printf("\nStrcpy completed\n");
		size_t var1,lSize;
		snprintf(buffer2,sizeof(buffer2),"ls -a >%s",list);
		system(buffer2);
    printf("System ls command executed\n");
		FILE *f;
		f= fopen(list,"r");
		if(f==NULL)
		{
			fputs ("file Error",stderr); 
		}
		else
		{
			printf("List File Created\n");
		}
/*calculatimg size of the file**/
		fseek(f,0,SEEK_END);
		lSize =ftell(f);
		fseek(f,0,SEEK_SET);

		//printf("File Size =%ld\n",lSize);
		size_t loops =lSize/99;
		size_t rem = lSize - (loops*99);
		//printf("loops are %ld\n",loops);
		//printf("rem are %ld\n",rem);

		char f_size[MAXBUFSIZE1];
		sprintf(f_size,"%ld",lSize);
		//printf("\n%d\n",strlen(f_size));
		nbytes = sendto(sock,f_size,strlen(f_size),0,(const struct sockaddr *)&remote,sizeof(remote));

		int count=0;
		int i,l,abc,def;
		char buff1[buffsize];
		//printf("\nboom\n");
		bzero(buff1,sizeof(buff1));
		abc=1;
		def=1;
		for(i=0;i<=loops;)
		{
			//printf("eye ki %d\n",i);
			if(i==loops)//for last loop, the l is set as 99-rem for exact size
			{
				l=99-rem;
				//printf("chuk agya\n");
			}
			else
			{
				l=0;
			}
			//printf("value of l is %d\n",l);
			//printf("loops are %ld\n",loops);
			//printf("rem are %ld \n",rem);
			while(abc)
			{
				//printf("abc mai atka");
				var1 = fread (buffer,1,(99-l),f);
				//printf("\n111 read  is: %d\n",var1);
				//strcat(buffer,"1");
				buffer[99-l] = '1';
				int flag1=1;
				while(flag1)
				{
					nbytes = sendto(sock,buffer,(100-l),0,(const struct sockaddr *)&remote,sizeof(remote));//////
					//printf("\n111 the nbytes value is: %d\n", nbytes);
					nbytes1 = recvfrom(sock,(char *)buff1,buffsize,0, (const struct sockaddr *)&remote,&remote_length);
					///// printf("\n111 the nbytes1 value is: %d\n", nbytes1);
					/////printf("111 signal is %s\n",buff1);
					if(nbytes1>0)//checking if nack or ack is recieved
					{
						char y=buff1[0];
						/////printf("yasir %c\n",y);
						bzero(buff1,1);
						if(y=='1')//checking ack for y is equal to 1
						{
							flag1=0;
							abc=0;
							//bzero(buff1,1);
							//printf("1 ack hogyi\n");
						}
						else 
						{		 
							// exit(1);
							printf("nack\n");
							//exit(1);
							//bzero(buff1,1);
						}
					}
					else
					{
						flag1=1;
					}
				}
				i++;
			}
			abc=1;
			count++;
			if(i==loops)//last loop , l calculations for exact packet data size
			{
				//printf("\ni==loops after abc\n");
				l=99-rem;
			}
			else if(i> loops)
			{
				def=0;
			}
			else
			{
				l=0;
			}

			while(def)
			{
				var1 = fread (buffer,1,(99-l),f);
				//printf("\n222 read is: %d\n", var1);
				buffer[99-l] = '2';
				int flag1=1;
				while(flag1)
				{
					nbytes = sendto(sock,buffer,(100-l),0,(const struct sockaddr *)&remote,sizeof(remote));//////
					// printf("\n222 the nbytes value is: %d\n", nbytes);

					nbytes1 = recvfrom(sock,(char *)buff1,buffsize,0, (const struct sockaddr *)&remote,&remote_length);
					//////   printf("\n222 the nbytes1 value is: %d\n", nbytes1);
					/////printf("222signal is %s\n",buff1);
					if(nbytes1>0)
					{
						char y= buff1[0];
						bzero(buff1,1);
						/////printf("yasir is %c\n",y);	
						//ack   
						if(y=='2')
						{
							flag1=0;
							def=0;
							//bzero(buff1,1);
							/////printf("3 ack");
						}
						//nack
						else 
						{
							//bzero(buff1,1);
							//def=1;
							printf("Nack\n");
						}
					}
					else
					{
						flag1=1;
					}
				}
				
				i++;
			}
			
			def=1;
			count++;
			bzero(buffer,sizeof(buffer));
			//	printf("i is %d\n",i);	
		}
		
	//	printf("file sent\n");
		fclose(f);
	}
  /*command: exit to exit server and client*/
	else if(strncmp(buffer1,"exit",4)==0)
	{
		printf("\n\nExiting Server\n\n");
		exit(1);
	}
  /*command: Remove to remove files.remove <filename> is used to remove file from server,If file is not 0resent the server and client displays error as no file found*/
	else if (strncmp(buffer1,"remove",6)==0)
	{
		char file[20];
		strcpy(file,buffer1+7);
		pFile=fopen(file,"r");
		if(pFile==NULL)
		{
			printf ("File not found\n");
		}
		else 
		{
			fclose(pFile);
			printf("Deleting %s\n",file);
			bzero(buffer,sizeof(buffer));
			remove(file); 
		}
	}
}        
close(sock);
}
 
