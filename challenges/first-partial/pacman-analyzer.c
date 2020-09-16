#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define REPORT_FILE "packages_report.txt"
#ifndef BUF_SIZE
#define BUF_SIZE 128
#endif

#ifndef SIZE
#define SIZE 1000
#endif

typedef struct Line{
    char time[24];
    char process[15];
    char action[40];
}Line;

typedef struct Program{
    int ID;
    char name[25];
    char installDate[25];
    char lastUpdate[25];
    int numUpdates;
    char removalDate[25];
}Program;

struct Program* hashArray[SIZE]; 
struct Program* app;

void readR1(int inputFd, int outputFd);
void readR2(int inputFd, int outputFd);
void writeR1(int outputFd, int installed, int removed, int upgraded, int current);
void writeR2(int inputFd, int outputFd);
int hashName(const char* word);
int hashCode(int ID);
void display();
void insert(int ID, char* name,char* installDate,char* lastUpdate, int numUpdates, char* removalDate);

int main(int argc, char **argv) {
    int inputFd, report1Fd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];


    if (argc < 2) {
	    printf("Usage:./pacman-analizer.o \n");
        printf("Not enough arguments \n");
	    exit(0);
    }

    if(strcmp(argv[1],"-input")!= 0){
        printf("No input flag \n");
        exit(0);
    }

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    //read input log file
    inputFd = open(argv[2], O_RDONLY);
    if (inputFd == -1){
        printf("Failed to read the file %s ", argv[2]);
        exit(0);
    }

    if(strcmp(argv[3],"-report")!= 0){
        printf("No report flag \n");
        exit(0);
    }

    //create packages report file
    outputFd = open(argv[4],openFlags, filePerms);
    if(outputFd == -1){
        printf("Failed to create and open the file \n");
        exit(0);
    }

    printf("Generating Report from: [%s] log file", argv[2]);

    //First part of the report
    readR1(inputFd, outputFd);

    //second part of the report
    readR2(inputFd, outputFd);
    
    printf("\n Report is generated at: [%s] \n", argv[4]);

    close(inputFd);
    close(outputFd);

    return 0;
}

int hashName(const char* name){
    //the key for our hashtable is based on the program name itself.
    signed int hash = 0;
    for (int i = 0 ; name[i] != '\0' ; i++){
        hash = 31*hash + name[i];
    }
    int answer = hash % SIZE;
    if(answer<0){
        answer=answer*(-1);
    }
    return answer;
}


void insert(int ID, char* name,char* installDate,char* lastUpdate, int numUpdates, char* removalDate) {
    //will insert each valid line into out hash table
    struct Program *app = (struct Program*) malloc(sizeof(struct Program));

    app->ID = ID;

    size_t lenName = strlen(name);
    strncpy(app->name, name, lenName);

    size_t lenID = strlen(installDate);
    strncpy(app->installDate, installDate, lenID);

    size_t lenLU = strlen(lastUpdate);
    strncpy(app->lastUpdate, lastUpdate, lenLU);

    app->numUpdates = numUpdates;

    size_t lenRD= strlen(removalDate);
    strncpy(app->removalDate, removalDate, lenRD);

    int hashIndex = hashCode(ID);

    while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->ID != -1) {
        ++hashIndex;
        hashIndex %= SIZE;
    }
    //saving
    hashArray[hashIndex] = app;
}

int hashCode(int ID) {
   return ID % SIZE;
}


void readR1(int inputFd, int outputFd){
    char location;
    int installed = 0, removed = 0, upgraded = 0;
    int delimiter = 0, operation = 0, manager = 0;
    int i=1;
    
    Line *myLine = (Line *)malloc(sizeof(Line));

    //ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);
    //getdelim is not **technically** getline
    //I'm gonna skip using it just in case.

    while( read(inputFd, &location, i) !=0){
        if(location == '['){
            delimiter++;
        } 
        else if(location == ']'){
            delimiter++;
        }
        else if(location == '\n') {

            manager = 0;
            operation = 0;
            delimiter = 0;

            if(strstr(myLine->process, "ALPM") != NULL){ 
                if(strstr(myLine->action,"installed") != NULL){
                    installed++;
                }
            }
            if(strstr(myLine->process, "ALPM") != NULL){
                if(strstr(myLine->action,"upgraded") != NULL){
                    upgraded++;
                }
            }
            if(strstr(myLine->process, "ALPM") != NULL){
                if(strstr(myLine->action,"removed") != NULL){
                    removed++;
                }
            }
            //void *memset(void *str, int c, size_t n)
            memset(myLine->time,0,24);
            memset(myLine->process,0,15);
            memset(myLine->action,0,40);
        }
        else if(delimiter == 3){
            myLine->process[manager++]=location;
        }
        else if(delimiter == 4){
            myLine->action[operation++]=location;      
        }
    }
    writeR1(outputFd, installed, removed, upgraded, installed-removed);
}

void writeR1(int outputFd, int installed, int removed, int upgraded, int current){

    char l1[30] = "\n Pacman Packages Report ";
    char l2[30] = "\n ----------------------";
    char l3[30];
    char l4[30];
    char l5[30];
    char l6[30];
    char l7[30] = "\n List of packages";
    char l8[30] = "\n  ----------------";

    sprintf(l3,"\n - Installed packages : %d ",installed);
    sprintf(l4,"\n - Removed packages : %d ",removed);
    sprintf(l5,"\n - Upgraded packages  : %d ",upgraded);
    sprintf(l6,"\n - Current installed  : %d ",current);

    size_t len1 = strlen(l1);
    size_t len2 = strlen(l2);
    size_t len3 = strlen(l3);
    size_t len4 = strlen(l4);
    size_t len5 = strlen(l5);
    size_t len6 = strlen(l6);
    size_t len7 = strlen(l7);
    size_t len8 = strlen(l8);

    write(outputFd, l1, len1);
    write(outputFd, l2, len2);
    write(outputFd, l3, len3);
    write(outputFd, l4, len4);
    write(outputFd, l5, len5);
    write(outputFd, l6, len6);
    write(outputFd, l7, len7);
    write(outputFd, l8, len8);

    printf("%s", l1);
    printf("%s", l2);
    printf("%s", l3);
    printf("%s", l4);
    printf("%s", l5);
    printf("%s", l6);
    printf("%s", l7);
    printf("%s", l8);
}

void readR2(int inputFd, int outputFd){

    
    writeR2(inputFd, outputFd);
}

void writeR2(int inputFd, int outputFd){

    //some test cases I made:

    //hashing the name of the package to get the ID/Key for the table
    int a = hashName("linux-api-headers");
    int b = hashName("libnetfilter_conntrack");
    
    //printf("%i", a);
    
    insert(a, "linux-api-headers", "2019-04-16 11:32","2019-04-17 11:33", 0, "2019-04-18 11:34");
    insert(b, "libnetfilter_conntrack", "2020-04-16 11:32","2020-04-17 11:33", 2, "-");

    //will save and print in console every package saved
    int i;
    for(i = 0; i<SIZE; i++) {
        if(hashArray[i] != NULL){
            //Name
            char pName[50];
            strcpy(pName, "\n - Package Name       : ");
            strcat(pName, hashArray[i]->name);
            printf("%s", pName);
            size_t lenpName = strlen(pName);
            write(outputFd, pName, lenpName);
            //Install
            char pInstallDate[50];
            strcpy(pInstallDate, "\n  - Install date      : ");
            strcat(pInstallDate, hashArray[i]->installDate);
            printf("%s", pInstallDate);
            size_t lenpID = strlen(pInstallDate);
            write(outputFd, pInstallDate, lenpID);

            //Update
            char pLastUpdate[50];
            strcpy(pLastUpdate, "\n  - Last update date  : ");
            strcat(pLastUpdate, hashArray[i]->lastUpdate);
            printf("%s", pLastUpdate);
            size_t lenpLU = strlen(pLastUpdate);
            write(outputFd, pLastUpdate, lenpLU);

            //numUpdates
            char pNumUpdates[50];
            sprintf(pNumUpdates,"\n  - How many updates  : %d ",hashArray[i]->numUpdates);
            printf("%s", pNumUpdates);
            size_t lenpNU = strlen(pNumUpdates);
            write(outputFd, pNumUpdates, lenpNU);
            
            //removalDate
            char pRemovalDate[50];
            strcpy(pRemovalDate, "\n  - Removal date      : ");
            strcat(pRemovalDate, hashArray[i]->removalDate);
            printf("%s", pRemovalDate);
            size_t lenpRD = strlen(pRemovalDate);
            write(outputFd, pRemovalDate, lenpRD);
        }
        else{
            printf(" ");
        }
    }

 
}