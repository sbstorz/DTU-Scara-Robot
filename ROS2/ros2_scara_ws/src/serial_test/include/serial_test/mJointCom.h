#ifndef MJOINTCOM_H
#define MJOINTCOM_H

#include <vector>
// #include <errno.h>
#include <iostream>
// #include <unistd.h>
#include "serial_test/mJoint.h"



class Joint_comms
{
public:
  /**
   * Constructor to create a join communication object
   * @param n number of joints to connect to.
   * @param addresses n-sized array of 1-byte device adresses
   * @param names n-sized array of string device names
   * @return Joint_comms object.
   */
  Joint_comms(size_t n, u_int8_t addresses[], std::string names[]);
  ~Joint_comms();

  int init(const char *portname, unsigned int baudrate);
  int deinit(void);
  int getPositions(std::vector<float> &angle_v);
  int setPositions(std::vector<float> angle_v);
  int getVelocities(std::vector<float> &degps_v);
  int setVelocities(std::vector<float> degps_v);

  /**
   * Sequentially checks the orientations of each joint.
   * @param angle_v vector of degrees to rotate to check the orientation.
   * @param timeout_ms a per joint (!) timeout, since the orientation check is
   * blocking on the joint controller one has to wait min. 500ms to receive a ACK indicating successfull execution.
   * @return error code.
   */
  int checkOrientations(std::vector<float> angle_v, const unsigned int timeout_ms = 500);

  /**
   * Overload to use standard angle of 10 degrees
   * @param timeout_ms a per joint (!) timeout, since the orientation check is
   * blocking on the joint controller one has to wait min. 500ms to receive a ACK indicating successfull execution.
   * @return error code.
   */
  int checkOrientations(const unsigned int timeout_ms = 500);

  /**
   * Stops the motors
   * @param mode Hard: 0, Soft: 1
   * @return error code.
   */
  int stops(bool mode);
  /**
   * Disables the Closed-Loop PID Controllers
   * @return error code.
   */
  int disableCLs(void);

  /**
   * Set the Drive Currents
   * @param current 0% - 100% of driver current
   * @return error code.
   */
  int setDriveCurrents(std::vector<u_int8_t> current);

  /**
   * Overload to set all drive currents to the same value
   * @param current 0% - 100% of driver current
   * @return error code.
   */
  int setDriveCurrents(u_int8_t current);

  /**
   * Set the Hold Currents
   * @param current 0% - 100% of driver current
   * @return error code.
   */
  int setHoldCurrents(std::vector<u_int8_t> current);

  /**
   * Overload to set all hold currents to the same value
   * @param current 0% - 100% of driver current
   * @return error code.
   */
  int setHoldCurrents(u_int8_t current);

  /**
   * Set Brake Modes
   * @param mode Freewheel: 0, Coolbrake: 1, Hardbrake: 2
   * @return error code.
   */
  int setBrakeModes(u_int8_t mode);

  // int home();

  std::vector<Joint> joints;

protected:
private:
  int fd;
};

#endif