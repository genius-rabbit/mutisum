#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <pthread.h>

#define OUTPUT_PATH "output.txt"
#define INPUT_PATH "input.txt"

struct struct_zone_data{
    unsigned long start;
    unsigned long end;
};
struct struct_zone_data zone_data[20];

unsigned long *sum = NULL;
pthread_mutex_t mutex;

int childCalculate(long N, long M){
    int process = 0;
    while(true){
        pid_t pid = fork();
        if(pid == 0){
            // child process
            // calculate and exit
            //printf("child : %d\n", process);
            //printf("calculate zone : %u ~ %u\n", zone_data[process].start, zone_data[process].end);
            unsigned long local_sum = 0;
            for(unsigned long start = zone_data[process].start; start <= zone_data[process].end; start++){
                local_sum += start;
            }

            // add local sum to sum
            pthread_mutex_lock(&mutex);
            int shmid = shmget((key_t)2333, 8, IPC_CREAT|0666);
            void *p_addr = shmat(shmid, NULL, 0);
            if(p_addr == (void*)-1){
                printf("connect shared memery fail\n");
                exit(1);
            }
            sum = (unsigned long*)p_addr;
            *sum = *sum + local_sum;
            //printf("add: %ld\nnow: %ld\n", local_sum, *sum);
            pthread_mutex_unlock(&mutex);
            exit(0);
        }else{
            // parents process
            //printf("parent process: child pid: %d\n", pid);
            // run until
            ++process;
            if(process > N - 1){
                // wait all child process exit
                while(wait(NULL)!=-1){}
                break;
            }
            
        }
    }
    return 0;
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
    unsigned long temp_num = 0;
    fscanf(fp,"%c=%u\n",&temp_char, &temp_num);
    (temp_char == 'M') ? (M = temp_num) : (N = temp_num);
    fscanf(fp,"%c=%u\n",&temp_char, &temp_num);
    (temp_char == 'N') ? (N = temp_num) : (M = temp_num);
    printf("thread num: %u\nnum to sum: %u\n",N, M);
    fclose(fp);

    // cacualate zone
    long block_num = M/N;
    unsigned long extra  = N == 1 ? M : M%N;
    unsigned start_num = 1;
    for(int i = 0; i < N; i++){
        if(i < extra){
            zone_data[i].start = start_num;
            zone_data[i].end = (start_num + block_num) <= M ? (start_num + block_num) : M;
            start_num += block_num + 1;
        }else{
            zone_data[i].start = start_num;
            zone_data[i].end = (start_num + block_num - 1) <= M ? (start_num + block_num -1) : M;
            start_num += block_num;
        }
    }

    pid_t pid = fork();
    if(pid < 0){
        printf("pid < 0 error!!\n");
        exit(1);
    }else if(pid == 0){
        //printf("child process\n");
        childCalculate(N, M);
        exit(0);
    }else{
        //printf("parent process\n");
        // init lock and create shared memery
        pthread_mutex_init(&mutex, NULL);
        int shmid = shmget((key_t)2333, 8, IPC_CREAT|0666);
        void *p_addr = shmat(shmid, NULL, 0);
        if(p_addr == (void*)-1){
            printf("create shared memery fail\n");
            exit(1);
        }
        sum = (unsigned long*)p_addr;

        // wait child process exit
        waitpid(pid, NULL, 0);
        printf("child is exit\n");

        // get end time
        gettimeofday(&tv, NULL);
        long end = tv.tv_sec*1000000 + tv.tv_usec;
        printf("time use: %ld\n", end - start);
        printf("result:%ld\n", *sum);
        if(argc < 2){
            printf("output file: %s\n", OUTPUT_PATH);
            fp = fopen(OUTPUT_PATH, "w+");
        }else{
            printf("output file: %s\n", argv[2]);
            fp = fopen(argv[2], "w+");
        }
        fprintf(fp, "%ld", *sum);
        fclose(fp);

        // destory share memery
        shmdt(sum);
        shmctl(shmid, IPC_RMID, NULL);
    }
    
    return 0;
}
