import matplotlib.pyplot as plt
import matplotlib.patches as patches
import os
import imageio

OUTPUT_PATH = "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\d_star_lite\\path"
STAR = (0, 0)
GOAL = (29, 15)

WIDTH = 30
HEIGHT = 30

PATH_SIDE_LENGTH = 0.5

def parse_path(fpath):
    
    path = []
    with open(fpath, 'r') as f:
        for line in f:
            data = line.split(",")
            path.append((int(data[0]), int(data[1])))
    
    return path


def calculate_patch_center(x, y, w=1, h=1):
    return (x - w / 2, y - h / 2)


def plot_path(fpath, output_path):
    data = parse_path(fpath)
    
    # Create a figure and axis
    fig = plt.figure()
    ax = fig.add_subplot(111)
    
    # Create a rectangle patch centered at start
    rect = patches.Rectangle(calculate_patch_center(STAR[0], STAR[1], 1, 1), 1, 1, linewidth=1, facecolor='blue')
    ax.add_patch(rect)
    
    # Create a rectangle patch centered at goal
    rect = patches.Rectangle(calculate_patch_center(GOAL[0], GOAL[1], 1, 1), 1, 1, linewidth=1, facecolor='green')
    ax.add_patch(rect)
    
    for i in range(len(data)):
        x, y = data[i]
        rect = patches.Rectangle(calculate_patch_center(x, y, PATH_SIDE_LENGTH, PATH_SIDE_LENGTH), PATH_SIDE_LENGTH, PATH_SIDE_LENGTH, linewidth=1, facecolor='pink')
        ax.add_patch(rect)
        
    ax.set_xlim([-1, WIDTH + 1])
    ax.set_ylim([-1, HEIGHT + 1])
    
    # equal aspect ratio
    ax.set_aspect('equal')
    plt.savefig(output_path)
    plt.close()

def animate_dir(pdir, start=0, end=28):
    
    for i in range(start, end + 1):
        print(f"Plotting path {i} of {end}")
        fpath = os.path.join(pdir, f"path_{i}.csv")
        output_path = os.path.join(pdir, "temp", f"{i}.png")
        if not os.path.exists(os.path.dirname(output_path)):
            os.makedirs(os.path.dirname(output_path))
        plot_path(fpath, output_path)

    with imageio.get_writer(os.path.join(pdir, "path.gif"), mode='I') as writer:
        for i in range(start, end + 1):
            image = imageio.imread(os.path.join(pdir, "temp", f"{i}.png"))
            writer.append_data(image)

    # remove temp folder and contents
    for i in range(start, end + 1):
        os.remove(os.path.join(pdir, "temp", f"{i}.png"))
    os.rmdir(os.path.join(pdir, "temp"))

if __name__ == "__main__":
    animate_dir(OUTPUT_PATH, 0, 28)