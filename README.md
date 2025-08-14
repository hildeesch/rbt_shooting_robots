# Software installation and testing
## To install the software, run the following commands
```bash
sudo apt update
sudo apt install python3-build python3-installer git
```
## Setting up the software architecture
```bash
mkdir rp_lidar
cd rp_lidar
```
## Installing the rp_lidar software:
```bash
git clone https://github.com/Slamtec/rplidar_sdk.git
cd rplidar_sdk
make
cd ..
```
## Installing the robotDriver software and test script
```bash
cd rp_lidar
git clone https://github.com/hildeesch/rbt_shooting_robots.git
```

## Installing the potmeter software 
For more info, check: https://github.com/abelectronicsuk/ABElectronics_Python_Libraries
```bash
cd rp_lidar
git clone https://github.com/abelectronicsuk/ABElectronics_Python_Libraries.git
cd ABElectronics_Python_Libraries
python3 -m build
sudo python3 -m installer dist/*.whl
cd ..
```
## Installing the IMU software
```bash
pip install mpu9250-jmdev
```
## To test the software and hardware 
NOTE: it will start driving
The prints tell you what the robot should be doing. Check if there are any mismatches between the prints and the behaviour.
```bash
ssh into the raspberry
cd rp_lidar
python3 building_day_test_script.py
```