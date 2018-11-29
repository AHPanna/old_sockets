#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 4440
#define IP "192.168.0.182"

int main(){
//partie modifier
	int sockfd, ret;
	 struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[4096];
	char db[4096]; // stockage de base recu via le buffer
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Erreur de connexion..\n");
		exit(1);
	}
	printf("[+]Client Socket est crée.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(IP);

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Erreur  binding.\n");
		exit(1);
	}
	printf("[+]Bind vers le port %d\n", PORT);

	if(listen(sockfd, 10) == 0){
		printf("[+]En Ecoute....\n");
	}else{
		printf("[-]Erreur en binding.\n");
	}


	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connexion accepter de %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(sockfd);
			int i = 0;
			while(i){
				recv(newSocket, buffer, 4096, 0);
				if(strcmp(buffer, ":Quitter") == 0){
					printf("Deconnexion de %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else{
					printf("%s\n", buffer);//Client: 
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
					db[i]=buffer[i];//copie du buffer au  au db 
					//printf("[+]buffer : %d %c \n",i,buffer[i]);
					
					printf("[+]db affiche db[%d]=%c",i,db[i]); //ce que db contient
					i++;
				}

			}
		}

	}


	//connexion à la base donnée.






	close(newSocket);


	return 0;
}
