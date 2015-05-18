#include <semaphore.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h> // O_CREAT

sem_t * sem;

int main() {

	char sem_name[] = "miSemaforo";
	char shm_path[] = "/dev/shm/sem.";
	char sem_path[20];
	strcpy(sem_path, shm_path);
	strcat(sem_path, sem_name);

	sem = sem_open(sem_name, O_CREAT, 0666, 0);

	chmod(sem_path, 0666);

	static struct timespec time_to_wait = {0, 0};

	for (;;) {

		time_to_wait.tv_sec = time(NULL) + 10;

		printf("Espero...\n");
		sem_timedwait(sem, &time_to_wait);
		printf("He salido\n");	
	}

	sem_close(sem);
	sem_unlink(sem_name);

}