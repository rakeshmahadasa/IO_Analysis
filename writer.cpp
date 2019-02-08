#include<iostream>
#include<string.h>
#include<vector>
#include<stdio.h>
#include<unistd.h>
#include<chrono>
#include <numeric>
#include<math.h>

using namespace std;
using namespace std::chrono;
char block[134217728];
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
    if(option == 3){
        for (int i = 0; i < iter_count; i++)
        {
            high_resolution_clock::time_point start_time = high_resolution_clock::now();
            fwrite(&block, sizeof(block), 1, ofile);
            fsync(fileno(ofile));
            high_resolution_clock::time_point end_time = high_resolution_clock::now();
            auto time_diff = duration_cast<milliseconds>(end_time - start_time).count();
            runtimes[i]=time_diff;
        }
    }
    else{
        for (int i = 0; i < iter_count; i++)
        {
            high_resolution_clock::time_point start_time = high_resolution_clock::now();
            fwrite(&block, sizeof(block), 1, ofile);
            if(i==iter_count) fsync(fileno(ofile));
            high_resolution_clock::time_point end_time = high_resolution_clock::now();
            auto time_diff = duration_cast<milliseconds>(end_time - start_time).count();
            runtimes.push_back(time_diff);
        }
    }
    
    double sum = accumulate(begin(runtimes), end(runtimes), 0.0);
    double mean =  sum / iter_count;
    cout<<sum<<" "<<runtimes.size()<<endl;
    double sq_sum = 0.0;
    for(vector<uint64_t>::iterator it = runtimes.begin();it!=runtimes.end();++it) {
        sq_sum += (*it - mean) * (*it - mean);
    }
    double stdev = sqrt(sq_sum / (iter_count-1));
    cout<<block_size<<" "<<sum<<" "<<mean<<" "<<stdev<<endl;
    fsync(fileno(ofile));
    fclose(ofile);
    return 0;

}