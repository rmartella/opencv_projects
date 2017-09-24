#include "MotorKinect.h"

MotorKinect::MotorKinect()
{
	m_isOpen = false;

}

MotorKinect::~MotorKinect()
{
	Close();

}

bool MotorKinect::Open()
{
	const XnUSBConnectionString *paths;
	XnUInt32 count;
	XnStatus res;

	// Init OpenNI USB
	res = xnUSBInit();
	if (res != XN_STATUS_OK) {
		clog << "Error: [KinectMotor.h] " << xnGetStatusString(res) << "xnUSBInit failed" << endl;
		return false;
	}

	// Open "Kinect motor" USB device
	res = xnUSBEnumerateDevices(0x045E /* VendorID */, 0x02B0 /*ProductID*/, &paths, &count);
	if (res != XN_STATUS_OK) {
		clog << "Error: [KinectMotor.h] " << xnGetStatusString(res) << "xnUSBEnumerateDevices failed" <<endl;
		return false;
	}

	// Open first found device
	res = xnUSBOpenDeviceByPath(paths[0], &m_dev);
	if (res != XN_STATUS_OK) {
		clog << "Error: [KinectMotor.h] " << xnGetStatusString(res) << "xnUSBOpenDeviceByPath failed" << endl;
		return false;
	}

	XnUChar buf[1]; // output buffer

	// Init motor
	res = xnUSBSendControl(m_dev, (XnUSBControlType) 0xc0, 0x10, 0x00,0x00, buf, sizeof(buf), 0);
	if (res != XN_STATUS_OK) {
		clog << "Error: [KinectMotor.h] " << xnGetStatusString(res) << "xnUSBSendControl failed" << endl;
		Close();
		return false;
	}

	res = xnUSBSendControl(m_dev, XN_USB_CONTROL_TYPE_VENDOR, 0x06, 0x01, 0x00, NULL,	0, 0);

	if (res != XN_STATUS_OK) {
		clog<<"Error: [KinectMotor.h] " << xnGetStatusString(res)<< "xnUSBSendControl failed" << endl;
		Close();
		return false;
	}
	m_isOpen = true;

	return true;

}

void MotorKinect::Close()
{
	if (m_isOpen) {
		xnUSBCloseDevice(m_dev);
		m_isOpen = false;
	}

}

bool MotorKinect::Move(int angle)
{
	bool moved;
	XnStatus res;

	moved = false;

	if ( m_isOpen )
	{
		// Send move control request
		res = xnUSBSendControl(m_dev, XN_USB_CONTROL_TYPE_VENDOR, 0x31,	angle, 0x00, NULL, 0, 0);
		if (res != XN_STATUS_OK)
			clog << "Error: [KinectMotor.h] " << xnGetStatusString(res) << "xnUSBSendControl failed" << endl;
		else
			moved = true;
	}
	return moved;

}

int MotorKinect::Pose()
{
	XnStatus res;
	XnUChar buf[10];
	XnUInt32 pnBytesReceived[10];

	int angle;

	if ( ! m_isOpen )
	{
		if( ! isOpen())
		{
			Open();
		}
		if ( ! m_isOpen )
			return -128;
	}
	else
	{

		// Send move control request
		res = xnUSBReceiveControl(m_dev, XN_USB_CONTROL_TYPE_VENDOR, 0x32,0x00, 0x00, buf, sizeof(buf), pnBytesReceived, 0);
		if (res != XN_STATUS_OK) {
			clog << "Error: [KinectMotor.h] " << xnGetStatusString(res) << "xnUSBSendControl failed" << endl;
			return -128;
		}

		angle = (int)(buf[8]);
		if (angle > 128) angle = angle - 256;
		if (angle != 128) angle = angle/2;
	}

	return angle;
}

int MotorKinect::isOpen()
{
	return m_isOpen;
}
