#include <semaphore.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // O_CREAT
#include <sys/stat.h> // S_IROTH, S_IWOTH

sem_t * sem;

int main() {

	printf("Content-Type: text/html\n\n");

	char myName[] = "miSemaforo";

	int valor;

	sem = sem_open(myName, O_CREAT, 0666, 0);

	sem_getvalue(sem, &valor);
	printf("Voy a abrir...%d\n", valor);
	sem_post(sem);
	sem_getvalue(sem, &valor);
	printf("Abierto. %d\n", valor);

	sem_close(sem);

}