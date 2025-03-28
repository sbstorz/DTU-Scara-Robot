#include "serial_test/uSerial.h"
#include "serial_test/mJointCom.h"

static int convert_baudrate(unsigned int br)
{
    switch (br)
    {
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    default:
        std::cout << "Error! Baud rate " << br << " not supported! Default to 9600" << std::endl;
        return B9600;
    }
}

Joint_comms::Joint_comms(size_t n, u_int8_t addresses[], std::string names[])
{
    for (size_t i = 0; i < n; i++)
    {
        this->joints.push_back(Joint(addresses[i], names[i]));
    }
}

Joint_comms::~Joint_comms()
{
    if(this->fd != -1){
        closeSerialPort(this->fd);
        this->fd = -1;
    }    
}

int Joint_comms::init(const char *portname, unsigned int baudrate)
{
    this->fd = openSerialPort(portname);
    if (this->fd < 0)
    {
        std::cerr << "Failed to open serial port:" << portname << std::endl;
        return -1;
    }
    if (!configureSerialPort(this->fd, convert_baudrate(baudrate)))
    {
        closeSerialPort(this->fd);
        this->fd = -1;
        return -1;
    }

    // Init each joint and test connection to each joint by pinging
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        if (this->joints[i].init(this->fd) < 0)
        {
            std::cerr << "Failed to connect to: " << this->joints[i].name << std::endl;
            return -1;
        }
    }

    std::cout << "Joint Initialization successfull" << std::endl;

    return 0;
}

int Joint_comms::deinit()
{
    
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        if (this->joints[i].deinit() < 0)
        {
            std::cerr << "Failed to deinit: " << this->joints[i].name << std::endl;
            return -1;
        }
    }

    if(this->fd != -1){
        closeSerialPort(this->fd);
        this->fd = -1;
    } 

    std::cout << "Joint Deinitialization successfull" << std::endl;

    return 0;
}

int Joint_comms::getPositions(std::vector<float> &angle_v)
{
    if (angle_v.size() != this->joints.size())
    {
        std::cerr << "vector size mismatch" << std::endl;
        return -2;
    }

    for (size_t i = 0; i < this->joints.size(); i++)
    {
        float a;
        if (this->joints[i].getPosition(a) < 0)
        {
            std::cerr << "Failed to get angle from: " << this->joints[i].name << std::endl;
            return -1;
        }
        angle_v[i] = a;
    }
    return 0;
}

int Joint_comms::setPositions(std::vector<float> angle_v)
{
    if (angle_v.size() != this->joints.size())
    {
        std::cerr << "vector size mismatch" << std::endl;
        return -2;
    }

    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].setPosition(angle_v[i]);
        if (err < 0)
        {
            std::cerr << "Failed to set angle for: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::getVelocities(std::vector<float> &degps_v)
{
    if (degps_v.size() != this->joints.size())
    {
        std::cerr << "vector size mismatch" << std::endl;
        return -2;
    }

    for (size_t i = 0; i < this->joints.size(); i++)
    {
        float a;
        if (this->joints[i].getVelocity(a) < 0)
        {
            std::cerr << "Failed to get speed from: " << this->joints[i].name << std::endl;
            return -1;
        }
        degps_v[i] = a;
    }
    return 0;
}

int Joint_comms::setVelocities(std::vector<float> degps_v)
{
    if (degps_v.size() != this->joints.size())
    {
        std::cerr << "vector size mismatch" << std::endl;
        return -2;
    }

    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].setVelocity(degps_v[i]);
        if (err < 0)
        {
            std::cerr << "Failed to set speed for: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}


int Joint_comms::checkOrientations(std::vector<float> angle_v, const unsigned int timeout_ms)
{
    if (angle_v.size() != this->joints.size())
    {
        std::cerr << "vector size mismatch" << std::endl;
        return -2;
    }

    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].checkOrientation(angle_v[i], timeout_ms);
        if (err < 0)
        {
            std::cerr << "Failed to check orientation for: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}


int Joint_comms::checkOrientations(const unsigned int timeout_ms)
{
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].checkOrientation(timeout_ms);
        if (err < 0)
        {
            std::cerr << "Failed to check orientation for: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::stops(bool mode){
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].stop(mode);
        if (err < 0)
        {
            std::cerr << "Failed to stop motor: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::disableCLs(void)
{
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].disableCL();
        if (err < 0)
        {
            std::cerr << "Failed to disable closed loop for motor: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::setDriveCurrents(std::vector<u_int8_t> current)
{
    if (current.size() != this->joints.size())
    {
        std::cerr << "vector size mismatch" << std::endl;
        return -2;
    }

    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].setDriveCurrent(current[i]);
        if (err < 0)
        {
            std::cerr << "Failed to set drive current for motor: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::setDriveCurrents(u_int8_t current)
{
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].setDriveCurrent(current);
        if (err < 0)
        {
            std::cerr << "Failed to set drive current for motor: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::setHoldCurrents(std::vector<u_int8_t> current)
{
    if (current.size() != this->joints.size())
    {
        std::cerr << "vector size mismatch" << std::endl;
        return -2;
    }

    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].setHoldCurrent(current[i]);
        if (err < 0)
        {
            std::cerr << "Failed to set hold current for motor: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::setHoldCurrents(u_int8_t current)
{
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].setHoldCurrent(current);
        if (err < 0)
        {
            std::cerr << "Failed to set hold current for motor: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}

int Joint_comms::setBrakeModes(u_int8_t mode)
{
    for (size_t i = 0; i < this->joints.size(); i++)
    {
        int err = this->joints[i].setBrakeMode(mode);
        if (err < 0)
        {
            std::cerr << "Failed to set brake mode for motor: " << this->joints[i].name << " - error: " << err << std::endl;
            return err;
        }
    }
    return 0;
}
