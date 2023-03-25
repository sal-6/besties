import matplotlib.pyplot as plt

OUTPUT_PATH = "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\d_star_lite\\path.csv"

def parse_path(fpath):
    
    path = []
    with open(fpath, 'r') as f:
        for line in f:
            data = line.split(",")
            path.append((int(data[0]), int(data[1])))
    
    return path


def main():
    data = parse_path(OUTPUT_PATH)
    
    x = [d[0] for d in data]
    y = [d[1] for d in data]
    
    plt.plot(x, y)
    
    plt.show()


if __name__ == "__main__":
    main()