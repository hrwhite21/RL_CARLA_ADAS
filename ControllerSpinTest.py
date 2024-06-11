import sys
sys.path.append('../logidrivepy')
from logidrivepy import LogitechController
import time

def spin_controller(controller):
    for i in range(-100, 100, 50):
        controller.LogiPlaySpringForce(0, i, 100, 100)
        controller.logi_update()
        time.sleep(0.5)

    # for k in range (-100,101,1):
    #     controller.LogiPlayConstantForce(0,i)
    #     controller.logi_update()
    #     time.sleep(0.1)


def spin_test():
    controller = LogitechController()

    controller.steering_initialize()
    print("\n---Logitech Spin Test---")
    spin_controller(controller)
    print("Spin test passed.\n")

    controller.steering_shutdown()


if __name__ == "__main__":
    spin_test()