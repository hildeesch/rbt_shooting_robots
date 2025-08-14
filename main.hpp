#ifndef MAIN_HPP_NADIEBFDIANBDU
#define MAIN_HPP_NADIEBFDIANBDU

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <iostream>

#include "./github_rplidar_drivers/rplidar_sdk/sdk/include/sl_lidar.h"
#include "./github_rplidar_drivers/rplidar_sdk/sdk/include/sl_lidar_driver.h"
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

#ifdef _WIN32
#include <Windows.h>
#define delay(x) ::Sleep(x)
#else
#include <unistd.h>
static inline void delay(sl_word_size_t ms)
{
    while (ms >= 1000)
    {
        usleep(1000 * 1000);
        ms -= 1000;
    };
    if (ms != 0)
        usleep(ms * 1000);
}
#endif

using namespace sl;

void print_usage(int argc, const char *argv[])
{
    printf("Usage:\n"
           " For serial channel\n %s --channel --serial <com port> [baudrate]\n"
           " The baudrate used by different models is as follows:\n"
           "  A1(115200),A2M7(256000),A2M8(115200),A2M12(256000),"
           "A3(256000),S1(256000),S2(1000000),S3(1000000)\n"
           " For udp channel\n %s --channel --udp <ipaddr> [port NO.]\n"
           " The T1 default ipaddr is 192.168.11.2,and the port NO.is 8089. Please refer to the datasheet for details.\n",
           argv[0], argv[0]);
}

bool checkSLAMTECLIDARHealth(ILidarDriver *drv)
{
    sl_result op_result;
    sl_lidar_response_device_health_t healthinfo;

    op_result = drv->getHealth(healthinfo);
    if (SL_IS_OK(op_result))
    { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("SLAMTEC Lidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == SL_LIDAR_STATUS_ERROR)
        {
            fprintf(stderr, "Error, slamtec lidar internal error detected. Please reboot the device to retry.\n");
            // enable the following code if you want slamtec lidar to be reboot by software
            // drv->reset();
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

bool ctrl_c_pressed = false;

class ScanPoint
{
public:
    ScanPoint(double angle, double dist) : angle{angle}, dist{dist}
    {
    }

    double angle, dist;
};

class CustomLidarDriver
{
public:
    CustomLidarDriver();

    std::vector<ScanPoint> get_scan();

private:
    ILidarDriver *drv;
    sl_result op_result;
    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count;
    int opt_channel_type;

};

namespace py = pybind11;

PYBIND11_MODULE(lidar_module, m){
    py::class_<ScanPoint>(m, "ScanPoint")
        .def(py::init<double, double>())
        .def_readwrite("angle", &ScanPoint::angle)
        .def_readwrite("dist", &ScanPoint::dist);

    py::class_<CustomLidarDriver>(m, "CustomLidarDriver")
        .def(py::init<>())
        .def("get_scan", &CustomLidarDriver::get_scan);
}


#endif