import matplotlib.pyplot as plt
import matplotlib.patches as patches
import os
import imageio
import numpy as np

OUTPUT_PATH = "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\edge_costs\\path"
STAR = (16, 35)
GOAL = (70, 54)

WIDTH = 101
HEIGHT = 101

PATH_SIDE_LENGTH = 0.5
OBS_SIDE_LENGTH = 1

VIEW_DISTANCE = 3

def parse_path(fpath):
    
    path = []
    with open(fpath, 'r') as f:
        for line in f:
            data = line.split(",")
            path.append((int(data[0]), int(data[1])))
    
    return path


def parse_obs(fpath):
    
    obs = []
    with open(fpath, 'r') as f:
        for line in f:
            data = line.split(",")
            obs.append((int(data[0]), int(data[1])))
    
    return obs


def parse_topo(fpath):

    topo = []
    with open(fpath, 'r') as fi:
        for line in fi:
            data = line.split(",")
            topo.append((int(data[0]), int(data[1]), float(data[2])))

    return topo

def calculate_patch_center(x, y, w=1, h=1):
    return (x - w / 2, y - h / 2)

def grey(val):
    hex_value = int(val * 255)
    return '#{0:02x}{0:02x}{0:02x}'.format(hex_value)

def plot_path_old(fpath, output_path, obs_path=None, true_obs_path=None):
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
        
    if obs_path and true_obs_path:
        obs = parse_obs(obs_path)
        true_obs = parse_obs(true_obs_path)
        for i in range(len(true_obs)):
            x, y = true_obs[i]
            if true_obs[i] not in obs:
                rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor='red')
            else:
                rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor='black')
            ax.add_patch(rect)

    # draw a 2*VIEW_DISTANCE x 2*VIEW_DISTANCE square around the current position
    x, y = data[0]
    total_size = 2*VIEW_DISTANCE+3
    rect = patches.Rectangle(calculate_patch_center(x, y, total_size, total_size), total_size, total_size, linewidth=1, edgecolor='black', facecolor='none')
    ax.add_patch(rect)
        
    ax.set_xlim([-1, WIDTH + 1])
    ax.set_ylim([-1, HEIGHT + 1])
    
    # equal aspect ratio
    ax.set_aspect('equal')
    plt.savefig(output_path)
    plt.close()

def animate_dir(pdir, start=0, end=10, plot_obs=False, delete_inputs_files=False):
    
    for i in range(start, end + 1):
        print(f"Plotting path {i} of {end}")
        fpath = os.path.join(pdir, f"path_{i}.csv")
        output_path = os.path.join(pdir, "temp", f"{i}.png")
        if not os.path.exists(os.path.dirname(output_path)):
            os.makedirs(os.path.dirname(output_path))
        if plot_obs:
            plot_path(fpath, output_path, obs_path=os.path.join(pdir, f"obs_{i}.csv"), true_obs_path=os.path.join(pdir, f"true_obs_{i}.csv"))
        else:
            plot_path(fpath, output_path)

    with imageio.get_writer(os.path.join(pdir, "path.gif"), mode='I') as writer:
        for i in range(start, end + 1):
            image = imageio.imread(os.path.join(pdir, "temp", f"{i}.png"))
            writer.append_data(image)

    # remove temp folder and contents
    for i in range(start, end + 1):
        os.remove(os.path.join(pdir, "temp", f"{i}.png"))
    os.rmdir(os.path.join(pdir, "temp"))
    
    if delete_inputs_files:
        for i in range(start, end + 1):
            os.remove(os.path.join(pdir, f"path_{i}.csv"))
            if plot_obs:
                os.remove(os.path.join(pdir, f"obs_{i}.csv"))
                os.remove(os.path.join(pdir, f"true_obs_{i}.csv"))

def plot_path(fpath, obs_path, true_obs_path, topo_path, output_path):

    fig = plt.figure()
    ax = fig.add_subplot(111)
    #os.path.join(OUTPUT_PATH, "topo.csv")
    path = parse_path(fpath)
    topo = parse_topo(topo_path)
    true_obs = parse_obs(true_obs_path)
    obs = parse_obs(obs_path)
    
    heights = [i[2] for i in topo]
    max_h = max(heights)
    
    for i in range(len(topo)):
        x, y, h = topo[i]
        rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor=grey( 1 - h / max_h))
        ax.add_patch(rect)
    
    # Create a rectangle patch centered at start
    rect = patches.Rectangle(calculate_patch_center(STAR[0], STAR[1], 1, 1), 1, 1, linewidth=1, facecolor='blue')
    ax.add_patch(rect)
    
    # Create a rectangle patch centered at goal
    rect = patches.Rectangle(calculate_patch_center(GOAL[0], GOAL[1], 1, 1), 1, 1, linewidth=1, facecolor='green')
    ax.add_patch(rect)
    
    for i in range(len(true_obs)):
        x, y = true_obs[i]
        if true_obs[i] not in obs:
            rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor='pink')
        else:
            rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor='red')
        ax.add_patch(rect)
    
    for i in range(len(path)):
        x, y = path[i]
        rect = patches.Rectangle(calculate_patch_center(x, y, PATH_SIDE_LENGTH, PATH_SIDE_LENGTH), PATH_SIDE_LENGTH, PATH_SIDE_LENGTH, linewidth=1, facecolor='orange')
        ax.add_patch(rect)
        
    x, y = path[0]
    total_size = 2*VIEW_DISTANCE+3
    rect = patches.Rectangle(calculate_patch_center(x, y, total_size, total_size), total_size, total_size, linewidth=1, edgecolor='black', facecolor='none')
    ax.add_patch(rect)
    
    ax.set_xlim([-1, WIDTH])
    ax.set_ylim([-1, HEIGHT])
    
    # equal aspect ratio
    ax.set_aspect('equal')
    plt.savefig(output_path)
    #plt.show()
    plt.close()
    
def animate(pdir, start=0, end=10, delete_inputs_files=False):
    for i in range(start, end + 1):
        print(f"Plotting path {i} of {end}")
        fpath = os.path.join(pdir, f"path_{i}.csv")
        output_path = os.path.join(pdir, "temp", f"{i}.png")
        if not os.path.exists(os.path.dirname(output_path)):
            os.makedirs(os.path.dirname(output_path))
        plot_path(fpath,
                  os.path.join(pdir, f"obs_{i}.csv"),
                  os.path.join(pdir, f"true_obs.csv"),
                  os.path.join(pdir, "topo.csv"),  
                  output_path
                 )

    with imageio.get_writer(os.path.join(pdir, "path.gif"), mode='I') as writer:
        for i in range(start, end + 1):
            image = imageio.imread(os.path.join(pdir, "temp", f"{i}.png"))
            writer.append_data(image)

    # remove temp folder and contents
    if delete_inputs_files:
        for i in range(start, end + 1):
            os.remove(os.path.join(pdir, "temp", f"{i}.png"))
        os.rmdir(os.path.join(pdir, "temp"))
    
    if delete_inputs_files:
        for i in range(start, end + 1):
            os.remove(os.path.join(pdir, f"path_{i}.csv"))
            os.remove(os.path.join(pdir, f"obs_{i}.csv"))
        os.remove(os.path.join(pdir, f"true_obs.csv"))
        os.remove(os.path.join(pdir, "topo.csv"))

if __name__ == "__main__":
    #animate_dir(OUTPUT_PATH, 0, 18, delete_inputs_files=True, plot_obs=True)
    """ plot_path("G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\edge_costs\\path\\path_0.csv",
              "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\edge_costs\\path\\obs_0.csv",
              "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\edge_costs\\path\\true_obs.csv",
              "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\edge_costs\\path\\topo.csv",
              0) """
    animate(OUTPUT_PATH, 0, 53, delete_inputs_files=False)
