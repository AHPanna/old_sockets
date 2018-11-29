/*
Auteur : ABDUL HAKIM Panna
date : Fevrier - Juin 2018
Projet : Robot Pong 1040 <-Upgraded to new tech from old services
Twitter : @AH_Panna
github : @epion93
Classe : BTS SNIR 2
licence GNU

Projet Realiser par BARETTE Alexandra & ALAOUA Othmane & moi
Contenue du projet : Carte Raspberry 3 - Carte Arduino - Application Android

compilation avce mysql :
g++ -o server $(mysql_config --cflags) server.cpp $(mysql_config --libs)
ou g++ -o server server.cpp -lmysqlclient
ou : g++ -o server server.cpp sans mysql

client doit envoyer des données : u.1.z.11001001 <-- application android
u= user qui est l'utilisateur 1
z= zone 8 caracteres 11001001
f.10 //frequence
v.4 //vitesses
et envoyer "play" dans le soscket de l'arduino 

#pour toutes modif les données devront obligé d'etre sous la forme :
modif.f.{frequence}
modif.v.{vitesse}

envoyé a l'arduino [e1110r1002f12v3]

puis le client envoi:

u.1.z.23243435.v.10.f.08.debug

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <time.h>
#include <mysql/mysql.h>

#include <fcntl.h>
#include <termios.h>

#define PORT 4441
//#define IP "192.168.0.168"
#define IP "127.0.0.1"
#define BUFFER_SIZE 4096
//mysql log info
#define USER "user"
#define PASS "rootroot"
#define DB	"DB"


using namespace std;

//valeur global message recu via le buffer plutart concatner et envoi vers l'arduino. 
string zoneER,freq,vit;
string get_s;



//ideale pour 1 socket
struct arduino {
  string id; //contien l'identifiant
  string zone; //init zone 10110010
  string zone_e;//1011
  string zone_r;//0010
  string zone_ER;
  string freq;//stock la freqeunce init ou modif
  string vit; //stock la vitesse init ou modif
  string fin; //trame finale envoyé vers l'arduino
  string score;//contient le score finale
}p;

//bdd -> zone
void update_Z_F_V_zER_ID(string z, string F,string V,string zER, string ID){
   MYSQL *conn;
   char *server = IP;
   char *user = USER;
   char *password = PASS;
   char *database = DB;
   conn = mysql_init(NULL);
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   string query = "UPDATE temp SET zone='"+z+"',freq='"+F+"',vit='"+V+"',zoneE_R='"+zER+"',userID='"+ID+"' WHERE id=1";
   if (mysql_query(conn, query.c_str())) { //show tables
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   mysql_close(conn);
}
//bdd -> zoneER
void update_zoneER(string q){
   MYSQL *conn;
   char *server = IP;
   char *user = USER;
   char *password = PASS;
   char *database = DB;
   conn = mysql_init(NULL);
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   string query = "UPDATE user SET zoneE_R='"+q+"' WHERE id=1";
   if (mysql_query(conn, query.c_str())) { //show tables
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   mysql_close(conn);
}
//bdd -> freq
void update_freq(string q){
   MYSQL *conn;
   char *server = IP;
   char *user = USER;
   char *password = PASS;
   char *database = DB;
   conn = mysql_init(NULL);
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   string query = "UPDATE user SET freq='"+q+"' WHERE id=1";
   if (mysql_query(conn, query.c_str())) { //show tables
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   mysql_close(conn);
}
//bdd -> vit
void update_vit(string q){
   MYSQL *conn;
   char *server = IP;
   char *user = USER;
   char *password = PASS;
   char *database = DB;
   conn = mysql_init(NULL);
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   string query = "UPDATE user SET vit='"+q+"' WHERE id=1";
   if (mysql_query(conn, query.c_str())) { //show tables
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   mysql_close(conn);
}
//bdd ->recup fonction ultime trame pour arduino
string get_sql(){
	MYSQL *conn;
   MYSQL_RES *res;
   MYSQL_ROW row;
   
   char *server = IP;
   char *user = USER;
   char *password = PASS;
   char *database = DB;
   string var_zone,var_freq,var_vit;
   conn = mysql_init(NULL);
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   if (mysql_query(conn, "Select * from temp where id=1;")) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   res = mysql_use_result(conn);
   while ((row = mysql_fetch_row(res)) != NULL){
	 var_zone =row[4];
	 var_freq = row[2];
	 var_vit = row[3];
   }
  get_s=var_zone+"v"+var_vit+"f"+var_freq; //v{vitesse}f{frequence}
   
   mysql_free_result(res);
   mysql_close(conn);

   return get_s;//renvoi la trame
}
//bdd -> score
void update_score(string q){
   MYSQL *conn;
   char *server = IP;
   char *user = USER;
   char *password = PASS;
   char *database = DB;
   conn = mysql_init(NULL);
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   string query = "UPDATE temp SET score='"+q+"' WHERE id=1";
   if (mysql_query(conn, query.c_str())) { //show tables
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   mysql_close(conn);
}


//communication port serie ecriture et lecture
void serie(){
	int port_serie;
	struct termios options;
	int reception;
	string score;
	char buffer[2] = "";
	char data[2]="2";
	bool stop = false;
	//conversion de msg vers char data pour envoyer le message
	
	//char *data = new char[msg.length() + 1];
	//strcpy(data, msg.c_str());
	
	port_serie = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);//lecture et ecriture | pas controlling terminal | ne pas attendre DCD

	//cas d'erreur d'ouverture
	if(port_serie < 0){
		perror("Erreur d'ouverture du port serie");
		// exit(-1);
		return;
	}else{
		printf("Port serie numero %d bien ouvert. \n", port_serie);

		//chargement des données
		tcgetattr(port_serie, &options);
		cfsetospeed(&options, B9600); //Baud = 9600
		options.c_cflag |= (CLOCAL | CREAD);//programme propriétaire du port
		//structure en 8N1 !!
		options.c_cflag &= ~PARENB; //pas de parité
		options.c_cflag &= ~CSTOPB; // 1 bit de stop
		options.c_cflag &= ~CSIZE; //option a 0
		options.c_cflag |= CS8; //8 bits
		tcsetattr(port_serie, TCSANOW, &options); //enregistrement des valeurs de configuration
		printf("Configuration OK strcuture en 8N1 !. \n");
		//cout << "Entrez \":exit\" pour quitter la communication." << endl;
		while(!stop){
			if(strcmp(data, ":exit") == 0) {
				stop = true;
			}else {		//ecriture
				if (write(port_serie, data, sizeof(data)) < 0){perror("ERROR\n");}
				cout << "En attente de la reponse... " << endl;
				sleep(1);
					//lecture
				fcntl(port_serie,F_SETFL,10);//mode bloquant pour la fonction read
				reception=read(port_serie,buffer,20);//buffer = donnees; 20 = nb octets
				cout << "Donnees recues : " << buffer << endl;
				
				//stockage du score 
				
				//update_score(buffer);


				if (reception == -1){printf("Erreur lecture port serie\n");}
			}
		}
		cout << "Deconnexion du port" << endl;
	}
	close(port_serie);//fermeture du port serie
}


void play(char *buf){
	
	stringstream ss(buf);
   	string temp[10];//tab convert from vector
	vector<string> result;
   	while( ss.good()){
    string substr;
    getline( ss, substr, '.' );
    result.push_back( substr );}
	//copie du vecteur a tableau
	copy(result.begin(), result.end(), temp);	

	//fonction de debug
	if(temp[8]=="1"){
		
		//cout << "in if"<< endl;
		// string sql=get_sql();
		// cout<< "j'ai reuperer ceci: "<< sql <<endl;
		// serie(sql);
		// string score= temp[1];
		// update_score(score);
		p.id=temp[1];
		p.zone=temp[3];
		p.zone_e = temp[3].substr (0,4);//zone d'envoi
		p.zone_r = temp[3].substr (4,4);//zone recep
		p.vit=temp[5];
		p.freq=temp[7];
		zoneER="e"+temp[3].substr(0,4)+"r"+temp[3].substr(4,4);//prepare zone envoi et reception
		p.zone_ER=zoneER;
		update_Z_F_V_zER_ID(p.zone,p.freq,p.vit,p.zone_ER, p.id);
		
		
		cout<< " requete fait"<<endl;
		
		//string sql=get_sql();
		//p.fin=sql;
		cout << "requete vers l'arduino :"<< p.fin<<endl; 
				
		serie();
		
	}
	//message envvoyé u(temp[0]).user(temp[1]).z(temp[2]).{zone(temp[3])}.v(temp[4]).{vit(temp[5])}.f(temp[6]).{freq(temp[7])}.play(temp[8])
	if(temp[0]=="u"){
		p.id=temp[1];
		p.zone=temp[3];
		p.zone_e = temp[3].substr (0,4);//zone d'envoi
		p.zone_r = temp[3].substr (4,4);//zone recep
		p.vit=temp[5];
		p.freq=temp[7];
		//zone_ER="e"+temp[3].substr(0,4)+"r"+temp[3].substr(4,4);//prepare zone envoi et reception
		//update_Z_F_V_zER_ID(p.zone,p.freq,p.vit,p.zoneER, p.id);
			


		// p.zone=temp[3];//zone
		// p.id=temp[1];//id pour bdd
		
		// update_zone_user(p.zone,p.id);
		
		// update_zoneER(zoneER);//bdd
		
	}
	if(temp[0]=="f"){
		p.freq=temp[1];//la frequence
		freq=temp[1];
		update_freq(freq);//bdd
	}
	if(temp[0]=="v"){
		p.vit=temp[1];//la vitesse
		vit=temp[1];
		update_vit(vit);//bdd
	
	}
	//p.fin="e"+p.zone_e+"r"+p.zone_r+"v"+p.vit+"f"+p.freq;//envoi trame
	// cout<<"mon message : "<<p.fin<<endl;
	//execution de la partie
	 if(temp[0]=="play"){// ma fonction pour envoi complete de la trame
	 	cout<<"demarrer la partie"<<endl;
		string sql=get_sql();
		p.fin=sql;
		//serie(sql);
	 	
	  }
	  //modification {message : modif.f.{frequence} ou modif.v.{vittesse}}
	  if(temp[0]=="modif"){
		  if(temp[1]=="f"){
			p.freq=temp[2];
			update_freq(p.freq);
			cout<< " modif f ok "<< endl;
			string sql=get_sql();
			p.fin=sql;
			//serie(sql);
			  
		  }else if (temp[1]=="v"){
			p.vit=temp[2];
			update_freq(p.vit);
			cout << " modfi vit ok"<< endl;
		    string sql=get_sql();
			p.fin=sql;
		   // serie(sql);
		  }
	  }
	
}


int main(){


	
	
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[BUFFER_SIZE];
	char *str;

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
		printf("[+]Connexion accepter de %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		if((childpid = fork()) == 0){//fork() multi connexion socket
		close(sockfd);

			while(1){
			recv(newSocket, buffer, BUFFER_SIZE, 0); // did i receive something

				if(strcmp(buffer, "q") == 0){ //logout for client is "q"
					printf("[-]Deconnexion de %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else{
					//cout << " mon numero de processeur : "<< getpid()<< endl;
					
					printf("%s\n",buffer);//print my buffer init
					
					str = (char *) malloc((256));//new memory location for str

					memcpy(str, buffer, strlen(buffer)+1);// copy buffer to str
					bzero(buffer, sizeof(buffer)); //clean buffer
					
					play(str);//fonction play recup val et modif pour arduino et bdd


					sleep(5); //ajouter le temps pour stoper la variation d'envoi donnée int
					memset(str, 0, sizeof str);//after sending data refresh again the str array for sure
					free(str); //cleaning the memory location
					
				}
			}

		}
		close(newSocket);
	}
	close(newSocket);
	return 0;
}