import os
block_iter_map = [
[1024 , 2000000],
[2048 , 1000000],
[4096 , 500000],
[8192 , 250000],
[16384 , 125000],
[32768 , 62500],
[65536 , 31250 ],
[1024000 , 2000],
[2048000 , 1000],
[4096000 , 500],
[8192000 , 250]
]
stat_files = ["stats/unif_data_stats.out","stats/unif_iter_stats.out","stats/flush_writer_stats.out","stats/reader_stats.out"]
for file in stat_files:
        open(file, 'w').close()

def unif_data_writer():
    os.system("g++ -std=c++11 writer.cpp -o writer")
    for x in block_iter_map:
        os.system("./writer "+str(x[0])+" "+str(x[1])+" 1 >> "+"stats/unif_data_stats.out")

def unif_iter_writer():
    os.system("g++ -std=c++11 writer.cpp -o writer")
    for x in block_iter_map:
        os.system("./writer "+str(x[0])+" "+str(1000)+" 2 >> "+"stats/unif_iter_stats.out")

def flush_writer():
    os.system("g++ -std=c++11 writer.cpp -o writer")
    for x in block_iter_map:
        os.system("./writer "+str(x[0])+" "+str(1000)+" 3 >> "+"stats/flush_writer_stats.out")

def reader():
    os.system("g++ -std=c++11 reader.cpp -o reader")
    for x in block_iter_map:
        os.system("./reader "+str(x[0])+" "+str(1000)+" >> "+"stats/reader_stats.out")

if __name__ == "__main__":
    unif_data_writer()
    unif_iter_writer()
    flush_writer()
    reader()
