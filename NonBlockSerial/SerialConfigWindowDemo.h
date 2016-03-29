#ifndef SerialConfigWindowDemo__H
#define SerialConfigWindowDemo__H


#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

#include <QSerialPort>
#include <QSerialPortInfo>


QT_BEGIN_NAMESPACE



class SerialConfigWindow : public QWidget
{
	Q_OBJECT

public:
    SerialConfigWindow(QSerialPort* port , QWidget * parent = 0, Qt::WindowFlags f = 0);


private: 
    QSerialPort* port ;


public slots:

	void updateValidPortNamesList() ;

	void on_rescanPortsButton_clicked();
	void on_openPortButton_clicked();

	void on_baudRateComboBox_currentTextChanged(const QString & text);
	void on_flowControlComboBox_currentTextChanged(const QString & text);
	void on_dataBitsComboBox_currentTextChanged(const QString & text);
	void on_parityComboBox_currentTextChanged(const QString & text);
	void on_portNameComboBox_currentTextChanged(const QString & text);
	void on_stopBitsComboBox_currentTextChanged(const QString & text);






	// UI 
private:
    QGridLayout *serialConfigWindowLayout;
    QLabel *portNameLabel;
    QComboBox *portNameComboBox;
    QLabel *baudRateLabel;
    QComboBox *baudRateComboBox;
    QLabel *parityLabel;
    QComboBox *parityComboBox;
    QLabel *dataBitsLabel;
    QComboBox *dataBitsComboBox;
    QLabel *stopBitsLabel;
    QComboBox *stopBitsComboBox;
    QLabel *flowControlLabel;
    QComboBox *flowControlComboBox;
    QPushButton *openPortButton;
    QPushButton *rescanPortsButton ;

    void UI_setup();
    void retranslateUi();

};








#endif

