#ifndef BMD101_H
#define BMD101_H

#include <QtWidgets/QMainWindow>
#include "ui_bmd101.h"
#include <qapplication.h>
#include <tchar.h>
#include "DataCollectThread.h"

/*QT������ʾ�Ż�*/
#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")  
#endif  3

/**
 * @brief The BMD101 class
 */
class BMD101 : public QMainWindow
{
	Q_OBJECT

public:
	BMD101(QWidget *parent = 0);
	~BMD101();

private:
	Ui::BMD101Class ui;
	/*�߳�*/
	DataCollectThread *dataCollectThread;
	/*����ͼ*/
	QCPGraph *Heart_Graph_Channel;


	int xAxis_num;

	void SettingWidgetInit();
	void HeartChartInit();
	void HeartChartRawWaveValueAdd(int value);

private slots:

	/*������*/
	void mousePress();
	void mouseWheel();
	void selectionChanged();

	/*�����źŲ�*/
	void BaseDataButton_Clicked();
	void ChartButton_Clicked();
	void SettingButton_Clicked();

	/*ComboBox�źŲ�*/
	void SelectPort(int num);
	void SelectBaud(int num);
	void SelectData(int num);
	void SelectStop(int num);
	void SelectCheck(int num);

	/*����״̬�ź�*/
	void SerialPortFlagSlot(int flag);
	void SerialPortRawWaveValueSlot(int value);
	void SerialPortHeartRateValueSlot(int value);
	void SerialPortPoorSignalQualitySlot(int value);

};

#endif // BMD101_H
