#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>

void client();
void server();

int pid;
int port;
int choice;

int main(int argc,char * argv[])
{
	// printf("Enter port no : ");
	// scanf("%d",&port);
	char s;
	printf("Enter '0' if TCP or '1' if UDP : ");
	scanf("%d",&choice);
	scanf("%c",&s);
	pid = fork();
	if(pid < 0)
	{
		printf("fork failed\n");
		return;
	}
	else if(pid == 0)
	{
		//printf("child process\n");
		client();
	}
	else
	{
		//printf("parent process\n");
		server();
	}
	return 0;
}

void client()
{
	if (choice == 0)
	{
		int ClientSocket = socket(AF_INET,SOCK_STREAM,0);
		if(ClientSocket<0)
		{
			printf("ERROR WHILE CREATING A SOCKET\n");
			return ;
		}

		struct sockaddr_in serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(5002);
		serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		while(connect(ClientSocket,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0);
		printf("connected\n");
		while(1)
		{
			printf("Enter Command: ");
			char buffer[1024],sendData[1024],recvData[1024];
			char *command;
			bzero(buffer,1024);
			int b;

			fgets(buffer,1024,stdin);
			buffer[(int)strlen(buffer) - 1] ='\0'; 
			command = strtok(buffer," ");
			//printf("..%s..\n",command);
			if (command != NULL)
			{
				//printf("1..\n");
				if (strcmp(command,"Exit") == 0 || strcmp(command,"q") == 0 || strcmp(command,"Q") == 0)
				{
					char *s;
					s = "EXIT";
					strcpy(sendData,s);
					send(ClientSocket,sendData,strlen(sendData),0);
					b = recv(ClientSocket,recvData,1024,0);
					recvData[b] = '\0';
					if (strcmp(recvData,"EXIT") == 0)
					{
						kill(pid,SIGKILL);
						printf("Exiting..\n");
						break;
					}
				}
				if (!strcmp(command,"IndexGet"))
				{
					//printf("2..\n");
					command = strtok(NULL," ");
					if (!strcmp(command,"--shortlist"))
					{
						//printf("*\n");
						char *s;
						s = "ShortList";
						strcpy(sendData,s);
						send(ClientSocket,sendData,strlen(sendData),0);
						sleep(1);
						command = strtok(NULL," ");
						if(command == NULL)
						{
							printf("Usage : IndexGet --shortlist <timestamp1> <timestamp2>\n");
							continue;
						}
						char *init = command;
						strcpy(sendData,init);
						//printf("%s\n", sendData);
						send(ClientSocket,sendData,strlen(sendData),0);
						sleep(1);
						command = strtok(NULL," ");
						if(command == NULL)
						{
							printf("Usage : IndexGet --shortlist <timestamp1> <timestamp2>\n");
							continue;
						}
						char *final = command;
						strcpy(sendData,final);
						//printf("%s\n",sendData );
						send(ClientSocket,sendData,strlen(sendData),0);
						usleep(1);
						//printf("**\n");

						while(1)
						{
							b = recv(ClientSocket,recvData,1024,0);
							recvData[b] = '\0';
							//printf("%s\n",recvData );
							if (strcmp(recvData,"EOF") == 0)
								break;
							fwrite(recvData,1,b,stdout);
						}
						//printf("Done shortlist..\n");
					}
					else if (!strcmp(command,"--longlist"))
					{
						//printf("3..\n");
						char *s;
						s = "LongList";
						strcpy(sendData,s);
						//printf("%s\n",sendData );
						send(ClientSocket,sendData,strlen(sendData),0);
						while(1)
						{
							b = recv(ClientSocket,recvData,1024,0);
							recvData[1024] = '\0';
							if (strcmp(recvData,"EOF") == 0)
								break;
							fwrite(recvData,1,b,stdout);
						}
						printf("Done !!\n");
					}
					else if(!strcmp(command,"--regex"))
					{
						//printf("reg\n");
						char *s;
						s = "Regex";
						bzero(sendData,1024);
						strcpy(sendData,s);
						send(ClientSocket,sendData,strlen(sendData),0);
						sleep(1);
						command = strtok(NULL," ");
						strcpy(sendData,command);
						send(ClientSocket,sendData,strlen(sendData),0);
						usleep(1);
						while(1)
						{
							b = recv(ClientSocket,recvData,1024,0);
							recvData[b] = '\0';
							if(strcmp(recvData,"EOF") == 0)
								break;
							fwrite(recvData,1,b,stdout);
						}
						//printf("Done..\n");
						continue;
					}
				}
				else if(!strcmp(command,"FileDownload"))
				{
					bzero(sendData,1024);
					strcpy(sendData,command);
					printf("%s\n",sendData );
					send(ClientSocket,sendData,strlen(sendData),0);
					sleep(1);
					command = strtok(NULL," ");
					printf("c %s\n", command);
						strcpy(sendData,command);
						send(ClientSocket,sendData,strlen(sendData),0);
						sleep(1);

						FILE *fp;
						printf("%s\n",command );
						fp = fopen(command,"w");
						if (fp == NULL)
						{
							printf("file open failed\n");
						}
						while(1)
						{
							bzero(recvData,1024);
							b = recv(ClientSocket,recvData,1024,0);
							recvData[b] = '\0';
							//printf("recvData: -- %s............................\n",recvData );
							if(strcmp(recvData,"EOF")==0)
							{
								printf("yess\n");
								break;
							}
							fprintf(fp,"%s",recvData);
						}
						printf("File received\n");
						fclose(fp);
						continue;

				}
				else if(!strcmp(command,"FileUpload"))
				{
					bzero(sendData,1024);
				}
				else if(!strcmp(command,"FileHash"))
				{
					//printf("***\n");
					command = strtok(NULL," ");
					//printf("command %s..\n",command );
					if(!strcmp(command,"--verify"))
					{
						//printf("*\n");
						char *s;
						s = "HashVerify";
						bzero(sendData,1024);
						strcpy(sendData,s);
						send(ClientSocket,sendData,strlen(sendData),0);
						sleep(1);

						command = strtok(NULL," ");
						if(command == NULL)
						{
							printf("Usage : FileHash --verify <filename>\n");
							continue;
						}
						else
						{
							strcpy(sendData,command);
							send(ClientSocket,sendData,strlen(sendData),0);
							usleep(1);
							while(1)
							{
								b = recv(ClientSocket,recvData,1024,0);
								recvData[b] = '\0';
								if(strcmp(recvData,"EOF") == 0)
									break;
								fwrite(recvData,1,b,stdout);
							}
							//printf("Done..\n");
							continue;
						}
					}	
					else if(!strcmp(command,"--checkall"))
					{
						;
					}
				}
			}
		}
		close(ClientSocket);
	}
	return;
}

void server()
{
	if (choice == 0)
	{
		int listenSocket = socket(AF_INET,SOCK_STREAM,0);
		if(listenSocket<0)
		{
			printf("ERROR WHILE CREATING A SOCKET\n");
			return ;
		}

		struct sockaddr_in serv_addr;
		bzero((char *) &serv_addr,sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;	//For a remote machine
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(5001);

		if(bind(listenSocket,(struct sockaddr * )&serv_addr,sizeof(serv_addr))<0)
			printf("ERROR WHILE BINDING THE SOCKET\n");

		if(listen(listenSocket,10) == -1)	//maximum connections listening to 10
		{
			printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
		}

		int connectionSocket;
		while((connectionSocket=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0);
		//printf("connected\n");

		char data[1024],sendData[1024];
		int bytes,sent,b;
		
		while(1)
		{
//			printf("entered\n");
			bytes = recv(connectionSocket,data,1024,0);
			data[bytes] = '\0';
			//printf("data is %s\n",data );
			if (!strcmp(data,"EXIT"))
			{
				char s[] = "EXIT";
				strcpy(sendData,s);
				write(connectionSocket,sendData,1024);
				kill(0,SIGKILL);
				printf("exiting..\n");
				break;
			}
			if (!strcmp(data,"LongList"))
			{
				system("find . -printf '%p %TY%Tm%Td%TH%Tm%Tm %k \n' > longlist");
				//printf("Finding Data .. \n");
				FILE *fp = fopen("longlist","r");
				if(fp == NULL)
				{
					printf("wrong file\n");
					exit(1);
				}
				bzero(sendData,1024);
				while(!feof(fp))
				{
					b = fread(sendData,1,1024,fp);
					sendData[b] = '\0';
					sent = write(connectionSocket,sendData,1024);
				}
				printf("EOF\n");
				bzero(sendData,1024);
				char done[] = "EOF";
				strcpy(sendData,done);
				write(connectionSocket,sendData,1024);
				system("rm longlist");
				fclose(fp);
				printf("send data\n");
			}
			if (!strcmp(data,"ShortList"))
			{
				//printf("*\n");
				char init[1024],final[1024];
				b = recv(connectionSocket,data,1024,0);
				data[b] = '\0';
				strcpy(init,data);
				printf("%s\n",init );
				b = recv(connectionSocket,data,1024,0);
				data[b] = '\0';
				strcpy(final,data);
				printf("%s\n",final );

				system("ls -l --time-style=+%Y%m%d%H%M%S -t > list");
				system("tail -n +2 list > shortlist");
				FILE *fp1,*fp2;
				fp1 = fopen("shortlist","r");
				fp2 = fopen("new","w");
				char *line;
				size_t len = 0;
				ssize_t read;
				while((read = getline(&line,&len,fp1)) != -1)
				{
					printf("%s",line);
					strcpy(data,line);
					char *buffer;
					int i;
					buffer = strtok(data," ");
					for (i = 0; i < 5; ++i)
					{
						buffer = strtok(NULL," ");
						printf("%s\n",buffer );
					}

					if (strcmp(buffer,init) >=0 && strcmp(buffer,final) <=0)
					{
						fprintf(fp2, "%s",line );						
					}

					//fprintf(fp2, "%s",line );
					// b = write(fp2,line,sizeof(line));
					// if (b != len)
					// {
					// 	printf("no\n");
					// }
					//fwrite(data,1,sizeof(line),fp2);
				}
				fclose(fp2);
				fp2 = fopen("new","r");
				while(!feof(fp2))
				{
					b = fread(sendData,1,1024,fp2);
					sendData[b] = '\0';
					send(connectionSocket,sendData,strlen(sendData),0);
				}
				printf("EOF\n");
				bzero(sendData,1024);
				char done[] = "EOF";
				strcpy(sendData,done);
				send(connectionSocket,sendData,strlen(sendData),0);

				printf("done !!\n");
				fclose(fp1);
				system("rm list shortlist new");
				fclose(fp2);
			}
			if (!strcmp(data,"Regex"))
			{
				//printf("*\n");
				char regex[1024];
				bzero(regex,1024);
				bzero(data,1024);
				b = recv(connectionSocket,data,1024,0);
				strcpy(regex,data);
				char command[1024];
				strcpy(command,"find . -name \"");
				strcat(command,regex);
				strcat(command,"\" > regex");
				printf(" command : %s\n",command );
				system(command);
				
				FILE *fp;
				fp = fopen("regex","r");
				while(!feof(fp))
				{
					bzero(sendData,1024);
					b = fread(sendData,1,1024,fp);
					sendData[b] = '\0';
					send(connectionSocket,sendData,strlen(sendData),0);
				}
				printf("EOF\n");
				bzero(sendData,1024);
				char done[] = "EOF";
				strcpy(sendData,done);
				send(connectionSocket,sendData,strlen(sendData),0);
				system("rm regex");
				fclose(fp);
			}
			if (!strcmp(data,"FileDownload"))
			{
					bzero(data,1024);
					b = recv(connectionSocket,data,1024,0);
					//printf("data is ..%s..",data );
					data[b] = '\0';
					char *file;
					file = data;

					FILE *fp;
					fp = fopen(file,"r");
					if (fp == NULL)
					{
						printf("File opening failed\n");
						return;
					}
					while(!feof(fp))
					{
						bzero(sendData,1024);
						b = fread(sendData,1,1024,fp);
						sendData[b] = '\0';
						printf("%s\n", sendData);
						write(connectionSocket,sendData,1024);
						// send(connectionSocket,sendData,strlen(sendData),0);
						// sleep(1);
					}
					fclose(fp);
					bzero(sendData,1024);
					char done[] = "EOF";
					strcpy(sendData,done);
					//send(connectionSocket,sendData,strlen(sendData),0);
					write(connectionSocket,sendData,1024);
					printf("Done\n");
			}
			if(!strcmp(data,"HashVerify"))
			{
				char cs[1024],lm[1024];
				bzero(cs,1024);
				bzero(data,1024);
				b = recv(connectionSocket,data,1024,0);
				data[b] = '\0';
				strcpy(cs,"md5sum ");
				strcat(cs,data);
				strcat(cs," | awk '{print $1}' > hash");
				bzero(lm,1024);
				strcpy(lm,"stat ");
				strcat(lm,data);
				strcat(lm," | grep Modify | awk '{print $3}' >> hash");
				system(cs);
				system(lm);

				FILE *fp1;
				fp1 = fopen("hash","r");
				if(fp1 == NULL)
				{
					printf("file open failed\n");
					continue;
				}
				while(!feof(fp1))
				{
					bzero(sendData,1024);
					b = fread(sendData,1,1024,fp1);
					sendData[b] = '\0';
					write(connectionSocket,sendData,strlen(sendData),0);
				}
				printf("EOF\n");
				bzero(sendData,1024);
				char done[] = "EOF";
				strcpy(sendData,done);
				send(connectionSocket,sendData,strlen(sendData),0);
				system("rm hash");
				fclose(fp1);
			}
		}

		close(listenSocket);
	}
	return;
}
