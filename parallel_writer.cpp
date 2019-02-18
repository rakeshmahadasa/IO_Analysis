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

int NUM_THREADS = 8;
int wait_counter = 0;
#ifndef QUEUE_SIZE
#define QUEUE_SIZE 8
#endif

template <class ET>
inline bool CAS(ET *ptr, ET oldv, ET newv) {
  if (sizeof(ET) == 1) {
    return __sync_bool_compare_and_swap((bool*)ptr, *((bool*)&oldv), *((bool*)&newv));
  } else if (sizeof(ET) == 4) {
    return __sync_bool_compare_and_swap((int*)ptr, *((int*)&oldv), *((int*)&newv));
  } else if (sizeof(ET) == 8) {
    return __sync_bool_compare_and_swap((long*)ptr, *((long*)&oldv), *((long*)&newv));
  }
  else {
    std::cout << "CAS bad length : " << sizeof(ET) << std::endl;
    abort();
  }
}

template <class ET>
inline ET writeAdd(ET *a, ET b) {
  volatile ET newV, oldV;
  do {oldV = *a; newV = oldV + b;}
  while (!CAS(a, oldV, newV));
  return newV;
}


class bqueue
{
  private:
    char **worklist;
    int start;
    int pivot;
    int end;

  public:
    bqueue()
    {
        worklist = new char*[QUEUE_SIZE];
        for(int i = 0; i < QUEUE_SIZE; i++){
            worklist[i] = new char[50331648];
            memset(worklist[i],'0',sizeof(worklist[i]));
        }
        start = 0;
        pivot=0;
        end = 0;
    };

    void give_work()
    {
        int curr_end = writeAdd(&end,1);
        while (!CAS(&pivot, curr_end-1, curr_end ))
            sched_yield();
    }

    int take_work(int waitline)
    {
        while (true)
        {
            int curr_start = start;
            int curr_pivot = pivot;
            if (curr_start == curr_pivot)
            {  
                if(waitline)
                    writeAdd(&wait_counter,1);
                if(wait_counter == NUM_THREADS){
                    for(int i=0;i<NUM_THREADS-1;i++){                        
                        give_work();
                    }
                    return -1;
                }
                return -2;
            }
            if(!waitline){
                writeAdd(&wait_counter,-1);
                waitline=true;
            }
            int args = worklist[curr_start];
            if (CAS(&start, curr_start, curr_start + 1))
            {
                return args;
            }
        }
    }

    ~bqueue()
    {

        delete[] worklist;
    };
};


int main(int argc,char** argv){
    
    if(argc < 2){
        cerr<<"Invalid command"<<endl;
        cerr<<"format : ./a.out blocksize iter_count"<<endl;
        exit(1);
    }

    uint32_t block_size = atoi(argv[1]);
    uint32_t iter_count = atoi(argv[2]);
    uint16_t option = atoi(argv[3]);

    
    memset(block,'0',sizeof(block));
    string out_file = "out/"+to_string(block_size)+".bin";
    FILE* ofile = fopen(out_file.c_str(),"w+b");
    fsync(fileno(ofile));
    vector<uint64_t> runtimes(2000,0);
    for (int i = 0; i < iter_count; i++)
    {
        high_resolution_clock::time_point start_time = high_resolution_clock::now();
        fwrite(&block, sizeof(block), 1, ofile);
        fsync(fileno(ofile));
        high_resolution_clock::time_point end_time = high_resolution_clock::now();
        auto time_diff = duration_cast<milliseconds>(end_time - start_time).count();
        runtimes[i] = time_diff;
    }
    double sum = accumulate(begin(runtimes), end(runtimes), 0.0);
    double mean =  sum / iter_count;
    cout<<sum<<" "<<runtimes.size()<<endl;
    double sq_sum = 0.0;
    for(vector<uint64_t>::iterator it = runtimes.begin();it!=runtimes.end();++it) {
        sq_sum += (*it - mean) * (*it - mean);
    }
    double stdev = sqrt(sq_sum / (iter_count-1));
    fsync(fileno(ofile));
    fclose(ofile);
    return 0;

}