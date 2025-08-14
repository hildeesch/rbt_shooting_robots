# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++11 -fPIC `python3 -m pybind11 --includes`
LDFLAGS = `python3-config --ldflags`

# Paths and files
MAIN_SRC = main.cpp
DRIVER_DIR = ./github_rplidar_drivers/rplidar_sdk
DRIVER_LIB = $(DRIVER_DIR)/output/Linux/Release/libsl_lidar_sdk.a
TARGET = lidar_module$(shell python3-config --extension-suffix)

# Build the target
all: $(TARGET)

# Link pybind_module.cpp and main.cpp with the pre-built driver library
$(TARGET): $(MAIN_SRC) $(DRIVER_LIB)
	$(CXX) $(CXXFLAGS) -shared -o $@ $(MAIN_SRC) $(PYBIND_SRC) \
		-L$(DRIVER_DIR)/output/Linux/Release -lsl_lidar_sdk $(LDFLAGS)

# Build the driver library by calling the driver's Makefile
$(DRIVER_LIB):
	$(MAKE) -C $(DRIVER_DIR)

# Clean both main project and driver files
clean:
	rm -f $(TARGET)
	$(MAKE) -C $(DRIVER_DIR) clean
