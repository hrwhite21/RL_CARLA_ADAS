import os
import sys
import glob

try:
    sys.path.append(glob.glob('../WindowsNoEditor/PythonAPI/carla/dist/carla-*%d.%d-%s.egg' % (
        sys.version_info.major,
        sys.version_info.minor,
        'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    print('Couldn\'t import Carla egg properly')

import carla
from simulation.settings import PORT, TIMEOUT, HOST

class ClientConnection:
    def __init__(self, town, sync_mode):
        self.client = None
        self.town = town
        self.synchronus = sync_mode

    def setup(self):
        try:

            # Connecting to the  Server
            self.client = carla.Client(HOST, PORT)
            self.client.set_timeout(TIMEOUT)
            world = self.client.get_world()
            new_world_settings = world.get_settings()
            new_world_settings.synchronous_mode = self.synchronus
            if self.synchronus:
                new_world_settings.fixed_delta_seconds = 0.05 # can change this val later
 
            self.world = self.client.load_world(self.town)
            self.world.set_weather(carla.WeatherParameters.CloudyNoon)
            self.world.apply_settings(new_world_settings)
            if self.town == "Town07":
                loc_rot = carla.Transform(carla.Location(x=72.651,y=-5.4441,z=62.4744),
                                          carla.Rotation(pitch=-55.919,roll=8.2287e-05,yaw=-78.21569))
                self.world.get_spectator().set_transform(loc_rot)
            world.tick() if self.synchronus else None
            return self.client, self.world

        except Exception as e:
            print(
                'Failed to make a connection with the server: {}'.format(e))
            self.error()

    # An error method: prints out the details if the client failed to make a connection
    def error(self):

        print("\nClient version: {}".format(
            self.client.get_client_version()))
        print("Server version: {}\n".format(
            self.client.get_server_version()))

        if self.client.get_client_version != self.client.get_server_version:
            print(
                "There is a Client and Server version mismatch! Please install or download the right versions.")
