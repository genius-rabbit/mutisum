#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define INPUT_PATH "input.txt"
#define OUTPUT_PATH "output.txt"

struct thread_data{
    unsigned long start;
    unsigned long end;
};

unsigned long sum = 0;
pthread_mutex_t mutex;

void *multisum(void *data_pack){
    struct thread_data *pack = (struct thread_data *)data_pack;
    //printf("cacualate : %u~%u\n", pack->start, pack->end);
    if(pack->start > pack->end){
        printf("start:%u > end:%u error!!!\n", pack->start, pack->end);
    }
    unsigned long local_sum = 0;
    for(unsigned long i = pack->start; i <= pack->end; i++){
       local_sum += i;
    }
    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);
    //printf("from %ld to %ld, add : %ld \nnow sum: %ld\n", pack->start, pack->end, local_sum, sum);
    pthread_exit(NULL); 
}

int main(int argc, char *argv[]){
    // start time 
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long start = tv.tv_sec*1000000 + tv.tv_usec;
    // M : max num to sum
    // N : process or thread nums
    long M = 0,N = 0;
    // read data from input.txt
    FILE *fp = NULL;
    if(argc < 2){
        printf("open file : %s\n", INPUT_PATH);
        fp = fopen(INPUT_PATH, "r+");
    }else{
        printf("open file : %s\n", argv[1]);
        fp = fopen(argv[1], "r+");
    }

    if(fp == NULL){
        printf("file path:%s is error\n", INPUT_PATH);
        exit(1);
    }
    char temp_char = '\0';
    char nums[20];
    unsigned long temp_num = 0;
    fscanf(fp,"%c=%u\n",&temp_char, &temp_num);
    (temp_char == 'M') ? (M = temp_num) : (N = temp_num);
    fscanf(fp,"%c=%u\n",&temp_char, &temp_num);
    (temp_char == 'N') ? (N = temp_num) : (M = temp_num);
    printf("thread num: %u\nnum to sum: %u\n",N, M);
    fclose(fp);

    pthread_t *thread_handles;
    thread_handles = malloc(N * sizeof(pthread_t));
    long block_num = M/N;
    unsigned long extra  = N == 1 ? M : M%N;
    
    // init lock
    pthread_mutex_init(&mutex, NULL);
    unsigned long start_num = 1;
    for(int thread = 0; thread < N; thread++){
        struct thread_data *data_pack = (struct thread_data *)malloc(sizeof(struct thread_data));
        if(thread < extra){
            data_pack->start = start_num;
            data_pack->end = (start_num + block_num) <= M ? (start_num + block_num) : M;
            start_num += block_num + 1;
        }else{
            data_pack->start = start_num;
            data_pack->end = (start_num + block_num - 1) <= M ? (start_num + block_num -1) : M;
            start_num += block_num;
        }
        //printf("thread:%d : %d~%d\n", thread, data_pack->start, data_pack->end);
        pthread_create(&thread_handles[thread], NULL, multisum, data_pack);
    }
    for(int thread = 0; thread < N; thread++){
        pthread_join(thread_handles[thread], NULL);
    }
    // destory lock
    pthread_mutex_destroy(&mutex);

    free(thread_handles);

    // end time
    gettimeofday(&tv, NULL);
    long end = tv.tv_sec*1000000 + tv.tv_usec;
    printf("time use: %ld\n", end - start);
    printf("result:%ld\n", sum);
    if(argc < 2){
        printf("output file: %s\n", OUTPUT_PATH);
        fp = fopen(OUTPUT_PATH, "w+");
    }else{
        printf("output file: %s\n", argv[2]);
        fp = fopen(argv[2], "w+");
    }
    fprintf(fp, "%ld", sum);
    fclose(fp);
    return 0;
}