#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4441
#define IP "192.168.0.182"
#define BUFFER_SIZE 4096
int main(){


	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[BUFFER_SIZE];


	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Erreur de connexion.\n");
		exit(1);
	}
	printf("[+]Client Socket est crée.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(IP);

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Erreur de connexion.\n");
		exit(1);
	}
	printf("[+]Connexion au serveur.\n");




	while(1){
		printf("Client: \t");
		scanf("%s", &buffer[0]);
		send(clientSocket, buffer, strlen(buffer), 0);

	

		if(strcmp(buffer, "q") == 0){
			close(clientSocket);
			printf("[-]Deconnexion.\n");
			exit(1);


		if(recv(clientSocket, buffer, BUFFER_SIZE, 0) < 0){
			printf("[-]Erreur Données non-recu..\n");
		}else{
			printf("Serveur : \t%s\n", buffer);


			send(clientSocket, buffer, strlen(buffer), 0);
			bzero(buffer, sizeof(buffer));

			}
		}
		
	}

	return 0;

}

//le probeleme avec ce code il doit enregistrer les les donner recu.
