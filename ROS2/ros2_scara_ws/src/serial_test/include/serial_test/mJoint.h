#ifndef MJOINT_H
#define MJOINT_H

// #include <stdio.h>

#define DUMP_BUFFER(buffer, size)                 \
  {                                               \
    std::cout << "Buffer dump: "; \
    for (size_t i = 0; i < size; i++)             \
    {                                             \
      printf("%#x ", buffer[i]);                    \
    }                                             \
    std::cout << std::endl;                       \
  }

class Joint
{
public:
  Joint(u_int8_t address, std::string name);
  // ~Joint();

  int init(int fd);
  int deinit(void);
  int printInfo(void);
  int getPosition(float &angle);
  int setPosition(float angle);
  int getVelocity(float &degps);
  int setVelocity(float degps);
  int checkOrientation(const unsigned int timeout_ms, float angle = 10);

  /**
   * Stops the motor
   * @param mode Hard: 0, Soft: 1
   * @return error code.
   */
  int stop(bool mode);
  /**
   * Disables the Closed-Loop PID Controller
   * @return error code.
   */
  int disableCL(void);

  /**
   * Set the Drive Current
   * @param current 0% - 100% of driver current
   * @return error code.
   */
  int setDriveCurrent(u_int8_t current);

  /**
   * Set the Hold Current
   * @param current 0% - 100% of driver current
   * @return error code.
   */
  int setHoldCurrent(u_int8_t current);

  /**
   * Set Brake Mode
   * @param mode Freewheel: 0, Coolbrake: 1, Hardbrake: 2
   * @return error code.
   */
  int setBrakeMode(u_int8_t mode);

  int moveSteps(int32_t steps);
  int checkCom(void);

  // int setPosition();
  // int home();

  std::string name;

protected:
private:
  enum stp_reg_t
  {
    PING = 0x0f,
    SETUP = 0x10,
    SETRPM = 0x11,
    GETDRIVERRPM = 0x12,
    MOVESTEPS = 0x13,
    MOVEANGLE = 0x14,
    MOVETOANGLE = 0x15,
    GETMOTORSTATE = 0x16,
    RUNCOTINOUS = 0x17,
    ANGLEMOVED = 0x18,
    SETCURRENT = 0x19,
    SETHOLDCURRENT = 0x1A,
    SETMAXACCELERATION = 0x1B,
    SETMAXDECELERATION = 0x1C,
    SETMAXVELOCITY = 0x1D,
    ENABLESTALLGUARD = 0x1E,
    DISABLESTALLGUARD = 0x1F,
    CLEARSTALL = 0x20,
    ISSTALLED = 0x21,
    SETBRAKEMODE = 0x22,
    ENABLEPID = 0x23,
    DISABLEPID = 0x24,
    ENABLECLOSEDLOOP = 0x25,
    DISABLECLOSEDLOOP = 0x26,
    SETCONTROLTHRESHOLD = 0x27,
    MOVETOEND = 0x28,
    STOP = 0x29,
    GETPIDERROR = 0x2A,
    CHECKORIENTATION = 0x2B,
    GETENCODERRPM = 0x2C
  };

  int read(const stp_reg_t reg, u_int8_t *data, const size_t data_length, const unsigned int timeout_ms = 100);
  int write(const stp_reg_t reg, u_int8_t *data, const size_t data_length, const unsigned int timeout_ms = 100);

  u_int8_t address;
  int multiplier = 1;
  int offset = 0;
  int range[2] = {0, 0};

  int fd;
};

#endif