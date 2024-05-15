'''
Most important part - going to be getting images and actor information to the ML Agent without
totally bricking the CARLA Simulation time.

inputs --> RGB Image, Semantic Segmentation Image, vehicle location, vehicle rotation, vehicle velocity, distance to nearest waypoint, distance to end position.
    [600,600,3 (4)?]    [600,600,3 (4)?]        [x,y,z],        [pitch,roll,yaw]    [x,y,z]             [Euclidean norm?]               [Euclidean norm?]
outputs --> Control actions: Steering, acceleration, braking, 
                             [-1, 1],  [0,1]         [0,1]

while crashed == False
    world.tick()
    


'''