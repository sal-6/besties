import math
import matplotlib
import numbers


def calculate_radiator_size(internal_heat, alpha_rad, epsilon_rad, Q_solar, Q_albedo, Q_earthir, temp_sc, temp_env):
    denom = (- Q_solar * alpha_rad - Q_albedo * alpha_rad - Q_earthir * epsilon_rad + epsilon_rad * 5.67e-8 * (temp_sc ** 4 - temp_env ** 4)) 
    radiator_size = internal_heat / denom

    return radiator_size


def main():
    
    coatings = [
        {
            "name": "Optical Solar Reflector",
            "alpha": 0.09,
            "epsilon": 0.78,
        },
        {
            "name": "White Paint",
            "alpha": 0.2,
            "epsilon": 0.92,
        },
        {
            "name": "Black Paint",
            "alpha": 0.92,
            "epsilon": 0.89,
        }
    ]

    envs = [
        {
            "name": "Nadir Radiator Earth Oriented",
            "albedo": 0.3 * 1419,
            "earth_ir": 239,
            "solar": 0
        },
        {
            "name": "L3 Anti Sun Face",
            "albedo": 0,
            "earth_ir": 0,
            "solar": 0
        },
        {
            "name": "Sun Facing",
            "albedo": 0,
            "earth_ir": 0,
            "solar": 1419,
        }
    ]

    internal_heat = 50
    temp_sc = 40 + 273.15
    temp_env = 3

    for env in envs:
        for coating in coatings:
            radiator_size = calculate_radiator_size(internal_heat, coating["alpha"], coating["epsilon"], 
                                                    env["solar"], env["albedo"], env["earth_ir"], temp_sc, temp_env)
            print("Radiator Size for {} in {} is {}".format(coating["name"], env["name"], radiator_size))

if __name__ == "__main__":
    main()


