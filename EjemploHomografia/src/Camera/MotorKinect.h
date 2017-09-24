#ifndef KinectMotor_H
#define	KinectMotor_H


// OpenNI includes
#include "XnUSB.h"

#include <iostream>

using namespace std;

/**
 * Class to control Kinect's motor.
 */
class MotorKinect
{
public:
        MotorKinect();
        virtual ~MotorKinect();

        /**
         * Open device.
         * @return true if succeeded, false - overwise
         */
        bool Open();

        /**
         * Close device.
         */
        void Close();

        /**
         * Move motor up or down to specified angle value.
         * @param angle angle value
         * @return true if succeeded, false - overwise
         */
        bool Move(int angle);

	int Pose();

	int isOpen();

private:
        XN_USB_DEV_HANDLE m_dev;
        bool m_isOpen;

};


#endif
