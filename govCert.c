#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

typedef struct{
    int year;
    int month;
    int day;
}date;

typedef struct{
    char IP[16];
    int port;
}netData;

typedef struct ctzn{
    date birthday;
    netData terminal;
    char name[31];
    char surname[31];
    char phone[15];
    char mail[31];
    char cert[20];
    char serial[15];
    struct ctzn *next;
}citizen;

void flushStdin() {
    char c;
    while((c = getchar()) != '\n' && c != EOF);
}

int my_random(int max) {
    double d;
    int r;
    d = (double)rand() / ((double)RAND_MAX+1);
    d = d*max;
    r = (int)d;
    return r;
}

citizen *enque(citizen *list, citizen *new)//insert at end
{
    citizen *pointer = list;
    if (list == NULL) {
        list = new;
        return list;
    }
    while(pointer->next != NULL){
        pointer = pointer->next;
    }
    pointer->next = new;
    return list;
}

citizen *deque(citizen *list)//delete first
{
    citizen *pointer = list;
    if (list == NULL) return NULL;
    if (list->next == NULL) {
        free(list);
        return NULL;
    }
    list = list->next;
    free(pointer);
    return list;
}

int printCitizen(citizen *cit) {
	printf("%-30s %-30s %02d/%02d/%4d %12s %30s %15s:%d %15s %15s\n",
		cit->name, cit->surname, cit->birthday.day, cit->birthday.month,
		cit->birthday.year, cit->phone, cit->mail, cit->terminal.IP,
		cit->terminal.port, cit->cert, cit->serial);
	return 1;
}

//name validation
int validName(char *name) {
    int i = 0;
    if (strlen(name) > 30) return 0;
    while (i < strlen(name)) {
        if (isalpha(name[i]) == 0) return 0;
        name[i] = tolower(name[i]);
        i++;
    }
    name[0] = toupper(name[0]);
    return 1;
}

//birthday validation
int validDate(char *dateStr, date *birthday){
    if (strlen(dateStr) != 10) return 0;
    if (dateStr[2] != '/' || dateStr[5] != '/') return 0;
    if (sscanf(dateStr, "%2u/%2u/%4u", &(birthday->day), &(birthday->month), &(birthday->year)) != 3) return 0;
    if (birthday->day < 1 || birthday->day > 31 ||
        birthday->month < 1 || birthday->month > 12 || 
        birthday->year < 1900 || birthday->year > 1995) {
        return 0;
    }
    return 1;
}

//phone validation
int validPhone(char *phone) {
    int i = 0;
    char *dashptr = NULL;
    if (strlen(phone) != 11) {
        return 0;
    }
    dashptr = strchr(phone, '-');
    if (dashptr == NULL) return 0;
    if (strrchr(phone, '-') != dashptr) return 0;
    if (strlen(dashptr) != 7 && strlen(dashptr) != 8) return 0;
    while (isdigit(phone[i]) || phone[i] == '-') i++;
    if (i != 11) return 0;
    return 1;
}

//email validation
int validEmail(char *email) {
    char *ptr;
    char *dot_ptr;
    char *at_ptr;
    if (strlen(email)>30) return 0;
    if (isalpha(email[0]) == 0) {
        return 0;
    }
    at_ptr = strchr(email, '@');
    if (at_ptr == NULL) return 0;
    if (at_ptr - email < 4) return 0;
    ptr = email;
    while (ptr < at_ptr) {
        if (isalnum(*ptr) == 0) return 0;
        ptr++;
    }
    ptr++;
    while (*ptr != '\0') {
        if (isalnum(*ptr)==0 && (*ptr) != '.') return 0;
        ptr++;
    }
    dot_ptr = strrchr(email, '.');
    if (dot_ptr == NULL) return 0;
    if (strlen(dot_ptr) != 3) return 0;
    ptr = dot_ptr+1;
    while(*ptr != '\0') {
		if (isalpha(*ptr) == 0) return 0;
		ptr++;
	}
    return 1;
}

//ip validation
int validIp(char *ipstr, netData *data) {
    int ip[4], port, i;
    if (sscanf(ipstr, "%d.%d.%d.%d:%d", &ip[0], &ip[1], &ip[2], &ip[3], &port) != 5) return 0;
    for (i = 0; i<4; i++) {
        if (ip[i] > 255 || ip[i] <0) return 0;
    }
    if (port > 65535 || port < 1) return 0;
    sprintf(data->IP, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    data->port = port;
    return 1;
}

//cert validation
int validCert(char *certstr) {
	if (strcmp(certstr, "familyCert") == 0 ||
		strcmp(certstr, "birthCert") == 0 ||
		strcmp(certstr, "debtCert") == 0 ||
		strcmp(certstr, "residenceCert") == 0 ||
		strcmp(certstr, "electionCert") == 0) return 1;
    return 0;
}

//valid checksum
int validChecksum(char *checksum) {
	int n[11];
	int i = 0;
	int sum = 0;
	if (strlen(checksum) != 13) return 0;
	if (sscanf(checksum, "%1d%1d%1d%1d%1d-%1d%1d%1d%1d%1d-%1d",
		&n[0], &n[1], &n[2], &n[3], &n[4], &n[5], &n[6], &n[7], &n[8], &n[9], &n[10]) != 11)
		return 0;
	for (i=1; i<10; i+=2) {
		n[i] = n[i] * 2;
		n[i] = n[i]/10 + n[i]%10;
	}
	for (i=0; i<11; i++) sum += n[i];
	if (sum % 10 != 0) return 0;
    return 1;
}

//create checksum
void newChecksum(char *checksum) {
	int n[11];
	int temp[11];
	int i = 0;
	int sum = 0;
	//randoms
	for (i=0; i<10; i++) {
		n[i] = my_random(10);
		temp[i] = n[i];
	}
	for (i=1; i<10; i+=2) {
		temp[i] = temp[i] * 2;
		temp[i] = temp[i]/10 + temp[i]%10;
	}
	for (i=0; i<10; i++) sum += temp[i];
	n[10] = (sum*9)%10;
	sprintf(checksum, "%d%d%d%d%d-%d%d%d%d%d-%d",
		n[0], n[1], n[2], n[3], n[4], n[5], n[6], n[7], n[8], n[9], n[10]);
}

int newCitizen(citizen **list, char *str, char *error)
{
    citizen *nCitizen;
    char *token;
    date tbirthday;
    netData tterminal;
    char tname[31];
    char tsurname[31];
    char tphone[15];
    char tmail[31];
    char tcert[20];
    char tserial[15];

	//name
    token = strtok(str, ", \t");
    strcpy(error, "invalid name\n");
    if (token == NULL) return 0;
    if (validName(token) == 0) return 0;
    strncpy(tname, token, strlen(token)+1);
    //surname
    token = strtok(NULL, ", \t");
    strcpy(error, "invalid surname\n");
    if (token == NULL) return 0;
    if (validName(token) == 0) return 0;
    strncpy(tsurname, token, strlen(token)+1);
    //birthday
    token = strtok(NULL, ", \t");
    strcpy(error, "invalid birth date\n");
    if (token == NULL) return 0;
    if (validDate(token, &tbirthday) == 0) return 0;
    //phone
    token = strtok(NULL, ", \t");
    strcpy(error, "invalid phone number\n");
    if (token == NULL) return 0;
    if (validPhone(token) == 0) return 0;
    strncpy(tphone, token, strlen(token)+1);
    //e-mail
    token = strtok(NULL, ", \t");
    strcpy(error, "invalid e-mail\n");
    if (token == NULL) return 0;
    if (validEmail(token) == 0) return 0;
    strncpy(tmail, token, strlen(token)+1);
    //ip
    token = strtok(NULL, ", \t");
    strcpy(error, "invalid ip\n");
    if (token == NULL) return 0;
    if (validIp(token, &tterminal) == 0) return 0;
    //cert
    token = strtok(NULL, ", \t");
    strcpy(error, "invalid certification\n");
    if (token == NULL) return 0;
    if (validCert(token) == 0) return 0;
    strncpy(tcert, token, strlen(token)+1);
    //checksum
    token = strtok(NULL, ", \t");
    if (token == NULL || validChecksum(token) == 0) {
		newChecksum(tserial);
	}else 
	strncpy(tserial, token, strlen(token)+1);

	nCitizen = (citizen *)malloc(sizeof(citizen));
	if (nCitizen == NULL) {
		strcpy(error, "bad memory");
		return 0;
	}
	strncpy(nCitizen->name, tname, strlen(tname)+1);
	strncpy(nCitizen->surname, tsurname, strlen(tsurname)+1);
	nCitizen->birthday.day = tbirthday.day;
	nCitizen->birthday.month = tbirthday.month;
	nCitizen->birthday.year = tbirthday.year;
	strncpy(nCitizen->phone, tphone, strlen(tphone)+1);
	strncpy(nCitizen->mail, tmail, strlen(tmail)+1);
	strncpy(nCitizen->terminal.IP, tterminal.IP, strlen(tterminal.IP)+1);
	nCitizen->terminal.port = tterminal.port;
	strncpy(nCitizen->cert, tcert, strlen(tcert)+1);
	strncpy(nCitizen->serial, tserial, strlen(tserial)+1);
	nCitizen->next = NULL;
	*list = enque(*list, nCitizen);
	return 1;
}

int serveCitizen(citizen **list) {
	if (*list == NULL) {
		printf("nobody to serve\n");
		return 1;
	}
	printf("serving %s %s\n", (*list)->name, (*list)->surname);
	*list = deque(*list);
	return 0;
}

int removeCitizen(citizen **list, char *search) {
	char answer = '0';
	citizen *current = *list;
	citizen *temp = NULL;
	if (current == NULL) {
		printf("noone waiting to serve\n");
		return 0;
	}
	if (strstr(current->surname, search) != NULL) {
		printCitizen(current);
		do {
			printf("do you want to remove this client (y/n)? ");
			scanf("%c", &answer);
			flushStdin();
		}while (answer != 'y' && answer != 'n');
		if (answer == 'y') {
			temp = current;
			*list = current->next;
			free(temp);
			printf("client removed\n");
			return 1;
		}
	}
	while (current->next != NULL) {
		if (strstr(current->next->surname, search) != NULL) {
			printCitizen(current->next);
			do {
				printf("do you want to remove this client (y/n)? ");
				scanf("%c", &answer);
				flushStdin();
			}while (answer != 'y' && answer!= 'n');
			if (answer == 'y') {
				temp = current->next;
				current->next = current->next->next;
				free(temp);
				printf("client removed\n");
				return 1;
			}
		}
		current = current->next;
	}
	printf("no client removed\n");
	return 1;
}

int print(citizen *list, char *data) {
	citizen *current = list;
	if (data[0]!= '*' && validCert(data) == 0) {
		printf("invalid cert to search\n");
		return 0;
	}
	while(current!=NULL) {
		if (data[0]=='*' || strcmp(current->cert, data)==0) {
			printCitizen(current);
		}
		current = current->next;
	}
	return 0;
}

int printRev(citizen *list, char *data) {
	if (data[0]!= '*' && validCert(data) == 0) {
		printf("invalid cert to search\n");
		return 0;
	}
	if (list->next != NULL) printRev(list->next, data);
	if (data[0]=='*' || strcmp(list->cert, data)==0) {
		printCitizen(list);
	}
	return 0;
}

int backup(citizen *list, char *filename) {
	citizen *cit = list;
	FILE *fp;
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("error opening file: %s\n", filename);
		return 0;
	}
	while (cit!=NULL){
		fprintf(fp, "%-30s %-30s %02d/%02d/%4d %12s %30s %15s:%d %15s %15s\n",
			cit->name, cit->surname, cit->birthday.day, cit->birthday.month,
			cit->birthday.year, cit->phone, cit->mail, cit->terminal.IP,
			cit->terminal.port, cit->cert, cit->serial);
		cit = cit->next;
	}
	fclose(fp);
	return 0;
}

int restore(citizen **list, char *filename) {
	char error[50];
	int lineno = 0;
	FILE *fp;
	char line[180];
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("error opening file: %s\n", filename);
		return 0;
	}
	while(fgets(line, 180, fp) != NULL) {
		line[strlen(line)-1] = '\0';
		if (newCitizen(list, line, error) == 0) {
			printf("error reading from line %d: %s", lineno, error);
		}
		lineno++;
	}
	return 0;
}
int main(int argc, char **argv)
{
    char *command;
    char *data;
    citizen *list = NULL;
    char error[50];

    srand(time(NULL));
    command = (char *)malloc(1024*sizeof(char));
    if (command == NULL) {
        perror("bad memory");
        return 0;
    }
    while(1) {
		error[0] = '\0';
        printf("$> ");
        fgets(command,1024,stdin);
        command[strlen(command)-1] = '\0';
        if (strcmp(command, "exit") == 0) {
            break;
        }
        if (strncmp(command, "newCitizen ", 11) == 0){
            data = command+11;
            if (newCitizen(&list, data, error) == 0) printf(error);
            continue;
        }
        if (strncmp(command, "serveCitizen", 12) == 0){
            data = command+13;
            serveCitizen(&list);
            continue;
        }
        if (strncmp(command, "removeCitizen ", 14) == 0) {
			data = command+14;
			removeCitizen(&list, data);
			continue;
		}
        if (strncmp(command, "print ", 6) == 0) {
			data = command + 6;
			print(list, data);
			continue;
		}
		if (strncmp(command, "printRev ", 9) == 0) {
			data = command + 9;
			printRev(list, data);
			continue;
		}
		if (strncmp(command, "backup ", 7) == 0) {
			data = command + 7;
			backup(list, data);
			continue;
		}
		if (strncmp(command, "restore ", 8) == 0) {
			data = command + 8;
			restore(&list, data);
			continue;
		}
        printf("bad command\n");
    }
    free(command);
    while (list != NULL) {
		list = deque(list);
	}
	//list = (citizen *)malloc(sizeof(list));
	return 0;
}
