import os
block_iter_map = {
    1024 : 2000000,
    2048 : 1000000,
    4096 : 500000,
    8192 : 250000,
    16384 : 125000,
    32768 : 62500,
    65536 : 31250 ,
    1024000 : 2000,
    2048000 : 1000,
    4096000 : 500,
    8192000 : 250
}
def writer():
    os.system("g++ -std=c++11 writer.cpp -o writer")
    for x in block_iter_map:
        os.system("./writer "+str(x)+" "+str(1000)+" >> "+"stats.out")

def reader():
    os.system("g++ -std=c++11 reader.cpp -o reader")
    for x in block_iter_map:
        os.system("./reader "+str(x)+" "+str(1000)+" >> "+"stats.out")

if __name__ == "__main__":
    #writer()
    reader()