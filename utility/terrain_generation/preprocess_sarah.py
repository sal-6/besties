import os

PATH = "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\data\\height_128_128.txt"

topo = []
with open(PATH, 'r') as fi:
    for line in fi:
        data = line.split(",")
        topo.append([int(data[0]) - 1, int(data[1]) - 1, float(data[2])])

with open(PATH[:-4] + "_proc.txt", 'w') as fo:
    for i in range(len(topo)):
        fo.write(str(topo[i][0]) + "," + str(topo[i][1]) + "," + str(topo[i][2]) + "\n")


    