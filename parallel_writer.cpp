#include<iostream>
#include<string.h>
#include<vector>
#include<stdio.h>
#include<unistd.h>
#include<chrono>
#include <numeric>
#include<math.h>
#include<pthread.h>

using namespace std;
using namespace std::chrono;

int NUM_THREADS = 0;
std::vector<std::vector<uint64_t> > t_runtimes(8,std::vector<uint64_t>(50)); 
int r_idx=0;
char block[268435456];
int block_size=256;
int writers_per_thread = 0;
pthread_mutex_t mtx;

void* parallel_write(void* input_args){
    
    int tid = *((int *) input_args);
    string out_file = "out/"+to_string(block_size)+ "/" +to_string(tid) +".bin";
    FILE* ofile = fopen(out_file.c_str(),"w+b");
    fsync(fileno(ofile));

    for (int i = 0; i < writers_per_thread; i++)
    {   
        
        high_resolution_clock::time_point start_time = high_resolution_clock::now();
        fwrite(&block, sizeof(block), 1, ofile);
        fsync(fileno(ofile));
        high_resolution_clock::time_point end_time = high_resolution_clock::now();
        auto time_diff = duration_cast<milliseconds>(end_time - start_time).count();
        t_runtimes[tid][i] = time_diff;
    }
    fclose(ofile);
    pthread_exit(0);
}   

int main(int argc,char** argv){
    
    if(argc < 2){
        cerr<<"Invalid command"<<endl;
        cerr<<"format : ./a.out blocksize iter_count"<<endl;
        exit(1);
    }

    uint32_t block_size = atoi(argv[1]);
    uint32_t iter_count = atoi(argv[2]);
    NUM_THREADS = atoi(argv[3]);
    writers_per_thread = iter_count / NUM_THREADS;
    memset(block,'0',sizeof(block));
    pthread_t threads[NUM_THREADS];
    high_resolution_clock::time_point t_start_time = high_resolution_clock::now();
    for(int i = 0; i < NUM_THREADS; i++){
        int rc =  pthread_create(&threads[i],NULL,parallel_write,new int(i));
        if( rc  != 0 ){
            cout<<"Thread creation Failed : "<<i<<endl;
            exit(1);
        }
    }
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i],NULL);
    }
    high_resolution_clock::time_point t_end_time = high_resolution_clock::now();
    cout<<"Total Run Time : "<<duration_cast<milliseconds>(t_end_time - t_start_time).count()<<endl;
    cout<<"======Per Thread Stats========="<<endl;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        cout<<"TID : "<<i<<endl;
        double sum = accumulate(begin(t_runtimes[i]), end(t_runtimes[i]), 0.0);
        double mean = sum / writers_per_thread;
        double sq_sum = 0.0;
        for (vector<uint64_t>::iterator it = t_runtimes[i].begin(); it != t_runtimes[i].end(); ++it)
        {
            sq_sum += (*it - mean) * (*it - mean);
        }
        double stdev = sqrt(sq_sum / (writers_per_thread - 1));
        cout << sum <<" "<< mean << " " << stdev << endl;
    }
    
    pthread_exit(0);

}
