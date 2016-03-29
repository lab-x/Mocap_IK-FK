#include <QDebug>
#include <QMessageBox>

#include <QSerialPort>
#include <QSerialPortInfo>

#include "SerialConfigWindowDemo.h"

static const int StopBitsSize = 3 ;
static const enum QSerialPort::StopBits StopBits [StopBitsSize] = {
    QSerialPort::OneStop , QSerialPort::OneAndHalfStop ,
    QSerialPort::TwoStop } ;


//static const int BaudRateSize = 8 ;
//static const enum QSerialPort::BaudRate BaudRate[BaudRateSize] = {
//	QSerialPort::Baud1200, QSerialPort::Baud2400, QSerialPort::Baud4800,
//	QSerialPort::Baud9600, QSerialPort::Baud19200, QSerialPort::Baud38400 ,
//	QSerialPort::Baud57600 , QSerialPort::Baud115200 } ;
static const qint32 BaudRate[] = {
    QSerialPort::Baud1200, QSerialPort::Baud2400, QSerialPort::Baud4800,
    QSerialPort::Baud9600, QSerialPort::Baud19200, QSerialPort::Baud38400 ,
    QSerialPort::Baud57600 , QSerialPort::Baud115200,  128000,
    256000, 460800, 921600, 1382400
} ;
static const int BaudRateSize = sizeof(BaudRate) / sizeof(BaudRate[0]) ;


static const int DataBitsSize = 4 ;
static const enum QSerialPort::DataBits DataBits[DataBitsSize] = {
    QSerialPort::Data5, QSerialPort::Data6, QSerialPort::Data7,
    QSerialPort::Data8 } ;

static const int FlowControlSize = 3 ;
static const enum QSerialPort::FlowControl FlowControl[FlowControlSize] = {
    QSerialPort::NoFlowControl, QSerialPort::HardwareControl,
    QSerialPort::SoftwareControl } ;

static const int ParitySize = 5 ;
static const enum QSerialPort::Parity Parity[ParitySize] = {
    QSerialPort::NoParity, QSerialPort::EvenParity, QSerialPort::OddParity,
    QSerialPort::SpaceParity, QSerialPort::MarkParity } ;


template <class T>
static int findIndex(T value,const T* array,const int arraySize)
{
    int i ;
    for (i=0;i<arraySize;i++)
    {
        if (value==array[i])
            return i ;
    }
    return -1 ;
}


SerialConfigWindow::SerialConfigWindow(QSerialPort* Port ,QWidget * parent, Qt::WindowFlags f)
                        : QWidget ( parent , f )
{
    UI_setup();

    port = Port ;

    qDebug()<<	"baudRateComboBox->currentIndex() : "<<baudRateComboBox->currentIndex() ;
    qDebug()<<	"dataBitsComboBox->currentIndex() : "<<dataBitsComboBox->currentIndex() ;
    qDebug()<<	"stopBitsComboBox->currentIndex() : "<<stopBitsComboBox->currentIndex() ;
    qDebug()<<	"flowControlComboBox->currentIndex() : "<<flowControlComboBox->currentIndex() ;
    qDebug()<<	"parityComboBox->currentIndex() : "<<parityComboBox->currentIndex() ;


    portNameComboBox->insertItem ( 0 , port->portName() );
    portNameComboBox->setCurrentText ( port->portName() );

    qDebug()<<	"portNameComboBox->currentIndex() : "<<portNameComboBox->currentIndex() ;
    qDebug()<<	"portNameComboBox->currentText() : "<<portNameComboBox->currentText() ;



    int i ;

    i = findIndex ( port->baudRate() , (const qint32*)BaudRate , BaudRateSize ) ;
    if ( i > -1 )
    {
        baudRateComboBox->setCurrentIndex(i) ;
        qDebug()<<	"get baudRate from port : "<<baudRateComboBox->currentText() ;
    }

    i = findIndex ( port->stopBits() , StopBits , StopBitsSize ) ;
    if ( i > -1 )
    {
        stopBitsComboBox->setCurrentIndex(i) ;
        qDebug()<<	"get stopBits from port : "<<stopBitsComboBox->currentText() ;
    }

    i = findIndex ( port->dataBits() , DataBits , DataBitsSize ) ;
    if ( i > -1 )
    {
        dataBitsComboBox->setCurrentIndex(i) ;
        qDebug()<<	"get dataBits from port : "<<dataBitsComboBox->currentText() ;
    }

    i = findIndex ( port->parity() , Parity , ParitySize ) ;
    if ( i > -1 )
    {
        parityComboBox->setCurrentIndex(i) ;
        qDebug()<<	"get parity from port : "<<parityComboBox->currentText() ;
    }

    i = findIndex ( port->flowControl() , FlowControl , FlowControlSize ) ;
    if ( i > -1 )
    {
        flowControlComboBox->setCurrentIndex(i) ;
        qDebug()<<	"get flowControl from port : "<<flowControlComboBox->currentText() ;
    }


    QMetaObject::connectSlotsByName(this);

    updateValidPortNamesList();
    qDebug()<<	"portNameComboBox->currentIndex() : "<<portNameComboBox->currentIndex() ;
    qDebug()<<	"portNameComboBox->currentText() : "<<portNameComboBox->currentText() ;

    if ( port->isOpen() )
    {
        openPortButton->setText(QApplication::translate("SerialConfigWindow", "close Port", 0));
    }
    else
    {
        openPortButton->setText(QApplication::translate("SerialConfigWindow", "open Port", 0));
    }

}




void SerialConfigWindow::on_rescanPortsButton_clicked()
{
    updateValidPortNamesList() ;
}

void SerialConfigWindow::on_openPortButton_clicked()
{
    if ( port->isOpen() )
    {
        port->close() ;
        openPortButton->setText(QApplication::translate("SerialConfigWindow", "open Port", 0));
    }
    else
    {
        // open port here .

        port->setPortName(portNameComboBox->currentText());
        if ( port->open(QIODevice::ReadWrite) )
        {
            //qDebug()<<"BaudRate[baudRateComboBox->currentIndex()] is :"<<BaudRate[baudRateComboBox->currentIndex()];
            //qDebug()<<"DataBits[dataBitsComboBox->currentIndex()] is :"<<DataBits[dataBitsComboBox->currentIndex()];
            //qDebug()<<"StopBits[stopBitsComboBox->currentIndex()] is :"<<StopBits[stopBitsComboBox->currentIndex()];
            qDebug()<<"Parity[parityComboBox->currentIndex()] is :"<<Parity[parityComboBox->currentIndex()];
            qDebug()<<"FlowControl[flowControlComboBox->currentIndex()] is :"<<FlowControl[flowControlComboBox->currentIndex()];
            qDebug()<<"parityComboBox->currentIndex() is :"<<parityComboBox->currentIndex();
            qDebug()<<"flowControlComboBox->currentIndex() is :"<<flowControlComboBox->currentIndex();

            port->setBaudRate(BaudRate[baudRateComboBox->currentIndex()]) ;
            port->setDataBits(DataBits[dataBitsComboBox->currentIndex()]) ;
            port->setStopBits(StopBits[stopBitsComboBox->currentIndex()]) ;
            port->setParity(Parity[parityComboBox->currentIndex()]);
            port->setFlowControl(FlowControl[flowControlComboBox->currentIndex()]);

            openPortButton->setText(QApplication::translate("SerialConfigWindow", "close Port", 0));
        }
        else
        {
            QMessageBox::warning(this,"error detected", "open port failed \n", QMessageBox::Ok, QMessageBox::Ok) ;
        }
    }
}


void SerialConfigWindow::on_baudRateComboBox_currentTextChanged(const QString & text)
{
    if ( port->isOpen() )
    {
        port->setBaudRate(BaudRate[baudRateComboBox->currentIndex()]) ;
    }
}

void SerialConfigWindow::on_flowControlComboBox_currentTextChanged(const QString & text)
{
    if ( port->isOpen() )
    {
        port->setFlowControl(FlowControl[flowControlComboBox->currentIndex()]) ;
    }
}

void SerialConfigWindow::on_dataBitsComboBox_currentTextChanged(const QString & text)
{
    if ( port->isOpen() )
    {
        port->setDataBits(DataBits[dataBitsComboBox->currentIndex()]) ;
    }
}

void SerialConfigWindow::on_parityComboBox_currentTextChanged(const QString & text)
{
    if ( port->isOpen() )
    {
        port->setParity(Parity[parityComboBox->currentIndex()]) ;
    }
}


void SerialConfigWindow::on_stopBitsComboBox_currentTextChanged(const QString & text)
{
    if ( port->isOpen() )
    {
        port->setStopBits(StopBits[stopBitsComboBox->currentIndex()]) ;
    }
}


void SerialConfigWindow::on_portNameComboBox_currentTextChanged(const QString & text)
{
    if ( port->isOpen() )
    {
        QMessageBox::warning(this,"incorrect operation", "you should close the active port first before switch port \n", QMessageBox::Ok, QMessageBox::Ok) ;

        portNameComboBox->disconnect(portNameComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_portNameComboBox_currentTextChanged(QString)));
        portNameComboBox->setCurrentText ( port->portName() );
        portNameComboBox->connect(portNameComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_portNameComboBox_currentTextChanged(QString)));
    }
    else
    {
        port->setPortName(portNameComboBox->currentText());
    }
}


// this function would keep the CurrentText not changed .
void SerialConfigWindow::updateValidPortNamesList()
{

    disconnect(portNameComboBox,SIGNAL(currentTextChanged(const QString & )),
                this,SLOT(on_portNameComboBox_currentTextChanged(const QString & )));

    QStringList list ;
    QString str = portNameComboBox->currentText() ;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();

        if ( str.compare(info.portName(),Qt::CaseInsensitive) != 0 )
            list<< info.portName() ;
    }

    if ( ! (str.isEmpty()) )
        list<< str ;

    list.sort(Qt::CaseInsensitive);	// don't differentiate upper and lower . i.e. regard 'a' and 'A' as the same character .

    portNameComboBox->clear();
    portNameComboBox->insertItems(0,list);

    if ( str.isEmpty() )
    {
        if ( ! (list.empty()) )
        {
            portNameComboBox->setCurrentText(list.at(0));
        }
    }
    else
    {
        portNameComboBox->setCurrentText(str);
    }

    connect(portNameComboBox,SIGNAL(currentTextChanged(const QString & )),
                this,SLOT(on_portNameComboBox_currentTextChanged(const QString & )));
}


void SerialConfigWindow::UI_setup()
{
    SerialConfigWindow*serialConfigWindow = this ;

    if (serialConfigWindow->objectName().isEmpty())
        serialConfigWindow->setObjectName(QStringLiteral("serialConfigWindow"));
//    serialConfigWindow->resize(229, 274);
    serialConfigWindow->resize(298, 311);
    serialConfigWindow->setMinimumSize(QSize(298, 311));
    serialConfigWindow->setMaximumSize(QSize(298, 311));

    serialConfigWindowLayout = new QGridLayout(serialConfigWindow);
    serialConfigWindowLayout->setObjectName(QStringLiteral("serialConfigWindowLayout"));
    serialConfigWindowLayout->setContentsMargins(0, 0, 0, 0);
    portNameLabel = new QLabel(serialConfigWindow);
    portNameLabel->setObjectName(QStringLiteral("portNameLabel"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(portNameLabel->sizePolicy().hasHeightForWidth());
    portNameLabel->setSizePolicy(sizePolicy);
    portNameLabel->setMinimumSize(QSize(100, 25));
    portNameLabel->setMaximumSize(QSize(100, 25));

    serialConfigWindowLayout->addWidget(portNameLabel, 0, 0, 1, 1);

    portNameComboBox = new QComboBox(serialConfigWindow);
    portNameComboBox->setObjectName(QStringLiteral("portNameComboBox"));
    sizePolicy.setHeightForWidth(portNameComboBox->sizePolicy().hasHeightForWidth());
    portNameComboBox->setSizePolicy(sizePolicy);
    portNameComboBox->setMinimumSize(QSize(180, 25));
    portNameComboBox->setMaximumSize(QSize(180, 25));

    serialConfigWindowLayout->addWidget(portNameComboBox, 0, 1, 1, 1);

    baudRateLabel = new QLabel(serialConfigWindow);
    baudRateLabel->setObjectName(QStringLiteral("baudRateLabel"));
    sizePolicy.setHeightForWidth(baudRateLabel->sizePolicy().hasHeightForWidth());
    baudRateLabel->setSizePolicy(sizePolicy);
    baudRateLabel->setMinimumSize(QSize(100, 25));
    baudRateLabel->setMaximumSize(QSize(100, 25));

    serialConfigWindowLayout->addWidget(baudRateLabel, 1, 0, 1, 1);

    baudRateComboBox = new QComboBox(serialConfigWindow);
    baudRateComboBox->setObjectName(QStringLiteral("baudRateComboBox"));
    sizePolicy.setHeightForWidth(baudRateComboBox->sizePolicy().hasHeightForWidth());
    baudRateComboBox->setSizePolicy(sizePolicy);
    baudRateComboBox->setMinimumSize(QSize(180, 25));
    baudRateComboBox->setMaximumSize(QSize(180, 25));

    serialConfigWindowLayout->addWidget(baudRateComboBox, 1, 1, 1, 1);

    parityLabel = new QLabel(serialConfigWindow);
    parityLabel->setObjectName(QStringLiteral("parityLabel"));
    sizePolicy.setHeightForWidth(parityLabel->sizePolicy().hasHeightForWidth());
    parityLabel->setSizePolicy(sizePolicy);
    parityLabel->setMinimumSize(QSize(100, 25));
    parityLabel->setMaximumSize(QSize(100, 25));

    serialConfigWindowLayout->addWidget(parityLabel, 2, 0, 1, 1);

    parityComboBox = new QComboBox(serialConfigWindow);
    parityComboBox->setObjectName(QStringLiteral("parityComboBox"));
    sizePolicy.setHeightForWidth(parityComboBox->sizePolicy().hasHeightForWidth());
    parityComboBox->setSizePolicy(sizePolicy);
    parityComboBox->setMinimumSize(QSize(180, 25));
    parityComboBox->setMaximumSize(QSize(180, 25));

    serialConfigWindowLayout->addWidget(parityComboBox, 2, 1, 1, 1);

    dataBitsLabel = new QLabel(serialConfigWindow);
    dataBitsLabel->setObjectName(QStringLiteral("dataBitsLabel"));
    sizePolicy.setHeightForWidth(dataBitsLabel->sizePolicy().hasHeightForWidth());
    dataBitsLabel->setSizePolicy(sizePolicy);
    dataBitsLabel->setMinimumSize(QSize(100, 25));
    dataBitsLabel->setMaximumSize(QSize(100, 25));

    serialConfigWindowLayout->addWidget(dataBitsLabel, 3, 0, 1, 1);

    dataBitsComboBox = new QComboBox(serialConfigWindow);
    dataBitsComboBox->setObjectName(QStringLiteral("dataBitsComboBox"));
    sizePolicy.setHeightForWidth(dataBitsComboBox->sizePolicy().hasHeightForWidth());
    dataBitsComboBox->setSizePolicy(sizePolicy);
    dataBitsComboBox->setMinimumSize(QSize(180, 25));
    dataBitsComboBox->setMaximumSize(QSize(180, 25));

    serialConfigWindowLayout->addWidget(dataBitsComboBox, 3, 1, 1, 1);

    stopBitsLabel = new QLabel(serialConfigWindow);
    stopBitsLabel->setObjectName(QStringLiteral("stopBitsLabel"));
    sizePolicy.setHeightForWidth(stopBitsLabel->sizePolicy().hasHeightForWidth());
    stopBitsLabel->setSizePolicy(sizePolicy);
    stopBitsLabel->setMinimumSize(QSize(100, 25));
    stopBitsLabel->setMaximumSize(QSize(100, 25));

    serialConfigWindowLayout->addWidget(stopBitsLabel, 4, 0, 1, 1);

    stopBitsComboBox = new QComboBox(serialConfigWindow);
    stopBitsComboBox->setObjectName(QStringLiteral("stopBitsComboBox"));
    sizePolicy.setHeightForWidth(stopBitsComboBox->sizePolicy().hasHeightForWidth());
    stopBitsComboBox->setSizePolicy(sizePolicy);
    stopBitsComboBox->setMinimumSize(QSize(180, 25));
    stopBitsComboBox->setMaximumSize(QSize(180, 25));

    serialConfigWindowLayout->addWidget(stopBitsComboBox, 4, 1, 1, 1);

    flowControlLabel = new QLabel(serialConfigWindow);
    flowControlLabel->setObjectName(QStringLiteral("flowControlLabel"));
    sizePolicy.setHeightForWidth(flowControlLabel->sizePolicy().hasHeightForWidth());
    flowControlLabel->setSizePolicy(sizePolicy);
    flowControlLabel->setMinimumSize(QSize(100, 25));
    flowControlLabel->setMaximumSize(QSize(100, 25));

    serialConfigWindowLayout->addWidget(flowControlLabel, 5, 0, 1, 1);

    flowControlComboBox = new QComboBox(serialConfigWindow);
    flowControlComboBox->setObjectName(QStringLiteral("flowControlComboBox"));
    sizePolicy.setHeightForWidth(flowControlComboBox->sizePolicy().hasHeightForWidth());
    flowControlComboBox->setSizePolicy(sizePolicy);
    flowControlComboBox->setMinimumSize(QSize(180, 25));
    flowControlComboBox->setMaximumSize(QSize(180, 25));

    serialConfigWindowLayout->addWidget(flowControlComboBox, 5, 1, 1, 1);

    openPortButton = new QPushButton(serialConfigWindow);
    openPortButton->setObjectName(QStringLiteral("openPortButton"));
    sizePolicy.setHeightForWidth(openPortButton->sizePolicy().hasHeightForWidth());
    openPortButton->setSizePolicy(sizePolicy);
    openPortButton->setMinimumSize(QSize(286, 30));
    openPortButton->setMaximumSize(QSize(286, 30));

    serialConfigWindowLayout->addWidget(openPortButton, 6, 0, 1, 2);

    rescanPortsButton = new QPushButton(serialConfigWindow);
    rescanPortsButton->setObjectName(QStringLiteral("rescanPortsButton"));
    sizePolicy.setHeightForWidth(rescanPortsButton->sizePolicy().hasHeightForWidth());
    rescanPortsButton->setSizePolicy(sizePolicy);
    rescanPortsButton->setMinimumSize(QSize(286, 30));
    rescanPortsButton->setMaximumSize(QSize(286, 30));

    serialConfigWindowLayout->addWidget(rescanPortsButton, 7, 0, 1, 2);



    retranslateUi();
} // setupUi

void SerialConfigWindow::retranslateUi()
{
    setWindowTitle(QApplication::translate("SerialConfigWindow", "Serial Configure", 0));
    portNameLabel->setText(QApplication::translate("SerialConfigWindow", "PortName", 0));
    baudRateLabel->setText(QApplication::translate("SerialConfigWindow", "BaudRate", 0));
    baudRateComboBox->clear();
    baudRateComboBox->insertItems(0, QStringList()
     << QApplication::translate("SerialConfigWindow", "1200", 0)
     << QApplication::translate("SerialConfigWindow", "2400", 0)
     << QApplication::translate("SerialConfigWindow", "4800", 0)
     << QApplication::translate("SerialConfigWindow", "9600", 0)
     << QApplication::translate("SerialConfigWindow", "19200", 0)
     << QApplication::translate("SerialConfigWindow", "38400", 0)
     << QApplication::translate("SerialConfigWindow", "57600", 0)
     << QApplication::translate("SerialConfigWindow", "115200", 0)
    << QApplication::translate("SerialConfigWindow", "128000", 0)
      << QApplication::translate("SerialConfigWindow", "256000", 0)
      << QApplication::translate("SerialConfigWindow", "460800", 0)
      << QApplication::translate("SerialConfigWindow", "921600", 0)
      << QApplication::translate("SerialConfigWindow", "1382400", 0)
    );
    baudRateComboBox->setCurrentText(QApplication::translate("SerialConfigWindow", "256000", 0));
    parityLabel->setText(QApplication::translate("SerialConfigWindow", "Parity", 0));
    parityComboBox->clear();
    parityComboBox->insertItems(0, QStringList()
     << QApplication::translate("SerialConfigWindow", "NoParity", 0)
     << QApplication::translate("SerialConfigWindow", "EvenParity", 0)
     << QApplication::translate("SerialConfigWindow", "OddParity", 0)
     << QApplication::translate("SerialConfigWindow", "SpaceParity", 0)
     << QApplication::translate("SerialConfigWindow", "MarkParity", 0)
    );
    dataBitsLabel->setText(QApplication::translate("SerialConfigWindow", "DataBits", 0));
    dataBitsComboBox->clear();
    dataBitsComboBox->insertItems(0, QStringList()
     << QApplication::translate("SerialConfigWindow", "5", 0)
     << QApplication::translate("SerialConfigWindow", "6", 0)
     << QApplication::translate("SerialConfigWindow", "7", 0)
     << QApplication::translate("SerialConfigWindow", "8", 0)
    );
    dataBitsComboBox->setCurrentText(QApplication::translate("SerialConfigWindow", "8", 0));
    stopBitsLabel->setText(QApplication::translate("SerialConfigWindow", "StopBits", 0));
    stopBitsComboBox->clear();
    stopBitsComboBox->insertItems(0, QStringList()
     << QApplication::translate("SerialConfigWindow", "1", 0)
     << QApplication::translate("SerialConfigWindow", "1.5", 0)
     << QApplication::translate("SerialConfigWindow", "2", 0)
    );
    flowControlLabel->setText(QApplication::translate("SerialConfigWindow", "FlowControl", 0));
    flowControlComboBox->clear();
    flowControlComboBox->insertItems(0, QStringList()
     << QApplication::translate("SerialConfigWindow", "NoFlowControl", 0)
     << QApplication::translate("SerialConfigWindow", "HardwareControl-(RTS/CTS)", 0)
     << QApplication::translate("SerialConfigWindow", "SoftwareControl-(XON/XOFF)", 0)
    );
    openPortButton->setText(QApplication::translate("SerialConfigWindow", "open Port", 0));
    rescanPortsButton->setText(QApplication::translate("SerialConfigWindow", "ReScanPorts", 0));
} // retranslateUi

