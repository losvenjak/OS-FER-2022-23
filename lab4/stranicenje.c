#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include<unistd.h>

typedef struct Okvir {
	int zauzet;
	int proces;
	int stranica;
	int sadrzaj[64];
	int adresa;
}Okvir;	
typedef struct Disk {
	int stranica[16][64];
}Disk;	
typedef struct Tablica {
	int stranica[16][16];	
}Tablica;

int dekadski(int binarni[], int len)
{
	int dek = 0;
	for (int i = 0; i < len; i++)
		dek += binarni[i] * (int)pow(2, len - 1 - i);
	return dek;
}
int* binarni(int dek, int len)
{
	int *bin = (int *)malloc(len * sizeof(int));
	for (int i = len - 1; i >= 0; i--) {   
		bin[i] = dek % 2;
		dek = dek / 2;   
	}
	return bin;
}
int dohvatiFizickuAdresu(int p, int *logAdresa, int N, int M, Disk disk[]
, Okvir okvir[], Tablica tablica[], int ispis, int t, int zajednicki) {
	int pomak[6];
	int indeks[4];
	int fizAdresa[16];
	int* tBin = binarni(t, 5);
	for (int i = 0; i < 10; i++) {
		if (i < 4)
			indeks[i] = logAdresa[i];
		else
			pomak[i - 4] = logAdresa[i];
	}
    	for (int i = 0; i < 16; i++) {
    		if (i < 10)
    			fizAdresa[i] = tablica[p].stranica[dekadski(indeks, 4)][i];
    		else
    			fizAdresa[i] = pomak[i-10];
   	}    	
    	if (tablica[p].stranica[dekadski(indeks, 4)][10] == 0) {
    		printf("	Promasaj!\n");    		
    		for (int i = 0; i < M; i++) {
			if(i == M-1 && okvir[i].zauzet == 1) {    				
    				int lru;
    				int lruN;
    				int lruStranica;
    				int lruBin[5];
    				int lruOkvir;
    				int prva = 0;
    				for (int i = 0; i < N; i++) {    
    					for (int j = 0; j < 16; j++) {    					
    						if (tablica[i].stranica[j][10] == 1) {
    							if (j == 0) {
    								lruN = i;
    								lruStranica = j;
    								lru = 0;
    								i = N;
    								break;
    							}
    							for (int k = 11; k < 16; k++) {
    								lruBin[k - 11] = tablica[i].stranica[j][k];
    							}
    							if (prva == 0) {
    								lru = dekadski(lruBin, 5);
    								lruN = i;
    								lruStranica = j;
    								prva = 1;
    							}
    							else {
    								if (dekadski(lruBin, 5) < lru) {
    									lru = dekadski(lruBin, 5);
    									lruN = i;
    									lruStranica = j;
    								}
    							}    							
    						}
    					}
    				}    				
    				if (lruStranica != 0) {					// izbacujem stranicu (!= zajednicki spremnik) iz okvira
    					int* lruStrBin = binarni(lruStranica, 4);
    					int lruStrIspis[10];
    					tablica[lruN].stranica[lruStranica][10] = 0; 		// azuriranje tablice odgovarajuceg procesa za izbacenu stranicu
    					for (int i = 0; i < 10; i++) {
    						if (i < 4) {
    							lruStrIspis[i] = lruStrBin[i];
    						}
    						else {
    							lruStrIspis[i] = 0;
    						}
    					}
    					printf("		Izbacujem stranicu 0x%04x iz procesa %d\n", dekadski(lruStrIspis, 10), lruN);
    					printf("		lru izbacene stranice: 0x%04x\n", lru);
    				}    				   				
    				if (lruStranica == 0) {
    					printf("		Izbacujem okvir dijeljenog spremnika\n");	
    				}    				
    				for (int o = 0; o < M; o++) {
    					if (okvir[o].proces == lruN && okvir[o].stranica == lruStranica) {
    						lruOkvir = o;
    						printf("		dodijeljen okvir: 0x%04x\n", lruOkvir);
    						break;
    					}
    				}    				
    				if (lruStranica == 0) {  					// izbacujem zajednicki spremnik iz okvira
	    				int stariPomak = 0;
    					for (int j = 0; j < N; j++)
    						tablica[j].stranica[0][10] = 0;  		// azuriranje tablica svih procesa
    					for (int j = 0; j < 64; j++) {
    						if (okvir[lruOkvir].sadrzaj[j] != 0) {
    							stariPomak = j;
    							break;
    						}
    					}
    					for (int j = 0; j < (N - 1) * 2; j++)			// zapisivanje podataka zajednickog spremnika na njegovo mjesto na disk
    						disk[N].stranica[0][j] = okvir[lruOkvir].sadrzaj[j + stariPomak];
    				}    				
    				for (int pomak = 0; pomak < 64; pomak++) {
    					disk[lruN].stranica[lruStranica][pomak] = okvir[lruOkvir].sadrzaj[pomak];					
    					okvir[lruOkvir].sadrzaj[pomak] = 0;
    				}
    				okvir[lruOkvir].zauzet = 1;
    				okvir[lruOkvir].proces = p;
    				okvir[lruOkvir].stranica = dekadski(indeks, 4);
    				int* okvirAdr = binarni(okvir[lruOkvir].adresa, 10);
    				//int* tBin = binarni(t, 5);
    				for (int i = 0; i < 16; i++) {
    					if (i < 10) {
    						fizAdresa[i] = okvirAdr[i];
    						tablica[p].stranica[dekadski(indeks, 4)][i] = okvirAdr[i];
    					}
    					else if (i == 10) {
    						tablica[p].stranica[dekadski(indeks, 4)][i] = 1;
    					}
    					else {
    						tablica[p].stranica[dekadski(indeks, 4)][i] = tBin[i-11];
    					}
    				}
    				if (zajednicki == 1) {    				
    					for (int i = 0; i < N; i++) {
    						for (int j = 0; j < 16; j++) {
    							if (j < 10)
    								tablica[i].stranica[0][j] = okvirAdr[j];
    						}
    					}
    				}    				
    				okvir[lruOkvir].sadrzaj[dekadski(pomak, 6)] 
    				= disk[p].stranica[dekadski(indeks, 4)][dekadski(pomak, 6)];
    				if (zajednicki == 1) {
    					for (int j = 0; j < (N - 1) * 2; j++) {
    						okvir[lruOkvir].sadrzaj[j + dekadski(pomak, 6) - 2 * p + 2] 
    						= disk[N].stranica[0][j];
    					}
    				}    				
    				free(okvirAdr);
    				free(tBin);
    				break;
    			}
    			if (okvir[i].zauzet == 0) {
    				printf("		dodijeljen okvir 0x%04x\n", i);
    				okvir[i].zauzet = 1;
    				okvir[i].proces = p;
    				okvir[i].stranica = dekadski(indeks, 4);
    				int* okvirAdr = binarni(okvir[i].adresa, 10);
    				
    				for (int i = 0; i < 16; i++) {
    					if (i < 10) {
    						fizAdresa[i] = okvirAdr[i];
    						tablica[p].stranica[dekadski(indeks, 4)][i] = okvirAdr[i];
    					}
    					else if (i == 10) {
    						tablica[p].stranica[dekadski(indeks, 4)][i] = 1;
    					}
    					else {
    						tablica[p].stranica[dekadski(indeks, 4)][i] = tBin[i - 11];
    					}
    				}
    				okvir[i].sadrzaj[dekadski(pomak, 6)] 
    				= disk[p].stranica[dekadski(indeks, 4)][dekadski(pomak, 6)];    				
    				if (zajednicki == 1) {
    					 for (int j = 0; j < N; j++) {
    						for (int k = 0; k < 16; k++) {
    							if (k < 10) {
    								fizAdresa[k] = okvirAdr[k];
    								tablica[j].stranica[0][k] = okvirAdr[k];
    							}
    							else if (k == 10) {
    								tablica[j].stranica[0][k] = 1;
    							}
    							else {
    								tablica[j].stranica[0][k] = tBin[k - 11];
    							}
    						}
    					}
    					for (int j = 0; j < 64; j++) {
    						okvir[i].sadrzaj[j] = disk[N].stranica[0][j];
    					}
    				}
    				free(okvirAdr);
    				free(tBin);
    				break;
    			}
    		}
    	}
    	else {
    		for (int i = 0; i < 16; i++) {
    			if (i > 10)
    				tablica[p].stranica[dekadski(indeks, 4)][i] = tBin[i - 11];
		}
	}
    		
    	if (ispis == 1 && zajednicki == 0) {
    		printf("	fiz. adresa: 0x%04x\n", dekadski(fizAdresa, 16));
    		int zapisTablica[16];
    		for (int i = 0; i < 16; i++)
    			zapisTablica[i] = tablica[p].stranica[dekadski(indeks, 4)][i];
    		printf("	zapis tablice: 0x%04x\n", dekadski(zapisTablica, 16));
   	}
   	return dekadski(fizAdresa, 16);
}
int dohvatiSadrzaj (int p, int *logAdresa, int N, int M, Disk disk[]
, Okvir okvir[], Tablica tablica[], int t, int zajednicki)
{
	int fizAdresa = dohvatiFizickuAdresu(p, logAdresa, N, M, disk, okvir
	, tablica, 1, t, zajednicki);
	int vrijednost;
	int* fizAdr = binarni(fizAdresa, 16);
	int pomak[6];	
	for (int i = 10; i < 16; i++)
		pomak[i - 10] = fizAdr[i];
	for (int i = 0; i < M; i++) {
		if (okvir[i].adresa == dekadski(fizAdr, 10)) {
			vrijednost = okvir[i].sadrzaj[dekadski(pomak, 6)];
			break;
		}
	}
	free(fizAdr);
	return vrijednost;
}
void zapisiSadrzaj(int p, int *logAdresa, int vrijednost, int N, int M
, Disk disk[], Okvir okvir[], Tablica tablica[], int t, int zajednicki)
{
	int fizAdresa = dohvatiFizickuAdresu(p, logAdresa, N, M
	, disk, okvir, tablica, 0, t, zajednicki);
	int* fizAdr = binarni(fizAdresa, 16);
	int pomak[6];
	for (int i = 10; i < 16; i++)
		pomak[i - 10] = fizAdr[i];
	for (int i = 0; i < M; i++) {
		if (okvir[i].adresa == dekadski(fizAdr, 10)) {
			okvir[i].sadrzaj[dekadski(pomak, 6)] = vrijednost;
			break;
		}
	}
	free(fizAdr);
}
int main(int argc, char *argv[])
{
	int N, M;
	int* logickaAdresa[10];
	int dekadskaAdresa = 1;
	int t = 0;
	int citaj = 0;
	srand(time(NULL));
	if (argc != 3) {
		printf("Netočan broj argumenata!\n");
		return 0;
	}
	else {		
		N = atoi(argv[1]);
        	M = atoi(argv[2]); 
	}
	struct Disk disk[N+1];
	struct Okvir okvir[M];
	struct Tablica tablica[N];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < 16; j++) {			
			for (int k = 0; k < 64; k++) {
				if (k < 16)
					tablica[i].stranica[j][k] = 0;
				disk[i].stranica[j][k] = 0;
				if (i == N - 1) {
					disk[N].stranica[j][k] = 0;
				}
			}
		}
	}
	for (int i = 0; i < M; i++)  {
		for (int j = 0; j < 64; j++)
			okvir[i].sadrzaj[j] = 0;
		okvir[i].zauzet = 0;
		okvir[i].adresa = 0;
		for (int j = 0; j < i; j++)
    			okvir[i].adresa = okvir[i].adresa + 64;
	}
	while (1) {
		for (int i = 0; i < N; i++) {
			int zajednicki = 1;							//svaki proces prvo pristupa zajednickom spremniku
			printf("\n------------------------------------\n\n");
			printf("proces: %d\n", i);
			printf("	t: %d\n", t);			
			dekadskaAdresa = 12;							//odabrana log. adresa zajednickog spremnika (ista za sve procese)
			*logickaAdresa = binarni(dekadskaAdresa, 10);			
			printf("logicka adresa: 0x%04X (zajednicki spremnik)\n", dekadski(*logickaAdresa, 10));			
			if (i == 0) {
				citaj = 0;
				int gornja = 255;
				for (int j = 0; j < (N-1) * 2; j += 2) {
					int vrijednost = dohvatiSadrzaj(i, binarni(dekadskaAdresa + j, 10), N, M
					, disk, okvir, tablica, t, zajednicki);
					vrijednost = (rand() % (gornja) + 1);
					int* prviDio = binarni(vrijednost, 8);
					zapisiSadrzaj(i, binarni(dekadskaAdresa + j, 10), vrijednost, N, M
					, disk, okvir, tablica, t, zajednicki);
					vrijednost = dohvatiSadrzaj(i, binarni(dekadskaAdresa + j + 1, 10), N, M
					, disk, okvir, tablica, t, zajednicki);
					vrijednost = (rand() % (gornja) + 1);
					int* drugiDio = binarni(vrijednost, 8);
					zapisiSadrzaj(i, binarni(dekadskaAdresa + j + 1, 10), vrijednost, N, M
					, disk, okvir, tablica, t, zajednicki);
					int poruka[16];
					for (int i = 0; i < 16; i++) {
						if (i < 8) {
							poruka[i] = prviDio[i];
						}
						else {
							poruka[i] = drugiDio[i - 8];
						}
					}
					printf("	Poslao poruku: ");
					printf("	%X\n", dekadski(poruka, 16));
				}							
			}						
			else {
				int vrijednost = dohvatiSadrzaj(i, binarni(dekadskaAdresa + citaj, 10), N, M
				, disk, okvir, tablica, t, zajednicki);
				citaj++;
				int* prviDio = binarni(vrijednost, 8);
				vrijednost = dohvatiSadrzaj(i, binarni(dekadskaAdresa + citaj, 10), N, M
				, disk, okvir, tablica, t, zajednicki);
				citaj++;
				int* drugiDio = binarni(vrijednost, 8);
				int poruka[16];
				for (int i = 0; i < 16; i++) {
					if (i < 8) {
						poruka[i] = prviDio[i];
					}
					else {
						poruka[i] = drugiDio[i - 8];
					}
				}
				printf("	Primio poruku: ");
				printf("	%X\n", dekadski(poruka, 16));
			}
			
			/*while(dekadskaAdresa % 2 != 0) {					// odkomentirati za generiranje random adrese za svaki proces
				dekadskaAdresa = (rand() % (1023 - 64 + 1)) + 64;
				*logickaAdresa = binarni(dekadskaAdresa, 10);						
			}*/
			
			dekadskaAdresa = 1678;							// odabrana log. adresa kojoj pristupaju procesi
			*logickaAdresa = binarni(dekadskaAdresa, 10);
			printf("logicka adresa: 0x%04X\n", dekadski(*logickaAdresa, 10));
			zajednicki = 0;			
    			int vrijednost = dohvatiSadrzaj(i, *logickaAdresa, N, M
    			, disk, okvir, tablica, t, zajednicki);
    			printf("	sadrzaj adrese: %d\n", vrijednost);
    			vrijednost += 1;
    			zapisiSadrzaj(i, *logickaAdresa, vrijednost, N, M
    			, disk, okvir, tablica, t, zajednicki);
    			if (t == 31) {
    				t = 0;
   				for (int i = 0; i < N; i++) {
					for (int j = 0; j < 16; j++) {
						for (int k = 11; k < 16; k++) {
							tablica[i].stranica[j][k] = 0;
						}
					}
				}
			}
    			t++;
    			sleep(3);   
    			 			
    			//dekadskaAdresa = 1;    						// odkomentirati za generiranje random adrese za svaki proces
    		}
    	}    	
    	return 0;
}