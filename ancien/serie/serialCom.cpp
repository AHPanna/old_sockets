#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
//#include <serial.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringSerial.h>

#include <iostream>
using namespace std;


int main(void)
{

	int port_serie;
	struct termios options;
	int reception;
	char buffer[20] ="";//"e1234r5678f123v644";
	//string data = "e1111r1111v100f100"; //message que tu veux envoyer
	bool stop = false;


	port_serie = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);//lecture et ecriture | pas controlling terminal | ne pas attendre DCD

	//cas d'erreur d'ouverture
	if(port_serie < 0)
	{
		perror("Erreur d'ouverture du port serie");
		exit(-1);
	}

	else
	{
		printf("Port serie numero %d bien ouvert. \n", port_serie);

		//chargement des données
		tcgetattr(port_serie, &options);
		//B57600 bauds
		cfsetospeed(&options, B9600); //B115200
		options.c_cflag |= (CLOCAL | CREAD);//programme propriétaire du port
		//structure en 8N1 !!
		options.c_cflag &= ~PARENB; //pas de parité
		options.c_cflag &= ~CSTOPB; // 1 bit de stop
		options.c_cflag &= ~CSIZE; //option a 0
		options.c_cflag |= CS8; //8 bits
		tcsetattr(port_serie, TCSANOW, &options); //enregistrement des valeurs de configuration
		printf("Configuration OK strcuture en 8N1 !. \n");
		cout << "Entrez \":exit\" pour quitter la communication." << endl;
		while(!stop){
			//cout << "Donnees a envoyer : ";
			//cin.getline(data, 20);

			//if(strcmp(data, ":exit") == 0) {
			//	stop = true;
			//}
			//else {		//ecriture
				
				//if (write(port_serie, data, sizeof(data)) < 0){perror("ERROR\n");}
				if (write(port_serie, data,20) < 0){perror("ERROR\n");}
				cout << "En attente de la reponse... " << endl;
				sleep(1);
					//lecture
				fcntl(port_serie,F_SETFL,10);//mode bloquant pour la fonction read
				reception=read(port_serie,buffer,20);//buffer = donnees; 20 = nb octets
				cout << "Donnees recues : " << buffer << endl;

				if (reception == -1)
                       		{printf("Erreur lecture port serie\n");}
			
		}
		cout << "Deconnexion du port" << endl;

	}

	close(port_serie);//fermeture du port serie

	return 0;
}
