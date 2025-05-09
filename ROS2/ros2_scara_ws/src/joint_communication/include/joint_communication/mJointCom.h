#ifndef MJOINTCOM_H
#define MJOINTCOM_H

#include <vector>
#include <iostream>
#include "joint_communication/mJoint.h"

class Joint_comms
{
public:
  Joint_comms(void);
  ~Joint_comms();

  int init(void);
  int deinit(void);

  /**
 * add joints to internal vector
 * @param addresses 1-byte device adress
 * @param names string device name
 * @param gearRatio gear ratio of joint
 * @param offset offset between encoder zero and joint zero (in joint units)

 */
  void addJoint(const int address, const std::string name, const float gearRatio, const int offset);

  /**
 * Initializes the drivers
 * @param driveCurrent_v vector of drive currents 0-100.
 * @param holdCurrent_v vector of hold currents 0-100.

 * @return error code.
 */
  int enables(std::vector<u_int8_t> driveCurrent_v, std::vector<u_int8_t> holdCurrent_v);

  /**
   * Initializes the drivers, with the same current settings for all
   * @param driveCurrent drive current 0-100.
   * @param holdCurrent hold current 0-100.
   * @return error code.
   */
  int enables(u_int8_t driveCurrent, u_int8_t holdCurrent);

  /**
   * disenganges the joint motors without closing i2c handle
   * @return error code.
   */
  int disables(void);

  /**
   * Homes a joint.
   * @param name joint name.
   * @param direction  CCW: 0, CW: 1.
   * @param rpm  speed of motor in rpm > 10
   * @param sensitivity Encoder stalldetect sensitivity - From -100 to 10 where lower number is less sensitive and higher is more sensitive
   * @param current homeing current, determines how easy it is to stop the motor and thereby provoke a stall
   * @return error code.
   */
  int home(std::string name, u_int8_t direction, u_int8_t rpm, int8_t sensitivity, u_int8_t current);

  int getPositions(std::vector<float> &angle_v);
  int setPositions(std::vector<float> angle_v);
  int getVelocities(std::vector<float> &degps_v);
  int setVelocities(std::vector<float> degps_v);

  /**
   * Sequentially checks the orientations of each joint.
   * @param angle_v vector of degrees to rotate to check the orientation.
   * @return error code.
   */
  int checkOrientations(std::vector<float> angle_v);

  /**
   * Overload to use standard angle of 10 degrees
   * @return error code.
   */
  int checkOrientations(float angle = 10.0);

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

  /**
   * @brief Enable encoder stall detection. A detected stall can be reset by homeing.
   * @param sensitivity Encoder stalldetect sensitivity - From -100 to 10 where lower number is less sensitive and higher is more sensitive
   */
  int enableStallguards(std::vector<u_int8_t> thresholds);

  // int home();

  std::vector<Joint> joints;

protected:
private:
};

#endif