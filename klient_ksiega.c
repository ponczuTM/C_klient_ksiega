/* klient Ksiegi skarg i wnioskow */
#define _WITH_GETLINE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<pwd.h>
#define MY_MSG_SIZE 64
key_t shmkey;
int   shmid;

struct my_data {
    int  typ;
    int size;
    char txt[MY_MSG_SIZE];
    char name[MY_MSG_SIZE];
} *shared_data;

char   *buf = NULL;
size_t bufsize = MY_MSG_SIZE;


int main(int argc, char * argv[]) {

	int wpis = 0;
	int ile = 0;
	int wolne;
	int i;
	/* obsluga bledow */
	if (argc != 2)
	{
		printf("Blad! Podaj odpowiednia liczbe argumentow!\n");
		return 0;
	}
	else
	{
	shmkey = ftok(argv[1], 1);
	struct passwd *pwd;
	uid_t uid;
	uid = getuid();
	pwd = getpwuid (uid);
	/* obsluga bledow */
	if( (shmid = shmget(shmkey, 0, 0)) == -1 ) {
		printf(" blad shmget\n");
		exit(1);
	}
	shared_data = (struct my_data *) shmat(shmid, (void *)0, 0);
	/* obsluga bledow */
	if(shared_data == (struct my_data *)-1) {
		printf(" blad shmat!\n");
		exit(1);
	}
	
	for (i = 0; i < shared_data->size; i++)
	{
		if((shared_data+i)->typ == 1)
			ile++;

	}
	wolne = shared_data->size - ile;
	printf("Klient ksiegi skarg i wnioskow wita!\n[Wolnych %d wpisow (na %d)]\n", wolne, shared_data->size);
	
	if (wolne == 0)
	{
		printf("W ksiedze skarg i wnioskow nie ma juz wolnego miejsca!\n"); /*komunikat ten wyskoczy rowniez, jesli uzytkownik poda przy wywolaniu serwera jako argv[1] ciag znakow, ktory nie bedzie liczba, a atoi zwroci 0*/
		return 0;
	}
	else
	{
	printf("Napisz co ci lezy na watrobie:\n");
	getline(&buf, &bufsize, stdin);
	
	for (i = 0; i < shared_data->size; i++)
	{
		if((shared_data+i)->typ == 1)
			wpis++;
		else
			break;
	}
	
	
	/* petla ignorujaca biale  znaki, dzieki niej "pusty wpis" nie zostanie dodany do ksiegi */
	for(i = 0; i < strlen(buf); i++)
	{
    		if(buf[i] > 32)
   	 {
       		(shared_data+wpis)->typ = 1;
     		 
    	}	
	}
	
	buf[strlen(buf)-1] = '\0'; /* techniczne: usuwam koniec linii */
	/* przekazywanie komunikatu i nazwy uzytkownika do struktury */
	strcpy((shared_data+wpis)->txt, buf);
	strcpy((shared_data+wpis)->name, pwd->pw_name);
	printf("Dziekuje za dokonanie wpisu do ksiegi\n");
	}
	}
	shmdt(shared_data);
	
	return 0;

}
