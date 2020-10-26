#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>


int main(int argc, char *argv[])
{
	
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9000);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	MYSQL * conn;
	int err;
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "datos",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	int i;
	// Bucle infinito
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		
		int terminar =0;
		// Entramos en un bucle para atender todas las peticiones de este cliente
		//hasta que se desconecte
		while (terminar ==0)
		{
			// Ahora recibimos la petici?n
			ret=read(sock_conn,peticion, sizeof(peticion));
			printf ("Recibido\n");
			
			// Tenemos que a?adirle la marca de fin de string 
			// para que no escriba lo que hay despues en el buffer
			peticion[ret]='\0';
			
			
			printf ("Peticion: %s\n",peticion);
			
			// vamos a ver que quieren
			char *p = strtok( peticion, "/");
			int codigo =  atoi (p);
			printf("%d \n",codigo);
			p = strtok( NULL, "/");
			char consulta[500]; //Guardamos consulta
			
			//Empezamos con las consultas
					
			if (codigo ==0) //
				terminar=1;
			else if (codigo ==1)                          //Registro
			{
				char nom[20];
			    char pword[20];
				
				strcpy(nom,p);
				printf("%s\n", nom);
				p = strtok(NULL,"/");
				strcpy(pword,p);
				printf("%s\n",pword);
				
				int num;
				num = atoi(respuesta);
				char registrar[80];
				
				if (num==0)
				{
					strcpy(registrar,"INSERT INTO Jugador (id,nom,pword,ganadas,jugadas VALUES ('");
					strcat(registrar,"0,");
					strcat(registrar,nom);
					strcat(registrar,"',");
					strcat(registrar,pword);
					strcat(registrar,",0,0");
					err=mysql_query (conn, registrar);
					
					if(err!=0)
					{
						printf("Error al consultar la base de datos %u %s\n",
							  mysql_errno(conn),mysql_error(conn));
						sprintf (respuesta,"1\n");
						exit(1);
					}
					
					printf("%s", registrar);
					sprintf (respuesta,"0\n");
					int num;
					num = atoi(respuesta);
					
					if(num == 1)
					{
						printf("Los datos introducidos no son correctos \n");
					}
					else	
					   printf("Usuario creado correctamente \n");
				}
			}
				
	  
		else if (codigo ==2)                          //Loguaer
			{
			char nom[20];
			char pword[20];
				
			strcpy (nom, p);
			printf("%s \n", nom);
			p = strtok(NULL, "/");
			strcpy (pword, p);
			printf("%s \n", pword);
			
			strcpy(consulta, "SELECT COUNT(nom) FROM jugador WHERE Jugador.nom ='");
			strcat(consulta, nom);
			strcat(consulta, "' AND Jugador.pword = '");
			strcat(consulta, pword);
			strcat(consulta, "';");
			
			err=mysql_query (conn, consulta);
			if (err!=0)
			{
				printf ("Error al consultar datos de la base %u %s\n",
						mysql_errno(conn), mysql_error(conn));
			    exit (1);
			}
			resultado = mysql_store_result (conn);
			row = mysql_fetch_row (resultado);
			
			if (row == NULL)
				sprintf (respuesta,"No se han obtenido datos en la consulta\n");
			else
				while (row !=NULL) {
					sprintf (respuesta,"%s \n", row[0]);
					row = mysql_fetch_row (resultado);
			}
				int num;
				num = atoi(respuesta);
				if(num == 1){
					printf("Usuario iniciado correctamente \n");
				}
				else	
				   printf("Los datos introducidos no son correctos \n");
			}
		
		}// Se acabo el servicio para este cliente
		
		close(sock_conn); 
	}
}
