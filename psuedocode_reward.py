'''
after world.tick()
retrieve sensor information
if (vehicle collision)
    reward = big negative or 0 and end episode
Option 1: Reward based on reaching goal.
 build reward from distance to end goal, lane invasion, etc
 reward = -(distance to goal) -1(invaded_lane) + vehicle_speed if vehicle_speed >= 5 else + 0

 Option 2: Reward based on driving time
 reward = 1  if vehicle velocity > small value
    Can add in reward for being not far away from lane center?

    Can subtract reward for if waypoint.lane_type is not in [Driving, Stop, Parking, Bidirectional, Offramp, OnRamp]
    waypoint = world.get_map().get_waypoint(vehicle.get_location(),project_to_road=False,
      # returns the exact location of the vehicle and not the location of the center of the lane closest to vehicle ...
    lane_type=(carla.LaneType.Driving | carla.LaneType.Shoulder | carla.LaneType.Sidewalk))
        Can restrict lane to certain types.











'''