/*
	Aluno: Gabriel Junges Baratto
	RA: 2037840
	03/10/2021
	Implementação Trabalho 2.1 de Sistemas Operacionais
*/

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int i=0;
void sair(){

	printf("Exiting...\n");

	signal(SIGINT, SIG_DFL); // Restaura função original do sinal "SIGINT"
	kill(getpid(), SIGINT); // Executa um kill do tipo SIGINT para esta aplicação
	kill(getpid(), SIGKILL); // Executa um kill do tipo SIGKILL para esta aplicação

}
void alarmHandler(){

	printf("Timed-out (5 seconds)\n");

	sair(); // Chama a função 'sair'

	return;
}
void SIGINT_handler(){ // Tratamento Ctrl + C
	i++;

	if(i>=10){

		char exit;
		
		printf("\nReally exit (Y/n)?\n");

		alarm(5); // Seta um timer para tratamento de int. de 5 segundos

		while(scanf("%c",&exit) && exit!='n' && exit!='Y'){
			//apenas avança quando receber um 'n' ou 'Y'
		}
		
		if(exit=='n'){

			printf("Ok, you were spared this time...\n");

			alarm(0); // Cancela o timer do alarme
			i=0; // Reseta o contador de "Ctrl+C's"

			return; // Sai da rti
		}
		else if(exit=='Y'){

			printf("You chose to quit, well...\n");

			sair(); //Chama a função 'sair'

			return; // Sai da rti
		}
	}
}

void SIGTSTP_handler(){ // Tratamento Ctrl + Z

	printf("\nForam recebidos %d SIGINT\n",i); // Mostra quantos "Ctrl+C's" foram lidos pelo programa

}

int main() {

	signal(SIGINT, SIGINT_handler);
	signal(SIGTSTP, SIGTSTP_handler);
	signal (SIGALRM, alarmHandler);

	for(;;){
		//infinite loop
	}

}
  
