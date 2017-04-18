/*
Sistemas Operativos
Adan Rivera
Cesar Cornejo
2016 - 04
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <errno.h>

#define SIN_DIRECCION 0
#define DIRECCION_IZQ 1
#define DIRECCION_DER 2
/*Variables Globales*/
int TAMANO;
pthread_mutex_t *puente;
pthread_mutex_t mutexPuente = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexImprimir = PTHREAD_MUTEX_INITIALIZER;
int *valores;
int direccionGlobal = SIN_DIRECCION;
int carro = 0;
pthread_cond_t condicion = PTHREAD_COND_INITIALIZER;
pthread_cond_t semaforo = PTHREAD_COND_INITIALIZER;
int carrosIzq=0;
int carrosDer=0;
int cantidadCarros;
int contador=0;

////////////////////////////////////////////////////////////////////////
struct timespec tiempoEspera;
struct timeval actual;
int rt;

////////////////////////////////////////////////////////////////////////
//Inicia todo...
void inicializador() {
    int i;
    for (i = 0; i < TAMANO; i++) {
        pthread_mutex_init(&puente[i], NULL);
        valores[i] = -1;
    }

    gettimeofday(&actual,NULL);
    tiempoEspera.tv_sec = actual.tv_sec+5;
    tiempoEspera.tv_nsec = (actual.tv_usec+10UL*1000)*10UL;

}



void disenoPuente(int direccion) {

    system("clear");
    int i;
    fprintf(stderr, "\n--------------------------------------------------------------------------\n");
    if (direccion == 2) {
        for (i = 0; i < TAMANO; i++) {
            if (valores[i] == -1) {
                fprintf(stderr, "|\t");
            } else if (valores[i] == 0) {
                fprintf(stderr, " @-@>");
            }
        }
    } else {
        for (i = TAMANO - 1; i >= 0; i--) {
            if (valores[i] == -1) {
                fprintf(stderr, "|\t");
            } else if (valores[i] == 0) {
                fprintf(stderr, "<@-@ ");

            }
        }


    }
    fprintf(stderr, "\n Direccion= %d", direccionGlobal);
    fprintf(stderr, "\n Carros= %d\n",carro);
    sleep(1);

}
//Manejar1Fifo
void manejar(int direccion) {

    if (direccionGlobal != direccion) {
		pthread_mutex_lock(&mutexPuente);
		if(direccionGlobal==0){
			direccionGlobal = direccion;

			}else {
			 pthread_cond_wait(&condicion, &mutexPuente);
			 direccionGlobal = direccion;
			}
    }


    int i;
    for (i = 0; i < TAMANO; i++) {
		if(i==0){
			carro++;
			}
        disenoPuente(direccion);
        pthread_mutex_lock(&puente[i]);
        valores[i] = 0;
        if (i != 0) {
            pthread_mutex_unlock(&puente[i - 1]);
            valores[i - 1] = -1;
        }
    }
    valores[TAMANO - 1] = -1;
    carro--;
     pthread_mutex_unlock(&puente[TAMANO - 1]);
     pthread_mutex_unlock(&mutexPuente);
    if (valores[TAMANO - 1] == -1 && carro==0) {
        direccionGlobal = 0;
        pthread_mutex_unlock(&mutexPuente);
		pthread_cond_signal(&condicion);
        disenoPuente(direccion);



    }
}


// Manejar
void manejar2(int direccion) {

   if (direccionGlobal == 0) {
        pthread_mutex_lock(&mutexPuente);
        direccionGlobal = direccion;
        pthread_mutex_unlock(&mutexPuente);
    }
   if (direccionGlobal != direccion) {
       rt = pthread_cond_timedwait(&semaforo, &mutexPuente, &tiempoEspera);

    }
    int i;
    for (i = 0; i < TAMANO; i++) {

		if(i==0){
			carro++;
			}
        disenoPuente(direccion);

        pthread_mutex_lock(&puente[i]);
        valores[i] = 0;

        if (i != 0) {
            pthread_mutex_unlock(&puente[i - 1]);
            valores[i - 1] = -1;
        }


    }

    valores[TAMANO - 1] = -1;
    carro--;
    pthread_mutex_unlock(&puente[TAMANO - 1]);
    if (valores[TAMANO - 1] == -1 && carro==0) {

        disenoPuente(direccion);
        if(rt==ETIMEDOUT){
			if(direccionGlobal==1){
				direccionGlobal=2;
				pthread_mutex_unlock(&mutexPuente);
				pthread_cond_signal(&condicion);

	  }else if(direccionGlobal==2){
		  direccionGlobal=1;
		  pthread_mutex_unlock(&mutexPuente);
		  pthread_cond_signal(&condicion);
	  }
  }

    }


 


}
/*Movimiento a la izquierda*/
void* izquierda(void *data) {
       sleep(6);
	int caso=*((int*)data);
	if(caso==1){
    manejar(DIRECCION_IZQ);
  }
    else
    if(caso==2)
    {
      manejar2(DIRECCION_IZQ);
    }
   else
    return NULL;
}
/*Movimiento a la derecha*/
void* derecha(void *data) {
sleep(6);
int caso=*((int*)data);
		if(caso==1){
    manejar(DIRECCION_DER);}
    else
		if(caso==2)
    {
      manejar2(DIRECCION_DER);
    }
    return NULL;
}
/*Iniciar*/
int run(int oeste, int este,int caso) {
	direccionGlobal=1;


    inicializador();

    int i,n = oeste + este;
    pthread_t thread[n];

    float r;
	int m;
    float media;
     srand(time(NULL));
	int x=0;
	int y=0;
		while(x+y<n){
        r = (float) (rand() % 100)/100;
        media = -(3 * log(r));
		m=(int)(rand() % 2);

		if(x<oeste && m==1){
			sleep(media);
			x++;
			carrosIzq++;
			pthread_create(&thread[(x+y)-1], NULL,izquierda, &caso) ;
         }
        else if(y<este && m==0){
			sleep(media);
			y++;
			carrosDer;
			pthread_create(&thread[(x+y)-1], NULL,derecha, &caso) ;
        }
	}


   for (int i = 0; i < n; i++) {
        if (thread[i]) pthread_join(thread[i], NULL);
    }

/*Para Volver al menu*/
   // return EXIT_SUCCESS;
}
/*Main*/
int main(int argc, char** argv) {

  int oeste ,este ;
 TAMANO;

    int opc,salir=1;
    do{
      system("clear");
      printf("1 FIFO\n ");
      printf("2 Semaforo \n");
      printf(" 3 salir\n");
       printf("inserte una opcion:");
      scanf("%d",&opc);

      switch(opc)
      {
        case 1:
        {
        printf("ingrese la cantidad de la izquierda\n");
        scanf("%d\n",&oeste );
        printf("ingrese la cantidad de la derecha\n");
        scanf("%d\n",&este );
        printf("ingrese el tamano\n" );
        scanf("%d\n",&TAMANO);
        puente=malloc(sizeof(pthread_mutex_t)*TAMANO);
        valores=malloc(sizeof(int)*TAMANO);
        cantidadCarros=3;
         run(oeste, este,1);
        break;
      }
      case 2:
      {
        printf("ingrese la cantidad de la izquierda\n");
        scanf("%d\n",&oeste );
        printf("ingrese la cantidad de la derecha\n");
        scanf("%d\n",&este );
        printf("ingrese el tamano\n" );
        scanf("%d\n",&TAMANO);
        puente=malloc(sizeof(pthread_mutex_t)*TAMANO);
        valores=malloc(sizeof(int)*TAMANO);
        cantidadCarros=3;
        run(oeste, este,2);
        break;
      }
      case 3:
      salir=2;
      break;
      }
    }while(salir==1);







}

