from RobotDriver import RobotDriver


# temp code for testing:
import time

robot_driver = RobotDriver()

# Shoot 
robot_driver.shoot_canon()
# Get laser data
s = robot_driver.get_laser_scan()
for sp in s:
    print("dist", sp.dist)
    print("angle", sp.angle)
# Get pot voltage
pot_volt = robot_driver.get_pot_volt()
print("Potentiometer voltage: ",pot_volt)
# Get IMU data
accel_data, gyro_data, mag_data = robot_driver.get_imu_data()
print("IMU Data:")
print(accel_data)
print(gyro_data)
print(mag_data)

# Drive forward
print("Driving forward...")
robot_driver.set_motor_velocities(1.0, 1.0)
start_time = time.time()
while(True):
    if time.time() - start_time > 2.0:
        break
robot_driver.set_motor_velocities(0., 0.)
# Drive backward
print("Driving backward...")
robot_driver.set_motor_velocities(-1., -1.)
start_time = time.time()
while(True):
    if time.time() - start_time > 2.0:
        break
robot_driver.set_motor_velocities(0., 0.)
# Turn left
print("Turning left...")
robot_driver.set_motor_velocities(0.3, 1.0)
start_time = time.time()
while(True):
    if time.time() - start_time > 2.0:
        break
robot_driver.set_motor_velocities(0., 0.)
# Turn right
print("Turning right...")
robot_driver.set_motor_velocities(1.0, 0.3)
start_time = time.time()
while(True):
    if time.time() - start_time > 2.0:
        break
robot_driver.set_motor_velocities(0., 0.)

# Rotating cannon upward
print("Rotating cannon upward...")
robot_driver.rotate_canon(0.5)
start_time = time.time()
while(True):
    if time.time() - start_time > 1.0:
        break
robot_driver.rotate_canon(0.)

# Rotating cannon downward
print("Rotating cannon downward...")
robot_driver.rotate_canon(-0.5)
start_time = time.time()
while(True):
    if time.time() - start_time > 1.0:
        break
robot_driver.rotate_canon(0.)


print("Test sequence completed!")
