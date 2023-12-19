#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<math.h>
#define KAPACITET_BRODA 7
#define UVJET_MIN_PUTNIKA 3
#define KAPACITET_OBALE 20

pthread_mutex_t m;
pthread_cond_t red[1];

int kanibal = 0;
int misionar = 0;
int C[7]; //tko je u brodu, - misionari, + kanibali
int LO[KAPACITET_OBALE];
int DO[KAPACITET_OBALE];
int strana = 1; // 1 desno, 0 lijevo
int vozi = 0;

void ispisBrod()
{
	char c;
	int x;
	char str[3];
	for (int i = 0; i < KAPACITET_BRODA; i++) {
		if (C[i] != 0) {
			if (C[i] < 0)
				c = 'M';
			else 
				c = 'K';
			x = abs(C[i]);
			sprintf(str, "%c%d", c, x);
			printf("%s ", str);
		}    								
	}
	printf("\n");
}

void ispis()
{
	int prvi = 0;
	char c;
	int x;
	char str[3];
	if(strana == 1) c = 'D';
	else if(strana == 0) c = 'L';
	printf("C[%c]={", c);
	for (int i = 0; i < KAPACITET_BRODA; i++) {
		if (C[i] != 0) {
			if (C[i] < 0) 
				c = 'M';
			else 
				c = 'K';
			x = abs(C[i]);
			sprintf(str, "%c%d", c, x);
			if (prvi == 0) {
				printf("%s", str);
				prvi = 1;
			}
			else
				printf(" %s", str);    			
    		}
	}
	prvi = 0;
	printf("} ");
	printf("LO={");
	for (int i = 0; i < KAPACITET_OBALE; i++) {
		if (LO[i] != 0) {
			if (LO[i] < 0) 
				c = 'M';
			else 
				c = 'K';
			x = abs(LO[i]);
			sprintf(str, "%c%d", c, x);
    			if (prvi == 0) {
				printf("%s", str);
				prvi = 1;
			}
			else
				printf(" %s", str);
    		}
	}
	prvi = 0;
	printf("} ");
	printf("DO={");
	for (int i = 0; i < KAPACITET_OBALE; i++) {
		if (DO[i] != 0) {
			if (DO[i] < 0) 
				c = 'M';
			else 
				c = 'K';
			x = abs(DO[i]);
			sprintf(str, "%c%d", c, x);
    			if (prvi == 0) {
				printf("%s", str);
				prvi = 1;
			}
			else
				printf(" %s", str);
    		}
	}	
	printf("}\n\n");
}

void ukrcaj(int p, int obala) 
{
	char str[3];
	char c;
	int brKanibala = 0;				//u brodu
	int brMisionara = 0;
	
	while (vozi == 1 || obala != strana) {	//dok brod vozi, onda stop
		pthread_cond_wait(&red[0], &m);
	}
	for (int i = 0; i < 7; i++) {
		if (C[i] > 0 )
			brKanibala++;
		else if (C[i] < 0)
			brMisionara++;
	}    
    	if (p > 0) { 					//kanibal
    		c = 'K';
    		while (brKanibala + 1 > brMisionara && brMisionara != 0 
    		|| brKanibala + brMisionara >= 7) { 
			pthread_cond_wait(&red[0], &m);
		}		
	}
	else if (p < 0) { 				//misionar
		c = 'M';
		while (brKanibala > brMisionara + 1 || brKanibala + brMisionara >= 7) { 
			pthread_cond_wait(&red[0], &m);
		}		
	}
    	for (int i = 0; i < 7; i++) {
		if (C[i] == 0 ){			
			C[i] = p; 			//  m/k ušao u brod
			break;
		}
	}
	sprintf(str, "%c%d", c, abs(p));
	printf("%s: ušao u čamac\n", str);
	if (strana == 1) {
		for (int i = 0; i < KAPACITET_OBALE; i++) {
			if (DO[i] == p) { 
    				DO[i] = 0;
    				break;
    			}
    		}
	}
	else if (strana == 0) {
		for (int i = 0; i < KAPACITET_OBALE; i++) {
			if (LO[i] == p) { 
    				LO[i] = 0;
    				break;
    			}
    		}
	}
	ispis();	
	pthread_mutex_unlock(&m);
	pthread_exit(NULL);	
}

void *Misionar() 
{	
	pthread_mutex_lock(&m);
	int obala = (rand() % 2);
	misionar++;
	printf("M%d: došao na", misionar);
	if (obala == 0) {
		printf(" lijevu obalu\n");
		for (int i = 0; i < KAPACITET_OBALE; i++) {
			if (LO[i] != 0 && LO[i+1] == 0 && i!=KAPACITET_OBALE - 1){
			 	LO[i+1] = -misionar;
			 	break;
			 }
			 else if (i==KAPACITET_OBALE - 1){
			 	LO[0] = -misionar;
			 	break;
			 }
		}	
	ispis();
	}	
	else {
		printf(" desnu obalu\n");
		for (int i = 0; i < KAPACITET_OBALE; i++) {
			if (DO[i] != 0 && DO[i+1] == 0 && i!=KAPACITET_OBALE - 1){
			 	DO[i+1] = -misionar;
			 	break;
			 }
			 else if (i==KAPACITET_OBALE - 1){
			 	DO[0] = -misionar;
			 	break;
			 }
		}
	ispis();
	}
	ukrcaj(-misionar, obala);	
	return NULL;
}

void *Kanibal() 
{
	pthread_mutex_lock(&m);
	int obala = (rand() % 2);
	kanibal++;
	printf("K%d: došao na", kanibal);
	if (obala == 0) {
		printf(" lijevu obalu\n");
		for (int i = 0; i < KAPACITET_OBALE; i++) {
			if (LO[i] != 0 && LO[i+1] == 0 && i!=KAPACITET_OBALE - 1){
			 	LO[i+1] = kanibal;
			 	break;
			 }
			 else if (i== KAPACITET_OBALE - 1){
			 	LO[0] = kanibal;
			 	break;
			 }
		}
	ispis();		
	}	
	else {
		printf(" desnu obalu\n");
		for (int i = 0; i < KAPACITET_OBALE; i++) {
			if (DO[i] != 0 && DO[i+1] == 0 && i!=KAPACITET_OBALE - 1){
			 	DO[i+1] = kanibal;
			 	break;
			 }
			 else if (i==KAPACITET_OBALE - 1){
			 	DO[0] = kanibal;
			 	break;
			 }
		}
	ispis();
	}	
	ukrcaj(kanibal, obala);	
	return NULL;
}

void *Camac() 
{
	char c;
	int x;
	char str[3];
	printf("C: prazan na desnoj obali\n");
	ispis();
	while (1) { 
		int brojac = 0;
		for (int i = 0; i < KAPACITET_BRODA; i++) {
			if(C[i] != 0 ) brojac++;
		}
		if (brojac >= UVJET_MIN_PUTNIKA) {
			usleep(1);
			printf("C: tri putnika ukrcana, polazim za jednu sekundu\n");
			ispis();
			sleep(1);
			vozi = 1;
			if (strana == 1)
				printf("C: prevozim s desne na lijevu obalu: ");
			else
				printf("C: prevozim s lijeve na desnu obalu: ");
			ispisBrod();
			printf("\n");			
			sleep(2);
			if (strana == 1)
				printf("C: preveo s desne na lijevu obalu: ");
			else
				printf("C: preveo s lijeve na desnu obalu: ");
			ispisBrod();
			strana = 1 - strana;
			if (strana == 1)
				printf("C: prazan na desnoj obali\n");
			else
				printf("C: prazan na lijevoj obali\n");
			for (int i = 0; i < KAPACITET_BRODA; i++) 
				C[i] = 0;
			ispis();
			vozi = 0;		
			pthread_cond_broadcast(&red[0]);		
		}
	}
	return NULL;
}

void *PomocnaK() 
{	
	while(1) {
		pthread_t kanibal;
		pthread_create(&kanibal, NULL, Kanibal, NULL);
		sleep(1);		
	}
}
void *PomocnaM() 
{	
	while(1) {
		pthread_t misionar;
		pthread_create(&misionar, NULL, Misionar, NULL);
		sleep(2);	
	}
}

int main(void) 
{	
	int i;
	srand ((unsigned int) time(NULL));		
	for (i = 0; i < KAPACITET_OBALE; i++) {
		if (i < KAPACITET_BRODA) 
			C[i] = 0;
		LO[i] = 0;
		DO[i] = 0;
	}	
	printf("Legenda: M-misionar, K-kanibal, C-čamac,\n	LO-lijeva obala, DO-desna obala\n	L-lijevo, D-desno\n\n");	
		
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&red[0], NULL);	
	pthread_t t[3];     	
     	pthread_create(&t[0], NULL, Camac, NULL);
     	pthread_create(&t[1], NULL, PomocnaK, NULL);
     	pthread_create(&t[2], NULL, PomocnaM, NULL);     	
    	
    	for (i = 0; i < 3; i++) {
       	pthread_join(t[i], NULL);
    	}   	
    
	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&red[0]);	
	
	return 0;
}

