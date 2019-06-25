#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "aleatorio.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <semaphore.h>

#define X 100
#define Y 100



typedef sem_t semaphore;

semaphore mutex;
semaphore empty;

void down (semaphore * s)
{
	sem_wait(s);
}
void up(semaphore * s){
	sem_post(s);
}


typedef enum
{
    pendiente,
    asignado,
    entregado
} estado;
typedef struct
{
    int x;
    int y;
} coordenada;
typedef struct
{
    int id;
    coordenada ubicacion;
} dron;
typedef struct
{
    char nombre[30];
    long telefono;
    char direccion[30];
    coordenada coordenada;
    estado stdo;
} destinatario;
int numPack = 0;
destinatario **espacio;

coordenada masCercano(dron);

void generarPaq();
coordenada generarCoordenada();
int finalizar;
destinatario **generar_matriz(size_t, size_t);
void free_matriz(destinatario **, size_t);

void * asignarDron(void *);

int main(int argc, char *argv[])
{
	finalizar=0;

    srand(time(NULL));

    numPack = atoi(argv[1]);

    if (X * Y < numPack)
    {
        fprintf(stderr, "El numero de paquetes es muy grande.");
        exit(EXIT_FAILURE);
    }
    sem_init(&mutex,0,1);
    sem_init(&empty,0,numPack);
    espacio = generar_matriz(X, Y);
    generarPaq();
    dron d;
    dron d2;
    d.id=1;
    d.ubicacion.x=12;
    d.ubicacion.y=15;
    d2.id=2;
    d2.ubicacion.x=40;
    d2.ubicacion.y=12;

	pthread_t dron1;
	pthread_t dron2;
	pthread_t dron3;
	pthread_create(&dron1,NULL,asignarDron,1);
	pthread_create(&dron3,NULL,asignarDron,3);
	pthread_create(&dron2,NULL,asignarDron,2);

	


	sleep(10);
	printf("despues de sleep \n");

	finalizar=1;



	pthread_join(dron1,NULL);
	pthread_join(dron3,NULL);
	pthread_join(dron2,NULL);
	free_matriz(espacio,X);
    exit(EXIT_SUCCESS);
}
void * asignarDron(void *argdron)
{
	coordenada co;
	co.x=0;
	co.y=0;
    dron d;
    d.id = (int)argdron;
    d.ubicacion = co;
   printf("iniciado dron %d\n",d.id);
   while(!finalizar)
    {
    	down(&mutex);	//semaforo
	printf("sesion critica\n");
	//sleep(5);
	//down(&empty);	
    	d.ubicacion = masCercano(d); //seccion critica
    	espacio[d.ubicacion.x][d.ubicacion.y].stdo = asignado;
    	up(&mutex); //semaforo

    //printf("mas cer: %d-%d\n",d.ubicacion.x,d.ubicacion.y);
    //printf("%d-",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.x);
    //printf("%d\n",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.y);
    	sleep(rand()%5);

    	printf("paquete entregado a el cliente: %s por el dron %d\n",espacio[d.ubicacion.x][d.ubicacion.y].nombre,d.id);
    	espacio[d.ubicacion.x][d.ubicacion.y].stdo = entregado;
   }
    //espacio[d.ubicacion.x][d.ubicacion.y].stdo = entregado;
    //printf("%d-",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.x);
    //printf("%d\n",espacio[d.ubicacion.x][d.ubicacion.y].coordenada.y);
}

coordenada masCercano(dron d)
{
    double distancia = 0.0;
    coordenada co;
    double menor = X * Y;
    double suma;
    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Y; j++)
        {
            if (strstr(espacio[i][j].nombre,"Cliente")!=NULL)
            {
                if (espacio[i][j].stdo == 0)
                {
                    suma = pow(d.ubicacion.x - i, 2) + pow(d.ubicacion.y - j, 2);
                    distancia = sqrt(suma);
                    if (distancia < menor)
                    {
                        menor = distancia;
                        co.x = i;
                        co.y = j;
                        //printf("dentrocercano: %d-%d\n",co.x,co.y);
                       
                    }
                }
            }
        }
    }
    printf("distancia %lf  del dron %d con coordenadas %d-%d ",distancia,d.id,co.x,co.y);
    return co;
}
coordenada generarCoordenada()
{
    coordenada co;
    co.x = aleatorio();
    co.y = aleatorio();
    if (co.x==0&&co.y==0)
    {
        return generarCoordenada();
    }
    
    return co;
}
void generarPaq()
{
    if (espacio != NULL)
    {
        //TODO
        char cliente[30];
        destinatario paq;
        for (int i = 0; i < numPack; i++)
        {
            //sprintf(cliente,"Cliente: %i",i);
            // puts(cliente);
            strcpy(paq.nombre, "Cliente");
            paq.coordenada.x = generarCoordenada().x;
         paq.coordenada.y = generarCoordenada().y;
         
            paq.telefono = rand();
            paq.stdo = pendiente;
            //printf("coor: %d -%d\n ",espacio[paq.coordenada.x][paq.coordenada.y].coordenada.x,espacio[paq.coordenada.x][paq.coordenada.y].coordenada.y);
            //strcat(paq.nombre,itoa(i));
            espacio[paq.coordenada.x][paq.coordenada.y] = paq;
            //printf("coor: %d -%d\n ",espacio[paq.coordenada.x][paq.coordenada.y].coordenada.x,espacio[paq.coordenada.x][paq.coordenada.y].coordenada.y);
        }
    }
}
destinatario **generar_matriz(size_t n, size_t m)
{

    destinatario **aux;

    //Reservar memoria para n apuntadores (arreglos) a double
    aux = (destinatario **)malloc(X * sizeof(destinatario *));
    if (aux == NULL)
    {
        free(aux);
        return NULL;
    }
    for (int i = 0; i < n; i++)
    {
        aux[i] = malloc(m * sizeof(destinatario));
        if (aux[i] == NULL)
        {
            free_matriz(aux, i);
            return NULL;
        }
    }
    return aux;
}
/**
* Libera una matriz de n x m aleatorios 
* @param int n filas
* @param int m columnas
*/
void free_matriz(destinatario **m, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        free(m[i]);
    }
    free(m);
}
