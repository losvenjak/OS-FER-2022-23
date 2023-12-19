#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BR_KLIJENATA	7
#define BR_MJESTA	3
sem_t *ceka, *slobodna, *ulazi, *semprvi;
int *kraj, *trenutni, *j, *arr, *brLjudi, *spava;

void frizerka (int id)
{
	printf("Frizerka: Otvaram salon\n");
	printf("Frizerka: Postavljam znak OTVORENO\n");
	int tr = 0;
	int zatvoreno = 0;
	while (1) {	
		if(*kraj == 1 && zatvoreno == 0) {			
			printf("Frizerka: Postavljam znak ZATVORENO\n");
			zatvoreno = 1;						
		}		
		if(*brLjudi > 0) {		
			sem_post(slobodna);
			sem_wait(ceka);
			printf("Frizerka: Idem raditi na klijentu %d\n", *(arr + tr));
			sleep(3);
			printf("Frizerka: Klijent %d gotov\n", *(arr + tr));
			tr++;
			*trenutni = tr;
		}
		else {
			if(*kraj == 0) {
				printf("Frizerka: Spavam dok klijenti ne dođu\n");
				*spava = 1;
				sem_wait(semprvi);
				*spava = 0;
			}
			else {
				printf("Frizerka: Zatvaram salon\n");	
				break;
			}
		}		
	}
	exit(0);
}

void klijent (int id)
{
	if (*kraj == 0) {
		printf("	Klijent(%d): Želim na frizuru\n", id);
		sem_wait(ulazi);		
		if (*brLjudi >= BR_MJESTA) {
			printf("	Klijent(%d): Nema mjesta u čekaoni, vratit ću se sutra\n", id);
			sem_post(ulazi);
		}		
		else {					 	
			*brLjudi = *brLjudi + 1;
			if (*brLjudi == 1 && *spava == 1) 
				sem_post(semprvi);
			printf("	Klijent(%d): Ulazim u čekaonicu (%d)\n", id, *brLjudi);			
			arr[*j] = id;
			*j = *j + 1;
			sem_post(ulazi);				
			sem_post(ceka);			
			sem_wait(slobodna);
			*brLjudi = *brLjudi - 1;
			int tr = *trenutni;
			printf("	Klijent(%d): frizerka mi radi frizuru\n", *(arr + tr));	
		}		
	}
	exit(0);
}

int main ()
{
	int ID, i, id[BR_KLIJENATA];
	pid_t pid;

	ID = shmget (IPC_PRIVATE, sizeof(sem_t) + sizeof(int), 0600);
	ceka = shmat(ID, NULL, 0);
	shmctl(ID, IPC_RMID, NULL); 							

	kraj = (int *) (ceka + 1);
	slobodna = (sem_t *) (ceka + 2);
	trenutni = (int *) (ceka + 3);
	j = (int *) (ceka + 4);
	arr = (int *) (ceka + 5);
	ulazi = (sem_t *) (ceka + 55);
	brLjudi = (int *) (ceka + 56);
	semprvi = (sem_t *) (ceka + 57);
	spava = (int *) (ceka + 58);	

	sem_init(ceka, 1, 0);
	sem_init(slobodna, 1, 0);
	sem_init(ulazi, 1, 1);
	sem_init(semprvi, 1, 0);
	*kraj = 0;
	*trenutni = 0;
	*j = 0;
	*brLjudi = 0;
	int array[100];
	for (i = 0; i < 100; i++) {
		array[i] = 0;
	}
	*arr = array[0];

	for (i = 0; i < BR_KLIJENATA + 1; i++ ) {
		pid = fork();
		if (pid == 0) {
			id[i] = i;
			if(id[i] == 0) {
				frizerka (id[i]);
				sleep(1);
			}
			else if(id[i] != 6 && id[i] != 7 && id[i]!=8)
				klijent(id[i]);
			else {
				sleep(15);
				klijent(id[i]);
			}
			
		}
		else if (pid == -1) {
			perror("Greska pri stvaranju procesa");
			exit(1);
		}
	}

	sleep(25);	

	*kraj = 1;
	sem_post(semprvi);
	for (i = 0; i < BR_KLIJENATA + 1; i++ )
		wait(NULL);

	sem_destroy(ceka);
	shmdt(ceka);
	sem_destroy(slobodna);
	shmdt(slobodna);
	sem_destroy(ulazi);
	shmdt(ulazi);
	sem_destroy(semprvi);
	shmdt(semprvi);
	return 0;
}
