#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 2000
#define MATRIX_SIZE 2000

int NUM_BUFFERS;
long **buffers;
char *RESULT_MATRIX_FILE;
pthread_mutex_t *mutex;
pthread_t threads[NUM_THREADS];

long *readMatrix(char *filename);
long *getColumn(int col, long *matrix);
long *getRow(int row, long *matrix);
int getLock();
int releaseLock(int lock);
long dotProduct(long *Vt1, long *Vt2);
long *multiply(long *matA, long *matB);
int saveResultMatrix(long *result);

typedef struct{
  int row;
  int col;
  long *matA;
  long *matB;
}argSt;


int main(int argc, char** argv){
    // Place your magic here
    if(argc != 5){
        printf("Error: Incorrect run arguments \n");
        exit(1);

    }else{
        for(int i = 1; i < argc; i ++){
            if(strcmp(argv[i],"-n") == 0){
                i++;
                if(i >= argc){
                    printf("Run incorrectly\n");
                    exit(1);
                }
                NUM_BUFFERS = atoi(argv[i]);
            } else if(strcmp(argv[i], "-out") == 0){
                i++;
                if(i>=argc){
                    printf("Run incorrectly\n");
                    exit(1);
                }
                RESULT_MATRIX_FILE = argv[i];
                }
        }
    }

    mutex = malloc(NUM_BUFFERS*sizeof(pthread_mutex_t));
    buffers = malloc(NUM_BUFFERS*sizeof(long *));

    long *matA = readMatrix("matA.dat");
    long *matB = readMatrix("matB.dat");

    
    for(int i = 0; i < NUM_BUFFERS; i++){
        pthread_mutex_init(&mutex[i], NULL);
    }
    
    long *ans = multiply(matA,matB);
    printf("Multiplication completed \n");
    saveResultMatrix(ans);
    free(matA);
    free(matB);
    free(mutex);
    free(buffers);
    return 0;
}


long *readMatrix(char *filename){
    int size = 0;
    FILE *file = fopen(filename, "r");
    
    if(file == NULL){
        printf("Unable to open the file.\n");
        exit(2);
    }

    printf("Reading: %s \n", filename);

    char c;
    while((c=fgetc(file))!=EOF){
        if(c=='\n'){
            size++;
        }
    }
    rewind(file);
    
    long *matrix = malloc(size*sizeof(long));
    int index = 0;
    while(fscanf(file, "%ld", &matrix[index])!=EOF){
        index++;
    }
    
    fclose(file);
    printf("File read successfully.\n");
    return matrix;
}

long *getColumn(int column, long *matrix){
    long *col=malloc(MATRIX_SIZE*sizeof(long));
    for(int i=0; i<MATRIX_SIZE; i++){
        col[i] = matrix[column];
        column += MATRIX_SIZE;
    }
    return col;
}

long *getRow(int row, long *matrix){
    long *rw=malloc(MATRIX_SIZE*sizeof(long));
    int rs=MATRIX_SIZE*(row);
    for(int i=0;i<MATRIX_SIZE;i++){
        rw[i]=matrix[rs+i];
    }
    return rw;
}

int getLock(){
    for(int i=0; i<NUM_BUFFERS; i++){
        if(pthread_mutex_trylock(&mutex[i])==0){
            return i;
        }
    }
    return -1;
}

int releaseLock(int lock){
    return pthread_mutex_unlock(&mutex[lock]);
}

long dotProduct(long *Vt1, long *Vt2){
    long ans=0;
    for(int i=0; i<MATRIX_SIZE; i++)
        ans+=Vt1[i]*Vt2[i];
    return ans;
}

long doMulty(argSt *arg){
    int buffer1 = -1; 
    int buffer2 = -1;

    while(buffer1 == -1 || buffer2 == -1){
        if(buffer1 == -1){
            buffer1 = getLock();
        }
        if(buffer2 == -1){
            buffer2 = getLock();
        }
    }
    buffers[buffer1] = getRow(arg->row, arg->matA);
    buffers[buffer2] = getColumn(arg->col, arg->matB);
    long ans = dotProduct(buffers[buffer1], buffers[buffer2]);

    free(buffers[buffer1]);
    free(buffers[buffer2]);
    free(arg);
    releaseLock(buffer1);
    releaseLock(buffer2);
    
    return ans;
}

long *multiply(long *matA, long *matB){
    printf("Multiplication started\n");
    long *ans= malloc(NUM_THREADS*NUM_THREADS*sizeof(long));

    for(int i=0; i<NUM_THREADS; i++){
        for(int j=0;j<NUM_THREADS;j++){
            argSt *argP = malloc(sizeof(argSt));
            argP -> row = i;
            argP -> col = j;
            argP -> matA = matA;
            argP -> matB = matB;
            pthread_create(&threads[j],NULL,(void * (*) (void *))doMulty,(void *)argP);
        }

        for(int j=0; j<NUM_THREADS; j++){
            void *result;
            pthread_join(threads[j], &result);
            ans[NUM_THREADS * j + i] = (long) result;
        }
    }
    printf("Finished multiplying matrices\n");
    return ans;
}

int saveResultMatrix(long *result) {    
    printf("Results saved to file \n");
    FILE *file;
    file = fopen(RESULT_MATRIX_FILE, "w+");
    if(file==NULL){
        return -1;
    }
    int size=MATRIX_SIZE*MATRIX_SIZE;
    for(int i=0; i<size; i++) {
        fprintf(file, "%ld\n", result[i]);
    }
    fclose(file);
    return 0;
}