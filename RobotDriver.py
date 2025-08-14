from lidar_module import CustomLidarDriver
from ABElectronics_Python_Libraries.ADCDifferentialPi import ADCDifferentialPi


import RPi.GPIO as GPIO
from mpu9250_jmdev.registers import *
from mpu9250_jmdev.mpu_9250 import MPU9250
import time


class RobotDriver:

    def __init__(self):
        self._custom_lidar_driver = CustomLidarDriver()

        GPIO.setmode(GPIO.BCM)
        # return
        
        # define the output pins for the different functionalities:
        _pin_left_motor_forwards = 17
        _pin_left_motor_backwards = 27
        _pin_right_motor_forwards = 22
        _pin_right_motor_backwards = 23

        _pin_canon_upwards = 24
        _pin_canon_downwards = 10
        self._pin_canon_shoot = 13
        self._pin_canon_button = 25

        # set all relevant pins as inputs and outputs
        GPIO.setup(_pin_left_motor_forwards, GPIO.OUT)
        self._output_left_motor_forwards = GPIO.PWM(_pin_left_motor_forwards, 1000)
        GPIO.setup(_pin_left_motor_backwards, GPIO.OUT)
        self._output_left_motor_backwards = GPIO.PWM(_pin_left_motor_backwards, 1000)
        GPIO.setup(_pin_right_motor_forwards, GPIO.OUT)
        self._output_right_motor_forwards = GPIO.PWM(_pin_right_motor_forwards, 1000)
        GPIO.setup(_pin_right_motor_backwards, GPIO.OUT)
        self._output_right_motor_backwards = GPIO.PWM(_pin_right_motor_backwards, 1000)
        GPIO.setup(_pin_canon_upwards, GPIO.OUT)
        self._output_canon_upwards = GPIO.PWM(_pin_canon_upwards, 1000)
        GPIO.setup(_pin_canon_downwards, GPIO.OUT)
        self._output_canon_downwards = GPIO.PWM(_pin_canon_downwards, 1000)

        # Set up adc for potentiometer
        self.adc = ADCDifferentialPi(0x6a, 0x68, 12)
        
        # Create an MPU9250 instance
        self.mpu = MPU9250(
            address_ak=AK8963_ADDRESS,
            address_mpu_master=MPU9050_ADDRESS_68,  # In case the MPU9250 is connected to another I2C device
            address_mpu_slave=None,
            bus=1,
            gfs=GFS_1000,
            afs=AFS_8G,
            mfs=AK8963_BIT_16,
            mode=AK8963_MODE_C100HZ)

        # Configure the MPU9250
        self.mpu.configure()


    def __del__(self):
        # GPIO.cleanup()
        pass
    
    def get_pot_volt(self):
        return self.adc.read_voltage(8)
    
    def rotate_canon(self, vel):

        assert vel >= -1
        assert vel <= 1

        if vel > 0:
            self._output_canon_upwards.start(abs(vel)*100)
            self._output_canon_downwards.start(0)
        else:
            self._output_canon_upwards.start(0)
            self._output_canon_downwards.start(abs(vel)*100)
    

    def get_laser_scan(self):
        return self._custom_lidar_driver.get_scan()

    def set_motor_velocities(self, right_wheel_velocity, left_wheel_velocity):

        assert abs(right_wheel_velocity) <= 1.0
        assert abs(left_wheel_velocity) <= 1.0

        if right_wheel_velocity > 0:
            self._output_right_motor_forwards.start(abs(right_wheel_velocity)*100)
            self._output_right_motor_backwards.start(0.)
        else:
            self._output_right_motor_forwards.start(0.)
            self._output_right_motor_backwards.start(abs(right_wheel_velocity)*100)
        
        if left_wheel_velocity > 0:
            self._output_left_motor_forwards.start(abs(left_wheel_velocity)*100)
            self._output_left_motor_backwards.start(0.)
        else:
            self._output_left_motor_forwards.start(0.)
            self._output_left_motor_backwards.start(abs(left_wheel_velocity)*100)

    def shoot_canon(self):
        GPIO.setup(self._pin_canon_shoot, GPIO.OUT)
        _output_canon_shoot = GPIO.PWM(self._pin_canon_shoot, 1000)
        _output_canon_shoot.start(100)
        time.sleep(3)
        _output_canon_shoot.start(0)
        GPIO.setup(self._pin_canon_shoot, GPIO.IN) # turn the pin to input to prevent inerference from causing the canon to shoot...
        
    def get_imu_data(self):
        accel_data = self.mpu.readAccelerometerMaster()
        gyro_data = self.mpu.readGyroscopeMaster()
        mag_data = self.mpu.readMagnetometerMaster()
        return accel_data, gyro_data, mag_data
