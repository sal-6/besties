import matplotlib.pyplot as plt
import matplotlib.patches as patches

OUTPUT_PATH = "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\d_star_lite\\path.csv"
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


def test():
    # Create a figure and axis
    fig, ax = plt.subplots()

    # Create a rectangle patch centered at (0,0)
    rect = patches.Rectangle((-.5, -.5), 1, 1, linewidth=1, edgecolor='r')

    # Add the patch to the axis
    ax.add_patch(rect)

    # Set the x and y limits to show only the rectangle
    ax.set_xlim([-0.6, 0.6])
    ax.set_ylim([-0.6, 0.6])

    # Show the plot
    plt.show()

def main():
    data = parse_path(OUTPUT_PATH)
    
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
        print(x, y)
        rect = patches.Rectangle(calculate_patch_center(x, y, PATH_SIDE_LENGTH, PATH_SIDE_LENGTH), PATH_SIDE_LENGTH, PATH_SIDE_LENGTH, linewidth=1, facecolor='pink')
        ax.add_patch(rect)
        
    ax.set_xlim([-1, WIDTH + 1])
    ax.set_ylim([-1, HEIGHT + 1])
    
    # equal aspect ratio
    ax.set_aspect('equal')
    plt.show()


if __name__ == "__main__":
    main()