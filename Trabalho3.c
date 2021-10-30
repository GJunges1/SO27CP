/*
	Gabriel Junges Baratto
	Trabalho 3 de Sistemas Operacionais
	L05 - Prática de threads

	1a Fórmula utilizada para aproximação de pi:
	pi/4 = sum (-1)^n / (2n + 1) , n from 0 to infinite

	2a Fórmula utilizada para aproximação de pi:
	pi/4 = sum (-1)^n / (2n + 1) (1 / 2^(2n + 1) + 1 / 3^(2n + 1)) , n from 0 to infinite
*/

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>

#define NUM_THREADS 8
#define BILHAO 1000000000

double parc[NUM_THREADS];
double result=0;
void* calculo(void* i){
	long tid = (long)i;

	int termos = BILHAO/NUM_THREADS;
	int inicio = tid*termos;
	int fim = tid*termos + termos;

//Fórmula 1º Algoritmo:
	
/*
	for(int i=inicio; i<fim; i++){
		if(i%2==0)// i é par
			parc[tid] += 1.0/(2*i+1.0);
		else// i é impar
			parc[tid] -= 1.0/(2*i+1.0);
*/
	//Fórmula 2º Algoritmo:
	for(int i=inicio; i<fim; i++){
		if(i%2==0)// i é par
			parc[tid] += 1.0 / (2.0*i + 1) * (1.0 / pow(2,2*i + 1) + 1 / pow(3, 2*i + 1));
		else// i é impar
			parc[tid] -= 1.0 / (2.0*i + 1) * (1.0 / pow(2,2*i + 1) + 1 / pow(3, 2*i + 1));
	}

	pthread_exit(NULL);
}

int main(void){
	pthread_t threads[NUM_THREADS];

	for(int i=0; i<NUM_THREADS; i++)
		parc[i] = 0;
	for(int i=0; i<NUM_THREADS; i++)
		pthread_create(&threads[i], NULL, calculo, (void*)(intptr_t)i);
	for(int i=0;i<NUM_THREADS; i++){
		pthread_join(threads[i], NULL);
		result += parc[i];
	}
	result *= 4;
	
	printf("Aproximação para pi: %.20lf\n", result);
}
