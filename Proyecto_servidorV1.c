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
	                                        // INICIALITZACIONES
	// Abrir el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
		
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	                                       // establecemos el puerto de escucha
	serv_adr.sin_port = htons(9070);
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
	                              // bucle infinito
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
				
		int terminar = 0;
		while(terminar==0){
		
			// Ahora recibimos la peticion
			ret=read(sock_conn,peticion, sizeof(peticion));
			printf ("Recibido\n");
			
			peticion[ret]='\0';
			printf ("Peticion: %s\n", peticion);
			
			// Atendemos peticiones segun el codigo asignado
			char *p = strtok( peticion, "/");
			int codigo =  atoi (p);
			
			printf("%d \n",codigo);
			p = strtok( NULL, "/");
			char consulta[500]; //Donde guardaremos la consulta
			
			if(codigo==0) 	
				terminar=1;
			
			else if(codigo==1){                       	//Registro
				
				char nom[20];
				char pword[20];
				
				strcpy (nom, p);
				printf("%s \n", nom);
				p = strtok(NULL, "/");
				strcpy (pword, p);
				printf("%s \n", pword);
				
				int num;
				num = atoi(respuesta);
				if(num == 0)
				{
					sprintf(consulta, "INSERT INTO Jugador VALUES (NULL,'%s','%s');",nom, pword);
					printf("%s\n", consulta);

					err=mysql_query (conn, consulta);
     				if (err!=0) 
					{
						printf ("Error al consultar datos de la base %u %s\n",
								mysql_errno(conn), mysql_error(conn));
						sprintf (respuesta,"1\n");
						exit (1);
					}
/*					
					/*int num;*/
/*					num = atoi(respuesta);
*//*					if(num == 1){*/
/*						printf("Los datos introducidos no son correctos \n");*/
/*					}*/
/*					else	*/
/*					   printf("Usuario creado correctamente \n");*/
/*					return 1;*/
				} printf("%s",consulta);
			}
			
			else if(codigo==2){	                       //Iniciar sesion
				
				char nom[20];
				char pword[20];
				
				strcpy (nom, p);
				printf("%s \n", nom);
				p = strtok(NULL, "/");
				strcpy (pword, p);
				printf("%s \n", pword);
				
				strcpy(consulta, "SELECT COUNT(nom) FROM Jugador WHERE Jugador.nom ='");
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
					while (row !=NULL) 
				    {
						sprintf (respuesta,"%s \n", row[0]);
						row = mysql_fetch_row (resultado);
					}
					int num;
					num= atoi(respuesta);
					if(num == 1){
						printf("Usuario iniciado correctamente \n");
					}
					else	
					   printf("Los datos introducidos no son correctos \n");
			}
			
			else if (codigo==3){                      //Jugador con mas victorias
										
				err=mysql_query (conn,"SELECT Jugador.nom FROM Jugador WHERE Jugador.ganadas = (SELECT MAX(Jugador.ganadas) FROM Jugador);");
				printf("%d\n", err);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n",
							mysql_errno(conn), mysql_error(conn));
					exit (1);
				}
				resultado = mysql_store_result (conn);
				
				row = mysql_fetch_row (resultado);
				
				if (row == NULL)
					sprintf (respuesta,"No se han obtenido datos en la consulta\n");
				else
					while (row !=NULL)
				    {
						
						sprintf (respuesta,"%s\n", row[0]);
						// obtenemos la siguiente fila
						row = mysql_fetch_row (resultado);
				    }
			}
			
			else if (codigo ==4){                    //Partida mas larga
			
				
				err=mysql_query (conn,"SELECT MAX(Partida.duracion) FROM Partida");
				printf("%d\n", err);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n",
							mysql_errno(conn), mysql_error(conn));
					exit (1);
				}
				resultado = mysql_store_result (conn);
				row = mysql_fetch_row (resultado);
				
				if (row == NULL)
					sprintf (respuesta,"No se han obtenido datos en la consulta\n");
				else
					while (row !=NULL) 
					{
						// la columna 0 contiene el nombre del jugador
						sprintf (respuesta,"%s\n", row[0]);
						// obtenemos la siguiente fila
						row = mysql_fetch_row (resultado);
					}
			}
			
			else if(codigo==5){         //partidas ganadas del ganador introducido por teclado
			   
				char ganador[20];
				printf("Codigo: %d\n", codigo);
				strcpy(ganador,p);
				
								
				strcpy(consulta, "SELECT Jugador.ganadas FROM Jugador WHERE Jugador.nom ='");
				strcat(consulta, ganador);
				strcat(consulta, "'");
				/*printf("%s\n", consulta);*/
				//nos aseguramos poder hacer la consulta
				
				err=mysql_query (conn, consulta);
				printf("%d\n", err);
				if (err!=0) {
					printf ("Error al consultar datos de la base %u %s\n",
							mysql_errno(conn), mysql_error(conn));
					exit (1);
				}
				//recogemos el resultado de la consulta
				resultado = mysql_store_result (conn);
				// El resultado es una estructura matricial en memoria
				row = mysql_fetch_row (resultado);
				
				if (row == NULL)
					sprintf (respuesta,"No se han obtenido datos en la consulta\n");
				else
					while (row !=NULL) {
						// la columna 0 contiene el nombre del jugador
						sprintf (respuesta, "%s\n", row[0]);
						// obtenemos la siguiente fila
						row = mysql_fetch_row (resultado);
				}
					
			}
			
			
			
			printf ("Respuesta: %s\n", respuesta);
			// Enviamos la respuesta
			write (sock_conn,respuesta, strlen(respuesta));
		}
		
		close(sock_conn);                       // Cerramos la conexion
	}
}
