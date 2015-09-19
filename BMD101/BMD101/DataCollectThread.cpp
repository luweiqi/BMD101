#include "DataCollectThread.h"

/**
 * @brief DataCollectThread::DataCollectThread
 */
DataCollectThread::DataCollectThread()
{
	/*���ڳ�ʼ��*/
	SerialPortState = false;
	/*��������*/
	PortName = "COM8";
	portSettings = { BAUD57600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 2 };
	SerialPortInit();
	ThreadState = true;
	RawWaveValue = 0;

}

/**
 * @brief DataCollectThread::~DataCollectThread
 */
DataCollectThread::~DataCollectThread()
{
	SerialPortClose();
	delete SerialPort;
}


/**
 * @brief DataCollectThread::run
 */
void DataCollectThread::run()
{
	unsigned char _SYNC = 0;
	unsigned char _PLength = 0;
	unsigned char _PayLoad[256] = {0};
	unsigned char _Check = 0;
	int _CheckSum = 0;
	int i = 0;

	while (ThreadState)
	{
		/*����SYNC*/
		SerialPort->getChar((char *)&_SYNC);
		if (_SYNC != SYNC) continue;
		_SYNC = 0x00;

		SerialPort->getChar((char *)&_SYNC);
		if (_SYNC != SYNC) continue;
		_SYNC = 0x00;
		/*��ȡPLength*/
		SerialPort->getChar((char *)&_PLength);
	
		if (_PLength > 169) continue;

		/*��ȡpayload*/
		memset(_PayLoad, 0, sizeof(_PayLoad));
		for (i = 0; i < _PLength; i++)
		{
			SerialPort->getChar((char *)&_PayLoad[i]);
		}

		/*����У����*/
		_CheckSum = 0;
		for (i = 0; i < _PLength; i++)
		{
			_CheckSum += _PayLoad[i];
		}
		_CheckSum &= 0xFF;
		_CheckSum = ~_CheckSum & 0xFF;
		SerialPort->getChar((char *)&_Check);

		/*�ж�У����*/
		if (_Check != _CheckSum)
		{
			qDebug("CheckSum failed");
			continue;
		}

		/*����PayLoad����*/
		ParsePayload(_PayLoad, _PLength);
	}
//	exit(0);
}

/**
 * @brief DataCollectThread::ParsePayload
 * @param payload
 * @param pLength
 * @return
 */
void DataCollectThread::ParsePayload(unsigned char *payload, unsigned char pLength)
{
	unsigned char bytesParsed = 0;
	unsigned char code;
	unsigned char length;
	unsigned char extendedCodeLevel;
	
	int i;
	/*����Payload*/
	while (bytesParsed < pLength)
	{
		/* ��¼EXCODE�ĸ���������Extended Code Level */
		extendedCodeLevel = 0;
		while (payload[bytesParsed] == EXCODE)
		{
			extendedCodeLevel++;
			bytesParsed++;
		}
		code = payload[bytesParsed++];/*��¼CODEֵ*/

		if (code & 0x80)/*code>0x7F,Ϊ���ֽ�����*/
		{
			length = payload[bytesParsed++];
		}
		else/*code<0x7F,Ϊ���ֽ�����*/
		{
			length = 1;
		}

		switch (code)
		{
			case PARSER_HEARTRATE_CODE:	emit SerialPortHeartRateValueSignal(payload[bytesParsed]); 
				break;
			case PARSER_POOR_SIGNAL_CODE: emit SerialPortPoorSignalQualitySignal(payload[bytesParsed]); 
				break;
			case PARSER_RAW_CODE: RawWaveValue = (payload[bytesParsed] << 8) | payload[bytesParsed + 1];
//				emit SerialPortRawWaveValueSignal(RawWaveValue);
				break;
			case PARSER_CONFIG_CODE:
				break;
			case PARSER_DEBUG1_CODE:
				break;
			case PARSER_DEBUG2_CODE:
				break;
			default:break;
		}
		/* Increment the bytesParsed by the length of the Data Value */
		bytesParsed += length;
	}
}

int DataCollectThread::getRawWaveValue(unsigned char highOrderByte, unsigned char lowOrderByte)
{
	/* Sign-extend the signed high byte to the width of a signed int */
	int hi = (int)highOrderByte;
	/* Extend low to the width of an int, but keep exact bits instead of sign-extending */
	int lo = ((int)lowOrderByte) & 0xFF;
	/* Calculate raw value by appending the exact low bits to the sign-extended high bits */
	int value = (hi << 8) | lo;
	return(value);
}


/**
* @brief BMD101::SerialPortInit
* ���ڳ�ʼ��
*/
void DataCollectThread::SerialPortInit()
{
	QString _SerialPortName = "\\\\.\\";
	_SerialPortName.append(PortName);
	SerialPort = new Win_QextSerialPort(_SerialPortName, QextSerialBase::Polling);
	//	connect(SerialPort, SIGNAL(readyRead()), this, SLOT(SerialPortRead()));
}

/**
* @brief BMD101::SerialPortOpen
* �򿪴���
*/
void DataCollectThread::SerialPortOpen()
{
	if (SerialPortState == false)
	{

		if (SerialPort->open(QIODevice::ReadWrite))
		{
			SerialPortState = true;
			emit SerialPortFlagSignal(SERIALPORT_OPEN);
			SerialPort->setBaudRate(portSettings.BaudRate);
			SerialPort->setParity(portSettings.Parity);
			SerialPort->setStopBits(portSettings.StopBits);
			SerialPort->setDataBits(portSettings.DataBits);
			//		SerialPort->setTimeout(5);
		}
		else
		{
			emit SerialPortFlagSignal(SERIALPORT_OPEN_FAILED);
		}
	}
}

/**
* @brief BMD101::SerialPortClose
* �رմ���
*/
void DataCollectThread::SerialPortClose()
{
	if (SerialPortState == true)
	{
		SerialPort->close();
		SerialPortState = false;
		emit SerialPortFlagSignal(SERIALPORT_CLOSE);
	}
}

void DataCollectThread::ChangeThreadState(bool state)
{
	ThreadState = state;
}




