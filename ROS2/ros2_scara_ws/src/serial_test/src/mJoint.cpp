#include "serial_test/uSerial.h"
#include "serial_test/mJoint.h"


// int write(u_int8_t address, u_int8_t register, const u_int8_t *data)
// {
//     // send adr + register
//     // await ACK
//     // send data
//     // await ACK
// }

// int read(u_int8_t address, u_int8_t register, const u_int8_t *data)
// {
//     // send adr + register
//     // await ACK
//     // await data
//     // send NACK
// }
Joint::Joint(u_int8_t address, std::string name)
{
    this->address = address;
    this->name = name;
}

int Joint::init(int fd)
{
    std::cout << "initializing " << this->name << std::endl;
    this->fd = fd;
    return checkCom();
}

int Joint::getAngle(float &angle)
{
    u_int8_t buf[4];
    int32_t int_angle;
    if (read(ANGLEMOVED, buf, 4) < 4)
    {
        return -1;
    }

    memcpy(&int_angle, buf, 4);
    angle = 1.0 * int_angle / 100;

    return 0;
}

int Joint::checkCom(void)
{
    u_int8_t buf;
    read(PING,&buf,1);
    if(buf == 'O'){
        return 0;
    }
    return -1;
}

int Joint::read(const stp_reg_t reg, u_int8_t *data, const size_t data_length)
{
    // Header contains: Slave Adress, Number of following bytes on read only one more byte is transmitted: the command byte.
    std::cout << "DEBUG: Sending header" << std::endl;
    u_int8_t header_buf[3] = {this->address, 1, reg};
    tcflush(this->fd, TCIOFLUSH);
    if (writeToSerialPort(this->fd, header_buf, sizeof(header_buf)) < 0)
    {
        std::cerr << "Error writing to serial port: " << strerror(errno) << std::endl;
    }

    u_int8_t ack_buf;
    int n = readFromSerialPort(this->fd, &ack_buf, 1, 100);
    if (n <= 0)
    {
        std::cerr << "Error reading from serial port: " << strerror(errno) << std::endl;
        return -1;
    }
    std::cout << "DEBUG: Received header" << std::endl;

    std::cout << "Read from serial port: " << std::string((char *)&ack_buf, n) << std::endl;

    // Add one byte for checksum
    u_int8_t *rx_buf = new u_int8_t[data_length + 1];
    n = readFromSerialPort(this->fd, rx_buf, data_length + 1, 100);
    if (n <= 0)
    {
        std::cerr << "Error reading from serial port: " << strerror(errno) << std::endl;
        return -1;
    }
    std::cout << "Read from serial port: " << std::string((char *)rx_buf, n) << std::endl;

    // check checksum
    memcpy(data, rx_buf, data_length);

    return n;
}

int Joint::write()
{
    return 0;
}
