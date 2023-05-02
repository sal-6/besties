import matplotlib.pyplot as plt
import matplotlib.patches as patches
import os
import imageio
import numpy as np

OUTPUT_PATH = "G:\\My Drive\\UMD\\Spring 2023\\ENAE788V\\Code\\besties\\output\\edge_costs\\path"
""" STAR = (10, 10)
GOAL = (120, 120)

WIDTH = 127
HEIGHT = 127

PATH_SIDE_LENGTH = 0.5
OBS_SIDE_LENGTH = 1

VIEW_DISTANCE = 3

DELETE_FILES = False
NUM_FRAMES = 28 """


""" STAR = (0, 0)
GOAL = (29, 29)

WIDTH = 30
HEIGHT = 30

PATH_SIDE_LENGTH = 0.5
OBS_SIDE_LENGTH = 1

VIEW_DISTANCE = 3

DELETE_FILES = False
NUM_FRAMES = 38 """

STAR = (10, 10)
GOAL = (115, 115)

WIDTH = 129
HEIGHT = 129

PATH_SIDE_LENGTH = 0.5
OBS_SIDE_LENGTH = 1

VIEW_DISTANCE = 3

DELETE_FILES = False
NUM_FRAMES = 196

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

def parse_shadows(fpath):
    
    shad = []
    with open(fpath, 'r') as f:
        for line in f:
            data = line.split(",")
            shad.append([int(data[0]), int(data[1]), float(data[2]), float(data[3]), float(data[4])])
    
    return shad

def calculate_patch_center(x, y, w=1, h=1):
    return (x - w / 2, y - h / 2)

def grey(val):
    hex_value = int(val * 255)
    return '#{0:02x}{0:02x}{0:02x}'.format(hex_value)

def jet(val):
    return plt.cm.jet(val)


def get_light_level(shadows, x, y):
    
    light_lvl = 1
    
    for s in shadows:
        start_x = s[0]
        start_y = s[1]
        end_x = s[2]
        end_y = s[3]
        strength = s[4]
        
        dx = end_x - start_x
        dy = end_y - start_y
        t = ((x - start_x) * dx + (y - start_y) * dy) / (dx * dx + dy * dy)
        
        if t < 0:
            closest_x = start_x
            closest_y = start_y
        
        elif t > 1:
            closest_x = end_x
            closest_y = end_y
        
        else:
            closest_x = start_x + t * dx
            closest_y = start_y + t * dy
        
        dist = np.sqrt((x - closest_x)**2 + (y - closest_y) ** 2)
    
        l_lvl = 1 - ( - 1 / strength * dist + 1)
        
        if l_lvl < 0:
            l_lvl = 0
        
        if l_lvl > 1:
            l_lvl = 1
        
        
        light_lvl *= l_lvl
        
    return light_lvl
        


def interpolate(color_a, color_b, t):
    # 'color_a' and 'color_b' are RGB tuples
    # 't' is a value between 0.0 and 1.0
    # this is a naive interpolation
    return tuple(int(a + (b - a) * t) for a, b in zip(color_a, color_b))

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

def plot_path(fpath, obs_path, true_obs_path, topo_path, shadow_path, output_path):

    fig = plt.figure()
    ax = fig.add_subplot(111)
    #os.path.join(OUTPUT_PATH, "topo.csv")
    path = parse_path(fpath)
    topo = parse_topo(topo_path)
    true_obs = parse_obs(true_obs_path)
    obs = parse_obs(obs_path)
    shads = parse_shadows(shadow_path)
    
    heights = [i[2] for i in topo]
    max_h = max(heights)
    
    # topology
    for i in range(len(topo)):
        x, y, h = topo[i]
        light_lvl = get_light_level(shads, x, y)
        c = jet( 1 - h / max_h)[0:3:1]
        base_color = [0, 0, 0]
        base_color[0] = int(c[0] * 255)
        base_color[1] = int(c[1] * 255)
        base_color[2] = int(c[2] * 255)
        color = interpolate((0, 0, 0), base_color, light_lvl)
        reformatted_color = (color[0] / 255, color[1] / 255, color[2] / 255)
        rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor=reformatted_color)
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
            rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor='grey')
        else:
            rect = patches.Rectangle(calculate_patch_center(x, y, OBS_SIDE_LENGTH, OBS_SIDE_LENGTH), OBS_SIDE_LENGTH, OBS_SIDE_LENGTH, linewidth=1, facecolor='black')
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
                  os.path.join(pdir, f"topo.csv"),
                  os.path.join(pdir, f"shadow_{i}.csv"),
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
    animate(OUTPUT_PATH, 0, NUM_FRAMES, delete_inputs_files=DELETE_FILES)
