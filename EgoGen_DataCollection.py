import carla
import numpy as np
import os
import platform
import random
import pygame

cc = carla.ColorConverter.CityScapesPalette


def camera_callback(camera_data, save_path):
    cam_save_path = f"{save_path}/rgb_cam_%06d.png"
    camera_data.save_to_disk(cam_save_path % camera_data.frame)

    return


def seg_camera_callback(semantic_segmentation_data, save_path):
    seg_save_path = f"{save_path}/seg_cam_%06d.png"
    semantic_segmentation_data.save_to_disk(seg_save_path % semantic_segmentation_data.frame, cc)
    return


def collision_callback(collision_data):
    print("Collision detected:\n" + str(collision_data) + '\n')


def invasion_callback(lane_invasion_data):
    print("Vehicle has left its lane:\n" + str(lane_invasion_data) + '\n')
    return


desired_town = 'Town_01Opt'
desired_fps = 30
client = carla.Client('localhost', 2000)
client.load_world(desired_town)

# Create and store all high level objects
world = client.get_world()
level = world.get_map()
weather = world.get_weather()
blueprint_library = world.get_blueprint_library()
settings = world.get_settings()
# Enabling synchronous mode required to get info from same timestep/frame when working with multiple sensors
settings.synchronous_mode = True  # Enables synchronous mode
# FPS = 1/fixed_delta_seconds
settings.fixed_delta_seconds = np.round(1 / desired_fps, 3)
traffic_manager = client.get_trafficmanager()
world.apply_settings(settings)

if platform.system == 'Windows':
    os_save_path = "C:/CarlaGitHub/RL_CARLA_ADAS/SavedData"
    sensor_os_save_path = "C:/CarlaGitHub/RL_CARLA_ADAS/SavedData/Sensors"
elif platform.system == 'Ubuntu':
    os_save_path = "/data/HunterWhite/CarlaUE4/Recordings"
    sensor_os_save_path = "/data/HunterWhite/CarlaUE4/Recordings/Sensors"

os.makedirs(sensor_os_save_path="C:/CarlaGitHub/RL_CARLA_ADAS/SavedData/Sensors",
            exist_ok=True)

# As an estimate, 1h recording with 50 traffic lights and 100 vehicles takes around 200MB in size.
spawn_points = world.get_map().get_spawn_points()

ego_bp = blueprint_library.filter('vehicle.mini.cooper_s_2021')
ego_bp.set_attribute('role_name', 'ego')
ego_color = random.choice(ego_bp.get_attribute('color').recommended_values)
ego_bp.set_attribute('color', ego_color)
ego_transform = random.shuffle(spawn_points)[0]
ego_vehicle = world.try_spawn_actor(ego_bp, ego_transform)

vehicles_bp = blueprint_library.filter('*vehicle*')

camera_bp = blueprint_library.find('sensor.camera.rgb')
camera_bp.set_attribute('image_size_x' '1280')
camera_bp.set_attribute('image_size_y' '720')
camera_bp.set_attribute('fov' '105')
camera_location = carla.Location(2, 0, 1)
camera_rotation = carla.Rotation(0, 180, 0)
camera_transform = carla.Transform(camera_location, camera_rotation)

segment_camera_bp = blueprint_library.find('sensor.camera.semantic_segmentation')
segment_camera_bp.set_attribute('image_size_x' '840')
segment_camera_bp.set_attribute('image_size_y' '600')
segment_camera_bp.set_attribute('fov' '90')
segment_camera_location = carla.Location(1.8, 0, 1.2)
segment_camera_rotation = carla.Rotation(0, 180, 0)
segment_camera_transform = carla.Transform(segment_camera_location, segment_camera_rotation)

collision_sensor_bp = blueprint_library.find('sensor.other.collision')
collision_sensor_location = carla.location(0, 0, 0)
collision_sensor_rotation = carla.Rotation(0, 0, 0)
collision_sensor_transformation = carla.Transform(collision_sensor_location, collision_sensor_rotation)

lane_invasion_sensor_bp = blueprint_library.find('sensor.other.lane_invasion')
invasion_sensor_location = carla.location(0, 0, 0)
invasion_sensor_rotation = carla.Rotation(0, 0, 0)
invasion_sensor_transformation = carla.Transform(invasion_sensor_location, invasion_sensor_rotation)

if ego_vehicle is None:
    print('\n Ego Vehicle failed to spawn')
else:
    print('\n Ego is spawned')

    try:
        ego_camera = world.spawn_actor(camera_bp, camera_transform, attach_to=ego_vehicle,
                                       attachment_type=carla.AttachmentType.Rigid)
        ego_camera.listen(lambda image: camera_callback(image, sensor_os_save_path))
        ego_segment_camera = world.spawn_actor(segment_camera_bp, segment_camera_transform, attach_to=ego_vehicle,
                                               attachment_type=carla.AttachmentType.Rigid)
        ego_segment_camera.listen(lambda seg_image: seg_camera_callback(seg_image, sensor_os_save_path))
        ego_collision_sensor = world.spawn_actor(collision_sensor_bp, collision_sensor_transformation,
                                                 attach_to=ego_vehicle, attachment_type=carla.AttachmentType.Rigid)
        ego_collision_sensor.listen(lambda coll_info: collision_callback(coll_info))
        ego_invasion_sensor = world.spawn_actor(lane_invasion_sensor_bp, invasion_sensor_transformation,
                                                attach_to=ego_vehicle, attachment_type=carla.AttachmentType.Rigid)
        ego_invasion_sensor.listen(lambda inv_info: invasion_callback(inv_info))

        print('\n Error Loading Vehicle Sensors')
    finally:
        print('\n Loading traffic!')

# Spawn in some traffic to keep things interesting
models = ['dodge', 'audi', 'model3', 'mini', 'mustang', 'lincoln', 'prius', 'nissan', 'crown', 'impala']
blueprints = []
for vehicle in world.get_blueprint_library().filter('*vehicle*'):
    if any(model in vehicle.id for model in models):
        blueprints.append(vehicle)

# Set a max number of vehicles and prepare a list for those we spawn
max_vehicles = 20
max_vehicles = min([max_vehicles, len(spawn_points)])
vehicles = []

# Take a random sample of the spawn points and spawn some vehicles
for i, spawn_point in enumerate(random.sample(spawn_points, max_vehicles)):
    temp = world.try_spawn_actor(random.choice(blueprints), spawn_point)
    if temp is not None:
        vehicles.append(temp)
    for vehicle in vehicles:
        vehicle.set_autopilot(True)

try:
    client.start_recorder(os_save_path, True)
    while True:
        world.tick()
        # Rest of Code to capture images

except KeyboardInterrupt:
    print('\n Data Collection Terminated, destroying actors and reloading world')
    for actor in carla.world.get_actors():
        try:
            actor.stop()
            actor.destroy()
        except:
            actor.destoy()

finally:
    if carla.world.get_actors() is not None:
        print('\n The following actors were not destroyed. Please manually delete them.\n')
        print(carla.world.get_actors())

    # DO the stuff to destroy the actors and free up computer resources.
