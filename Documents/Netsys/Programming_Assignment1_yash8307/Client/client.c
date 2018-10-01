/*Author:Yasir Aslam Shah
 *Project Assignment 1
 *Network System
 *ECEN 5273
 **/
//client.c
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
#include <errno.h>

#define MAXBUFSIZE 100
#define MAXBUFSIZE1 10000
#define buffsize 1
/*MAIN FUNCTION*/
int main (int argc, char * argv[])
{
        int nbytes,nbytes1;                             // number of bytes send by sendto()
        int sock;                               //this will be our socket
        char buffer[MAXBUFSIZE];
	unsigned int remote_length;
        struct sockaddr_in remote;              //"Internet socket address structure"
        if (argc < 3)
        {
                printf("USAGE:  <server_ip> <server_port>\n");
                exit(1);
        }
        /******************
          Here we populate a sockaddr_in struct with
          information regarding where we'd like to send our packet 
          i.e the Server.
         ******************/
        bzero(&remote,sizeof(remote));               //zero the struct
        remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
        remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address
        //Causes the system to create a generic socket of type UDP (datagram)
        if ((sock = socket(AF_INET, SOCK_DGRAM,0)) < 0)
        {
                printf("unable to create socket\n");
        }
	else printf("socket created succesfully\n");
        /******************
          sendto() sends immediately.  
          it will report an error if the message fails to leave the computer
          however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
         ******************/
	//to send command to the server
struct timeval timeout;
timeout.tv_sec =5;
timeout.tv_usec=0;

if(setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout))<0)
error("setsockopt failed\n");

while(1)
{
  printf("\nPlease enter commands from following list\n1. get [filename]\n2. put [filename]\n3. remove [filename]\n4. ls\n5. exit\n");
  printf("\nWaiting to recieve command\n");
	char command[50];
	bzero(command,50);
	gets(command);
	nbytes =sendto(sock,command,strlen(command),0,(const struct sockaddr *)&remote,sizeof(remote));
	command[nbytes] = '\0';
	
	FILE *pFile;
	printf("Command is %s\n",command);
	char file[10];
	bzero(file,10);
	int value = 1;

  /*command :"get" to recieve file from server.This comamnd is executed as get <filename>, the files are saved as Recieved_<filename>.
   * */
  if((value = strncmp(command,"get",3))==0)
	{
		char file_name[20] = "Recieved_";  
		strcat(file_name,(command+4));
		//to recieve a file from server
		// Blocks till bytes are received
		
		struct sockaddr_in from_addr;
		int addr_length = sizeof(struct sockaddr);
		bzero(buffer,sizeof(buffer));
		remote_length = sizeof(remote);
		//FILE *pFile;
		size_t var2;
		//	remote_length = sizeof(remote);
	
		bzero(buffer,MAXBUFSIZE);
		nbytes = recvfrom(sock,(char *)buffer,MAXBUFSIZE,0, (const struct sockaddr *)&remote,&remote_length);
		size_t f_size = atoi(buffer);
    if(f_size == 0)
    {
      printf("No file found on receiver side\n");
      continue;
    }

		printf("File name is %s\n",file_name);
    pFile=fopen(file_name,"w+");
		if (pFile==NULL) 
		{	
			printf("File not created\n");
			exit (1);	
		}
		else
		{	
			printf("File Created\n");	
		}	
	/*file size is used to run the number of loops so that the packets are sent in 100 bytes size and the value of 'l' is set so that the residue last packet is sent perfectly*/
		//printf("file size = %ld\n",f_size);
		size_t loops = f_size/99;
		size_t rem = f_size - (loops*99);
		//printf("loops are %ld\n",loops);
		//printf("rem are %ld \n",rem);
		size_t i,l,count=0;
		char a[2];
		bzero(a,2);
		char packet_count='1';
		//bzero(buffer,MAXBUFSIZE);
		for(i=0;i<=loops;i++)
		{
			//printf("\n\neye ki %d\n",i);
			bzero(buffer,MAXBUFSIZE);
			if(i==loops)
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
			//	printf("waiting to receive\n");
				nbytes = recvfrom(sock,(char *)buffer,(100-l),0, (const struct sockaddr *)&remote,&remote_length);
				// printf("\nthe nbytes value is: %d value of i is %d\n", nbytes,i);	
				a[0]=buffer[99-l];
				if(nbytes > 0)
				{
					if(a[0] == packet_count)
					{
						//printf("a[0] = %c\n",a[0]);
						flag =0;
						nbytes1 = sendto(sock,a,strlen(a),0,(const struct sockaddr *)&remote,sizeof(remote));
            /*writing 99 bytes of data*/
						var2 = fwrite (buffer,1,(99-l),pFile);
						//printf("signal is %c\n",a[0]);
						if(packet_count=='1')
						{
							//printf("packet count value changed to 2\n");
							packet_count='2';
						}
						else 
						{
							//printf("packet count value changed to 1\n");
							packet_count='1';
						}
					}
					else 
					{
						nbytes1 = sendto(sock,a,strlen(a),0,(const struct sockaddr *)&remote,sizeof(remote));
						flag=1;
						//printf("\nSENDING NACK value of a is %c\n",a[0]);
					}
				}
				else
				{
					flag=1;
				}
			}
	  
			count++;
			printf("count = %ld\n",i);        
		}
	
		printf("File Recieved\n");
		fclose(pFile);
	}	
	//**************************
	/***********file sending*******/
	
  /*command :put is used to send file to server.The command line is put <filename> and the files are stored in server as Sent_<filename>.*/
  else if(strncmp("put",command,3)==0)
	{      
		strcpy(file,command+4);
		printf("%s\n\n",file);
		bzero(buffer,sizeof(buffer));
		// FILE * pFile;
		char f_size[MAXBUFSIZE1];
		
		size_t lSize;
		size_t var1;
		pFile =fopen(file,"r");
		if (pFile==NULL) 
		{
			fputs ("File error",stderr); 
      lSize = 0;
			sprintf(f_size,"%ld",lSize);
		  nbytes = sendto(sock,f_size,strlen(f_size),0,(const struct sockaddr *)&remote,sizeof(remote));
 
      continue;
		}		
		else 
		{
      printf("File Created\n");  
    }
		
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		fseek (pFile , 0 , SEEK_SET);
		
		size_t loops = lSize/99;
		size_t rem = lSize - (loops*99);
		//printf("loops are %ld\n",loops);
		//printf("rem are %ld \n",rem);
		
		sprintf(f_size,"%ld",lSize);
		
		nbytes = sendto(sock,f_size,strlen(f_size),0,(const struct sockaddr *)&remote,sizeof(remote));
		//printf("\nthe nbytes value is: %d\n", nbytes);
		
		//to send a file
		int count=0;
		int i,l,abc,def;
		char a='1';
		char b='2';
		char buff1[buffsize];
		//printf("\nboom\n");
		bzero(buff1,sizeof(buff1));
		abc=1;
		def=1;
		for(i=0;i<=loops;)
		{
			//printf("eye ki %d\n",i);
			
			if(i==loops)
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
				
				var1 = fread (buffer,1,(99-l),pFile);
				buffer[99-l] = '1';//appending 1 as last bit for the packet
				int flag1=1;
				while(flag1)
				{
					nbytes = sendto(sock,buffer,(100-l),0,(const struct sockaddr *)&remote,sizeof(remote));
					nbytes1 = recvfrom(sock,(char *)buff1,buffsize,0, (const struct sockaddr *)&remote,&remote_length);
                  
					if(nbytes1>0)
					{
						char y=buff1[0];
						/////printf("yasir %c\n",y);
						bzero(buff1,1);
						if(y=='1') //comparing y as 1 for ack
						{ 
							flag1=0;
							abc=0;
							//bzero(buff1,1);
							//printf("1 ack hogyi\n");
						}
						else 
						{ 
							// exit(1);
							printf("nack\n");//printing nack in case y is not equal to 1
							//exit(1);
							//bzero(buff1,1);
						}
					}
					else
					{
						flag1=1;//in case the recieved packet for ack and nack is empty
					}
				}
	
				i++;
			}
	
			abc=1;
			count++;
      /*for last loop, l is used to calculate the exact size of the last packet so that the buffer and ack and nack is in sync*/
			if(i==loops)
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
				var1 = fread (buffer,1,(99-l),pFile);
				//printf("\n222 read is: %d\n", var1);

				buffer[99-l] = '2';//appending 2 to the last of the packet
				int flag1=1;
				while(flag1)
				{
					nbytes = sendto(sock,buffer,(100-l),0,(const struct sockaddr *)&remote,sizeof(remote));

					nbytes1 = recvfrom(sock,(char *)buff1,buffsize,0, (const struct sockaddr *)&remote,&remote_length);
					if(nbytes1>0)//checking if the ack or nack is lost from server
					{
						char y= buff1[0];
						bzero(buff1,1);
						if(y=='2')// ack is if y is equal to 2
						{
							flag1=0;
							def=0;
							//bzero(buff1,1);
							/////printf("3 ack");
						}
						//nack
						else 
						{
				      /*nack*/
							//printf("4 nack\n");
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
printf("count is %d\n",i);
			bzero(buffer,sizeof(buffer));	
		}
    printf("file sent\n");
		fclose(pFile);
	}
  /*command: ls is used to list out the files in a seperate file created on server side and sent to client*/
	else if(strcmp(command,"ls")==0)
	{
		struct sockaddr_in from_addr;
		bzero(buffer,sizeof(buffer));
		remote_length =sizeof(remote);
		size_t var2 = 0;
		size_t nbytes = 0;
		pFile=fopen("received_ls_op.txt","w+");
		if(pFile==NULL)
		{
		  printf("File not created\n");
		  exit(1);
		}
		else
		{
			printf("File Created\n");
		}
		
		bzero(buffer,MAXBUFSIZE);
		nbytes=recvfrom(sock,(char *)buffer,MAXBUFSIZE,0,(const struct sockaddr *)&remote,&remote_length);
		size_t f_size =atoi(buffer);
		size_t loops= f_size/99;
		size_t rem = f_size -(loops*99);
		size_t i,l,count=0;
		char a[2];
		bzero(a,2);
		char packet_count='1';
    for(i=0;i<=loops;i++)//loops are defined by the size of the file
		{
			bzero(buffer,MAXBUFSIZE);
			if(i==loops)
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
				nbytes=recvfrom(sock,(char *)buffer,(100-l),0,(const struct sockaddr *)&remote,&remote_length);
				a[0]=buffer[99-l]; //collecting the last byte and saving it in char 'a' 
				if(nbytes > 0)//checking if the ack or nack is recieved
				{
					if(a[0] ==packet_count) //checking for ack and nack
					{
						flag =0;
						nbytes1 =sendto(sock,a,strlen(a),0,(const struct sockaddr *)&remote,sizeof(remote));
						var2=fwrite(buffer,1,(99-l),pFile);
						if(packet_count =='1')//for ack packet_count =1
						{
							packet_count='2';
						}
						else
						{
							packet_count ='1';
						}
					}
					else
					{
						nbytes1 = sendto(sock,a,strlen(a),0,(const struct sockaddr *)&remote,sizeof(remote));
						flag=1;
					}
				}
				else
				{
					flag=1;
				}
			}
			count++;
		}	
		fclose(pFile);
    char buffer[100];
    strcpy(buffer, "cat received_ls_op.txt");
    system(buffer);
	}
/*command: exit is used to exit client and exit server*/
	else if(strncmp("exit",command,4)==0)
	{
		printf("\n\nExiting Client\n\n");
		exit(1);
	}
  /*command: delete is used to delete the file from server.delete <filename>is used to put in as command.File is created.If file is not available then an error statement is generating on both client and server*/
	else if(strncmp("delete",command,6)==0)
	{      
		// printf("Deleting %s",file)
		strcpy(file,command+7);
		printf("Deleting %s\n\n",file);
	}
	
}

//fclose(pFile);
close(sock);//close socket
}

