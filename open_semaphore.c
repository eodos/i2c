/* Este script cgi abre un semáforo con nombre */

#include <semaphore.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // O_CREAT

sem_t * sem;

int main() {

	char myName[] = "miSemaforo";

	printf("Content-Type: text/html\n\n");
	
	sem = sem_open(myName, O_CREAT, 0666, 0);

	sem_post(sem);

	sem_close(sem);

}