#include "./main.hpp"


int main(int argc, const char *argv[])
{
    CustomLidarDriver lidar_driver {};

    printf( "lidar driver constructed \n");
    while (1){
    auto scan_points = lidar_driver.get_scan();

    for (auto sp : scan_points){
        std::cout << sp.angle << '\n';
        std::cout << sp.dist << '\n';
    }
    }

    return 0;
}


CustomLidarDriver::CustomLidarDriver()
    {
        const char *opt_is_channel = "--channel";
        const char *opt_channel = "--serial";
        const char *opt_channel_param_first = "/dev/ttyUSB0";
        sl_u32 opt_channel_param_second = 115200;
        sl_u32 baudrateArray[2] = {115200, 256000};
        this->opt_channel_type = CHANNEL_TYPE_SERIALPORT;

        bool useArgcBaudrate = false;

        IChannel *_channel;

        printf("Ultra simple LIDAR data grabber for SLAMTEC LIDAR.\n"
               "Version: %s\n",
               SL_LIDAR_SDK_VERSION);

        if (opt_channel_type == CHANNEL_TYPE_SERIALPORT)
        {
            if (!opt_channel_param_first)
            {
#ifdef _WIN32
                // use default com port
                opt_channel_param_first = "\\\\.\\com3";
#elif __APPLE__
                opt_channel_param_first = "/dev/tty.SLAB_USBtoUART";
#else
                opt_channel_param_first = "/dev/ttyUSB0";
#endif
            }
        }

        // create the driver instance
        this->drv = *createLidarDriver();

        if (!this->drv)
        {
            fprintf(stderr, "insufficent memory, exit\n");
            exit(-2);
        }

        sl_lidar_response_device_info_t devinfo;
        bool connectSuccess = false;
        if (opt_channel_type == CHANNEL_TYPE_SERIALPORT)
        {
            if (useArgcBaudrate)
            {
                _channel = (*createSerialPortChannel(opt_channel_param_first, opt_channel_param_second));
                if (SL_IS_OK((this->drv)->connect(_channel)))
                {
                    this->op_result = this->drv->getDeviceInfo(devinfo);

                    if (SL_IS_OK(this->op_result))
                    {
                        connectSuccess = true;
                    }
                    else
                    {
                        delete this->drv;
                        this->drv = NULL;
                    }
                }
            }
            else
            {
                size_t baudRateArraySize = (sizeof(baudrateArray)) / (sizeof(baudrateArray[0]));
                for (size_t i = 0; i < baudRateArraySize; ++i)
                {
                    _channel = (*createSerialPortChannel(opt_channel_param_first, baudrateArray[i]));
                    if (SL_IS_OK((this->drv)->connect(_channel)))
                    {
                        this->op_result = this->drv->getDeviceInfo(devinfo);

                        if (SL_IS_OK(this->op_result))
                        {
                            connectSuccess = true;
                            break;
                        }
                        else
                        {
                            delete this->drv;
                            this->drv = NULL;
                        }
                    }
                }
            }
        }
        else if (opt_channel_type == CHANNEL_TYPE_UDP)
        {
            _channel = *createUdpChannel(opt_channel_param_first, opt_channel_param_second);
            if (SL_IS_OK((this->drv)->connect(_channel)))
            {
                this->op_result = this->drv->getDeviceInfo(devinfo);

                if (SL_IS_OK(op_result))
                {
                    connectSuccess = true;
                }
                else
                {
                    delete this->drv;
                    this->drv = NULL;
                }
            }
        }

        if (!connectSuccess)
        {
            (opt_channel_type == CHANNEL_TYPE_SERIALPORT) ? (fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n", opt_channel_param_first)) : (fprintf(stderr, "Error, cannot connect to the specified ip addr %s.\n", opt_channel_param_first));

            goto on_finished;
        }

        // print out the device serial number, firmware and hardware version number..
        printf("SLAMTEC LIDAR S/N: ");
        for (int pos = 0; pos < 16; ++pos)
        {
            printf("%02X", devinfo.serialnum[pos]);
        }

        printf("\n"
               "Firmware Ver: %d.%02d\n"
               "Hardware Rev: %d\n",
               devinfo.firmware_version >> 8, devinfo.firmware_version & 0xFF, (int)devinfo.hardware_version);

        // check health...
        if (!checkSLAMTECLIDARHealth(this->drv))
        {
            goto on_finished;
        }

        if (opt_channel_type == CHANNEL_TYPE_SERIALPORT)
            this->drv->setMotorSpeed();

        // start scan...
        this->drv->startScan(0, 1);

        this->count = _countof(this->nodes);

    on_finished:
        return;
    }


std::vector<ScanPoint> CustomLidarDriver::get_scan()
    {

        std::vector<ScanPoint> result{};

        // fetech result and print it out...

        this->op_result = this->drv->grabScanDataHq(nodes, count);

        if (SL_IS_OK(this->op_result))
        {
            this->drv->ascendScanData(nodes, count);
            for (int pos = 0; pos < (int)count; ++pos)
            {
                // printf("%s theta: %03.2f Dist: %08.2f Q: %d \n",
                //     (nodes[pos].flag & SL_LIDAR_RESP_HQ_FLAG_SYNCBIT) ?"S ":"  ",
                //     (nodes[pos].angle_z_q14 * 90.f) / 16384.f,
                //     nodes[pos].dist_mm_q2/4.0f,
                //     nodes[pos].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);

                // make scanpoint with data
                ScanPoint sp{(nodes[pos].angle_z_q14 * 90.f) / 16384.f, nodes[pos].dist_mm_q2 / 4.0f};

                // add to result
                result.push_back(sp);
            }
        }

        return result;
    }
