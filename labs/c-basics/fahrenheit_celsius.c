#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void table(float LOWER, float UPPER, float STEP);
void simple(float fahr);
/* print Fahrenheit-Celsius table */
typedef enum {false, true} bool;

int main()
{
    float fahr, LOWER, UPPER, STEP;
    char inputU[100];
    char delim[] = " ";
    scanf("%[^\n]s", &inputU);

    int inputL = strlen(inputU);
    bool check = false;

    int i=0;

    while (i<inputL) {
        if(inputU[i] == ' ') {
            check = true;
        }
        i++;
    }

    if(check == false)
        fahr = 0.0;
        fahr = atof(inputU);
    simple(fahr);

    if(check == true){
        fahr=0,0;
        char *ptr = strtok(inputU, delim);
        LOWER = atof(ptr);
        ptr = strtok(NULL, delim);
        UPPER = atof(ptr);
        ptr = strtok(NULL, delim);
        STEP = atof(ptr);
        ptr = strtok(NULL, delim);
        table(LOWER, UPPER, STEP);
    }
    return 0;
}

void table(float LOWER, float UPPER, float STEP) {
    for (int i = LOWER; LOWER <= UPPER; LOWER = LOWER + STEP){
        printf("Fahrenheit: %f, Celcius: %6.1f\n", LOWER, (5.0/9.0)*(LOWER-32));
        i= i+1;
    }
}

void simple(float fahr) {
    float answ = (5.0/9.0)*(fahr-32);
    printf("Fahrenheit: %f, Celcius: %f \n", fahr, answ);
}