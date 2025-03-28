#include "serial_test/uSerial.h"
#include "serial_test/mJoint.h"

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

int Joint::deinit(void)
{

    int rc = 0;
    rc |= stop(1);
    rc |= disableCL();
    rc |= setHoldCurrent(0);
    rc |= setBrakeMode(0);
    this->fd = -1;
    return rc;
}

int Joint::printInfo(void)
{
    std::cout << "Name: " << this->name << " address: " << this->address << " fd: " << this->fd << std::endl;
    return 0;
}

int Joint::getPosition(float &angle)
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

int Joint::setPosition(float angle)
{
    u_int8_t buf[4];
    int32_t int_angle = angle * 100;
    memcpy(buf, &int_angle, 4);
    return write(MOVETOANGLE, buf, 4);
}

int Joint::moveSteps(int32_t steps)
{
    u_int8_t buf[4];
    memcpy(buf, &steps, 4);
    return write(MOVESTEPS, buf, 4);
}

int Joint::getVelocity(float &degps)
{
    u_int8_t buf[4];
    int32_t int_rpm;
    if (read(GETENCODERRPM, buf, 4) < 4)
    {
        return -1;
    }

    memcpy(&int_rpm, buf, 4);
    degps = 6.0 * int_rpm / 100;

    return 0;
}

int Joint::setVelocity(float degps)
{
    u_int8_t buf[4];
    int32_t int_rpm = degps * 100 / 6;
    memcpy(buf, &int_rpm, 4);
    return write(SETRPM, buf, 4);
}

int Joint::checkOrientation(const unsigned int timeout_ms, float angle)
{
    u_int8_t buf[4];
    int32_t int_angle = angle * 100;
    memcpy(buf, &int_angle, 4);
    return write(CHECKORIENTATION, buf, 4, timeout_ms);
}

int Joint::stop(bool mode)
{
    u_int8_t buf = mode;
    return write(STOP, &buf, 1);
}

int Joint::disableCL(void)
{
    u_int8_t buf = 0;
    return write(DISABLECLOSEDLOOP, &buf, 1);
}

int Joint::setDriveCurrent(u_int8_t current)
{
    return write(SETCURRENT, &current, 1);
}

int Joint::setHoldCurrent(u_int8_t current)
{
    return write(SETHOLDCURRENT, &current, 1);
}

int Joint::setBrakeMode(u_int8_t mode)
{
    return write(SETBRAKEMODE, &mode, 1);
}

int Joint::checkCom(void)
{
    u_int8_t buf;
    read(PING, &buf, 1);
    if (buf == 'O')
    {
        return 0;
    }
    return -1;
}

int Joint::read(const stp_reg_t reg, u_int8_t *data, const size_t data_length, const unsigned int timeout_ms)
{

    u_int8_t header_buf[3] = {this->address, reg, 0};
    tcflush(this->fd, TCIOFLUSH);
    if (writeToSerialPort(this->fd, header_buf, 3) < 0)
    {
        std::cerr << "Error writing to serial port: " << strerror(errno) << std::endl;
        return -1;
    }

    u_int8_t ack_buf;
    int n = readFromSerialPort(this->fd, &ack_buf, 1, timeout_ms);
    if (n <= 0)
    {
        std::cerr << "ERROR: No ACK received" << std::endl;
        return -1;
    }
    // std::cout << "DEBUG: Received header" << std::endl;

    // std::cout << "Read from serial port: " << std::string((char *)&ack_buf, n) << std::endl;
    // std::cout << "Read from serial port: " << ack_buf << std::endl;

    if (ack_buf != ACK)
    {
        std::cerr << "ERROR: NACK received" << std::endl;
        return -2; // NACK
    }

    // Add one byte for checksum
    u_int8_t *rx_buf = new u_int8_t[data_length + 1];
    n = readFromSerialPort(this->fd, rx_buf, data_length + 1, timeout_ms);
    if (n <= 0)
    {
        std::cerr << "ERROR: reading from serial port: " << strerror(errno) << std::endl;
        delete[] rx_buf;
        return -1;
    }
    // std::cout << "Read from serial port: " << std::string((char *)rx_buf, n) << std::endl;
    DUMP_BUFFER(rx_buf,data_length+1);
    if (rx_buf[data_length] != generateChecksum(rx_buf, data_length))
    {
        std::cerr << "ERROR: CHK failed" << std::endl;
        delete[] rx_buf;
        return -3; // CHK Error
    }

    memcpy(data, rx_buf, data_length);

    delete[] rx_buf;

    return n;
}

int Joint::write(const stp_reg_t reg, u_int8_t *data, const size_t data_length, const unsigned int timeout_ms)
{
    u_int8_t header_buf[3] = {this->address, reg, (u_int8_t)data_length};
    tcflush(this->fd, TCIOFLUSH);
    if (writeToSerialPort(this->fd, header_buf, 3) < 0)
    {
        std::cerr << "Error writing to serial port: " << strerror(errno) << std::endl;
        return -1; // R/W-error
    }

    u_int8_t ack_buf;
    int n = readFromSerialPort(this->fd, &ack_buf, 1, timeout_ms);
    if (n <= 0)
    {
        std::cerr << "ERROR: No ACK received" << std::endl;
        return -1; // R/W-error
    }
    // std::cout << "DEBUG: Received header" << std::endl;

    // std::cout << "Read from serial port: " << std::string((char *)&ack_buf, n) << std::endl;

    if (ack_buf != ACK)
    {
        std::cerr << "ERROR: NACK received" << std::endl;
        return -2; // NACK
    }

    // Add one byte for checksum
    u_int8_t *tx_buf = new u_int8_t[data_length + 1];
    memcpy(tx_buf, data, data_length);

    u_int8_t checksum = generateChecksum(tx_buf, data_length);

    tx_buf[data_length] = checksum;

    if (writeToSerialPort(this->fd, tx_buf, data_length + 1) < 0)
    {
        std::cerr << "Error writing to serial port: " << strerror(errno) << std::endl;
        delete[] tx_buf;
        return -1; // R/W-error
    }
    // std::cout << "Read from serial port: " << std::string((char *)tx_buf, n) << std::endl;

    n = readFromSerialPort(this->fd, &ack_buf, 1, timeout_ms);
    if (n <= 0)
    {
        std::cerr << "Error reading from serial port: " << strerror(errno) << std::endl;
        delete[] tx_buf;
        return -1; // R/W-error
    }

    if (ack_buf != ACK)
    {
        delete[] tx_buf;
        return -2; // NACK
    }

    delete[] tx_buf;

    return 0;
}
