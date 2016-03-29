
#include <QDialog>
#include <QMessageBox>
#include "IKMonitor.h"

extern "C"
{
    #include "MathToolsC/cQuaternion.h"
    #include "SerialProtocol.h"
    #include "SerialCmdAndReplies.h"
    #include "POTDataParser20.h"
}

uint16_t IKMonitor::UdpPortsNo[UDP_POT_NODES] = {
   9901, 9902, 9903, 9904, 9905, 9906
} ;


IKMonitor::IKMonitor(QWidget *parent)
{
    int i ;
    for (i=0; i<UDP_POT_NODES; i++)
    {
        UdpPOTSystem[i] = new POT() ;
        UdpPOTSystem[i]->setLHStationNum(2) ;

        UdpPOTPort[i].socket = NULL ;
//        UdpPOTPort[i].socket = new QUdpSocket(this) ;
//        UdpPOTPort[i].socket->bind(QHostAddress::Any,UdpPortsNo[i]);
//        connect(UdpPOTPort[i].socket, SIGNAL(readyRead()),this, SLOT(processPOTUdpPortData()));

        UdpPOTPort[i].DataViewer = new SerialDataViewer(NULL,false);
        UdpPOTPort[i].Data = new QByteArray() ;


        UdpPOTDataInfo[i].FrameCount = 0 ;
        UdpPOTDataInfo[i].preFrameCount = 0 ;
        UdpPOTDataInfo[i].FPS = 0 ;
        UdpPOTDataInfo[i].KeyStatus = 0 ;
        UdpPOTDataInfo[i].NodeID = 0 ;
        UdpPOTDataInfo[i].dataValid = 0 ;
        UdpPOTDataInfo[i].StationID = 0 ;
//        UdpPOTDataInfo[i].SensorMask = 0 ;
        UdpPOTDataInfo[i].ts = 0 ;


    }

    for (i=0; i<POT_COUNT; i++)
    {
        POTSystem[i] = new POT();
        POTSystem[i]->setLHStationNum(2);

        POTPort[i].Port = new QSerialPort (this) ;
        POTPort[i].Port->setBaudRate(460800) ;
        POTPort[i].ConfigWindow = new SerialConfigWindow (POTPort[i].Port) ;
        POTPort[i].DataViewer = new SerialDataViewer (POTPort[i].Port,false);
        POTPort[i].Data = new QByteArray() ;
        connect(POTPort[i].Port,SIGNAL(readyRead()),this,SLOT(processPOTComPortData())) ;

        POTDataInfo[i].FrameCount = 0 ;
        POTDataInfo[i].preFrameCount = 0 ;
        POTDataInfo[i].FPS = 0 ;
        POTDataInfo[i].KeyStatus = 0 ;
        POTDataInfo[i].NodeID = 0 ;
        POTDataInfo[i].dataValid = 0 ;
        POTDataInfo[i].StationID = 0 ;
//        POTDataInfo[i].SensorMask = 0 ;
        POTDataInfo[i].ts = 0 ;
    }



    ui()->setupUi(this);


    MocapComPort = new QSerialPort (this) ;
    MocapPortConfigWindow = new SerialConfigWindow (MocapComPort) ;
    MocapPortDataViewer = new SerialDataViewer (MocapComPort,false);
    connect(MocapComPort,SIGNAL(readyRead()),this,SLOT(processMocapComPortData())) ;
    MocapComData = new QByteArray() ;
    MocapDataInfo.FrameCount = 0 ;
    MocapDataInfo.preFrameCount = 0 ;
    MocapDataInfo.FPS = 0 ;
    mocapIMU = new MocapIMU() ;


    POTTime.start();
    for (i=0; i<POT_COUNT; i++)
    {
        POTSystem[i] = new POT();
        POTSystem[i]->setLHStationNum(2);

        POTPort[i].Port = new QSerialPort (this) ;
        POTPort[i].Port->setBaudRate(460800) ;
        POTPort[i].ConfigWindow = new SerialConfigWindow (POTPort[i].Port) ;
        POTPort[i].DataViewer = new SerialDataViewer (POTPort[i].Port,false);
        POTPort[i].Data = new QByteArray() ;
        connect(POTPort[i].Port,SIGNAL(readyRead()),this,SLOT(processPOTComPortData())) ;

        POTDataInfo[i].FrameCount = 0 ;
        POTDataInfo[i].preFrameCount = 0 ;
        POTDataInfo[i].FPS = 0 ;
        POTDataInfo[i].KeyStatus = 0 ;
        POTDataInfo[i].NodeID = 0 ;
        POTDataInfo[i].dataValid = 0 ;
        POTDataInfo[i].StationID = 0 ;
//        POTDataInfo[i].SensorMask = 0 ;
        POTDataInfo[i].ts = 0 ;
    }



    FPSupdateTimer = new QTimer(this) ;
    FPSupdateTimer->setInterval(1000);
    connect(FPSupdateTimer,SIGNAL(timeout()),this,SLOT(updateFPS()));
    FPSupdateTimer->start();


    _3DViewer = new Virtual3dCharacterViewer( ui()->_3DPlayerGroupBox ) ; 
    connect(_3DViewer,SIGNAL(jointMoved()),this,SLOT(updateStatusPanel()));

//    WorkMode = TEST_IK ;
    WorkMode = TEST_POT ;
    _3DViewer->showFK(false) ;
    _3DViewer->showIK(true) ;

    StatusUpdateTimer = new QTimer(this);
    StatusUpdateTimer->setInterval(10);
    StatusContentChanged = true ;
    connect(StatusUpdateTimer,SIGNAL(timeout()),this,SLOT(prepareUpdateStatusPanel()));
    StatusUpdateTimer->start();

    FK2IKUpdateTimer = new QTimer(this);
    FK2IKUpdateTimer->setInterval(50);
    FK2IKReady = false ;
    connect(FK2IKUpdateTimer,SIGNAL(timeout()),this,SLOT(prepareUpdateFK2IK()));
    FK2IKUpdateTimer->start();



    this->FK_isCali = false;
    this->TcpSend = false;
    this->tcpServer = new QTcpServer(this);
    this->tcpSocket = new QTcpSocket(this);    
    newListen();
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(displayError(QAbstractSocket::SocketError)));
    TcpContentChanged = false ;
    TcpSendTimer = new QTimer(this);
    TcpSendTimer->setInterval(10);
    connect(TcpSendTimer,SIGNAL(timeout()),this,SLOT(onTcpSend())) ;
    TcpSendTimer->start();


    deployOscilloscopes();


    ActivedPOTNodeID = 4 ;
    ui()->activedPOTNodeLineEdit->setText(QString("%1").arg(ActivedPOTNodeID));

    float DampingTime = POTSystem[0]->d->lhpot->d->Fusion->p->OffsetDampingTime ;
    ui()->OffsetDampingTimeLineEdit->setText(QString("%1").arg(DampingTime));


    DampingTime = POTSystem[0]->d->lhpot->d->Fusion->p->HistoryConfidenceDampingTime ;
    ui()->ConfidenceDampingTimeLineEdit->setText(QString("%1").arg(DampingTime));

    updateStatusPanel();

    StartRecord = false;
    QuatLocal_File.setFileName("QL.txt");
    EulerAngle_File.setFileName("EA.txt");
    RawData_File.setFileName("raw.txt");


    for ( i=0; i<POT_COUNT; i++ )
    {
        setupSerialDataParser ( SerialParser+i, (const uint8_t*)"reply  ", &IKMonitor::SerialParserHandlerAdaptor ) ;
        SerialParser[i].aux = this ;
    }

    PollTimer = new QTimer(this);
    PollTimer->setInterval(10);
    connect(PollTimer,SIGNAL(timeout()),this,SLOT(onPollTimerout())) ;
    PollTimer->start();

    _3DViewer->MarkCubeFK[0].show = true;
    _3DViewer->MarkCubeFK[0].Pos = Vector3D(0, 0, 0);
    _3DViewer->MarkCubeFK[0].Color =  Vector3D(1, 1, 1);

    _3DViewer->MarkCubeFK[1].show = true ;
    _3DViewer->MarkCubeFK[1].Pos =  Vector3D(0, 0, 0);
    _3DViewer->MarkCubeFK[1].Color =  Vector3D(1, 0, 0);

    _3DViewer->MarkCubeFK[2].show = true ;
    _3DViewer->MarkCubeFK[2].Pos =  Vector3D(0, 0, 0);
    _3DViewer->MarkCubeFK[2].Color =  Vector3D(0, 0, 1);

    _3DViewer->MarkCubeFK[3].show = true ;
    _3DViewer->MarkCubeFK[3].Pos =  Vector3D(0, 0, 0);
    _3DViewer->MarkCubeFK[3].Color =  Vector3D(1, 0, 0);

    _3DViewer->MarkCubeFK[4].show = true ;
    _3DViewer->MarkCubeFK[4].Pos =  Vector3D(0, 0, 0);
    _3DViewer->MarkCubeFK[4].Color =  Vector3D(0, 0, 1);

//    _3DViewer->MarkCubeFK[3].show = true ;
//    _3DViewer->MarkCubeFK[3].Pos =  Vector3D(0, 0, 0);
//    _3DViewer->MarkCubeFK[3].Color =  Vector3D(0, 1, 1);

    CurHubMode = HubMode_Normal ;
    IKSrc = 0;
}

IKMonitor::~IKMonitor()
{
    undeployOscilloscopes();
}

void IKMonitor::on_EnableTcpSendCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        TcpSend = true ;
    }
    else if ( state == Qt::Unchecked )
    {
        TcpSend = false ;
    }

}

void IKMonitor::newListen(){
    //监听是否有客户端来访，且对任何来访者监听，端口为6666
    if(!tcpServer->listen(QHostAddress::Any,7001))
    {
        qDebug()<<tcpServer->errorString();

        close();

        return;
    }
}
void IKMonitor::acceptConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
}

void IKMonitor::displayError(QAbstractSocket::SocketError)

{

   qDebug()<<tcpSocket->errorString();

   tcpSocket->close();

}

void IKMonitor::on_showMocapComDataCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        MocapPortDataViewer->setWindowTitle(QString("Mocap Port Data"));
        MocapPortDataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        MocapPortDataViewer->close();
    }
}

void IKMonitor::on_showOscilloscopeCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        ScopesClub->show();
    }
    else if ( state == Qt::Unchecked )
    {
        ScopesClub->close();
    }
}


void IKMonitor::on_ShowIKCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        _3DViewer->showIK(true) ;
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->showIK(false) ;
    }
}

void IKMonitor::on_ShowFKCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        _3DViewer->showFK(true) ;
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->showFK(false) ;
    }

}

void IKMonitor::on_ShowAllPOTNodesCheckBox_stateChanged(int state)
{
    bool checked ;
    if ( state == Qt::Checked )
    {
        checked = true ;
    }
    else if ( state == Qt::Unchecked )
    {
        checked = false ;
    }

    ui()->ShowPOTNode1CheckBox->setChecked(checked);
    ui()->ShowPOTNode2CheckBox->setChecked(checked);
    ui()->ShowPOTNode3CheckBox->setChecked(checked);
    ui()->ShowPOTNode4CheckBox->setChecked(checked);
    ui()->ShowPOTNode5CheckBox->setChecked(checked);
    ui()->ShowPOTNode6CheckBox->setChecked(checked);
    ui()->ShowPOTNode7CheckBox->setChecked(checked);

}


void IKMonitor::on_ShowPOTNode1CheckBox_stateChanged(int state)
{
    int idx = 1 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx);
    }
}

void IKMonitor::on_ShowPOTNode2CheckBox_stateChanged(int state)
{
    int idx = 2 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx);
    }
}
void IKMonitor::on_ShowPOTNode3CheckBox_stateChanged(int state)
{
    int idx = 3 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx);
    }
}
void IKMonitor::on_ShowPOTNode4CheckBox_stateChanged(int state)
{
    int idx = 4 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx);
    }
}
void IKMonitor::on_ShowPOTNode5CheckBox_stateChanged(int state)
{
    int idx = 5 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx);
    }
}
void IKMonitor::on_ShowPOTNode6CheckBox_stateChanged(int state)
{
    int idx = 6 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx);    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx);
    }
}


void IKMonitor::on_activedPOTNodeLineEdit_editingFinished()
{
    QString str = ui()->activedPOTNodeLineEdit->text() ;
    bool OK ;
    int node = str.toInt(&OK) ;
    if ( ! OK || node < 1 || node > 7 )
    {
 //       QMessageBox (QMessageBox::Warning,QString("WARNING"),QString("slave ID must be a integer between 0 and 255 ! \n set to default value instead ."),QMessageBox::Ok,this);
        node = ActivedPOTNodeID ;
        ui()->activedPOTNodeLineEdit->setText(QString("%1").arg(node));
    }

    ActivedPOTNodeID = node ;
    ScopesClub->on_ResetAllButton_clicked();
}

void IKMonitor::on_ConfidenceDampingTimeLineEdit_editingFinished()
{
    QString str = ui()->ConfidenceDampingTimeLineEdit->text() ;
    bool OK ;
    float DampingTime = str.toDouble(&OK) ;
    if ( ! OK || DampingTime < 0 )
    {
 //       QMessageBox (QMessageBox::Warning,QString("WARNING"),QString("slave ID must be a integer between 0 and 255 ! \n set to default value instead ."),QMessageBox::Ok,this);
        DampingTime = 0.5 ;
        ui()->ConfidenceDampingTimeLineEdit->setText(QString("%1").arg(DampingTime));
    }

    int i ;
    for (i=0; i<UDP_POT_NODES; i++)
        UdpPOTSystem[i]->pLHFusion()->HistoryConfidenceDampingTime = DampingTime ;
    for (i=0; i<POT_COUNT; i++)
        POTSystem[i]->pLHFusion()->HistoryConfidenceDampingTime = DampingTime ;
    ui()->ConfidenceDampingTimeLineEdit->setText(QString("%1").arg(DampingTime));

}

void IKMonitor::on_OffsetDampingTimeLineEdit_editingFinished()
{
    QString str = ui()->OffsetDampingTimeLineEdit->text() ;
    bool OK ;
    float DampingTime = str.toDouble(&OK) ;
    if ( ! OK || DampingTime < 0 )
    {
 //       QMessageBox (QMessageBox::Warning,QString("WARNING"),QString("slave ID must be a integer between 0 and 255 ! \n set to default value instead ."),QMessageBox::Ok,this);
        DampingTime = 3 ;
        ui()->OffsetDampingTimeLineEdit->setText(QString("%1").arg(DampingTime));
    }

    int i ;
    for (i=0; i<UDP_POT_NODES; i++)
        UdpPOTSystem[i]->pLHFusion()->OffsetDampingTime = DampingTime ;
    for (i=0; i<POT_COUNT; i++)
        POTSystem[i]->pLHFusion()->OffsetDampingTime = DampingTime ;
    ui()->OffsetDampingTimeLineEdit->setText(QString("%1").arg(DampingTime));
}


void IKMonitor::on_POTSmoothRatioLineEdit_editingFinished()
{
    QString str = ui()->POTSmoothRatioLineEdit->text() ;
    bool OK ;
    float ratio = str.toDouble(&OK) ;
    if ( ! OK || ratio < 0 || ratio > 1 )
    {
 //       QMessageBox (QMessageBox::Warning,QString("WARNING"),QString("slave ID must be a integer between 0 and 255 ! \n set to default value instead ."),QMessageBox::Ok,this);
        ratio = 1.0 ;
        ui()->POTSmoothRatioLineEdit->setText(QString("%1").arg(ratio));
    }

    int i ;
    for (i=0; i<UDP_POT_NODES; i++)
        UdpPOTSystem[i]->pLHFusion()->SmoothRatio = ratio ;
    for (i=0; i<POT_COUNT; i++)
        POTSystem[i]->pLHFusion()->SmoothRatio = ratio ;
    ui()->POTSmoothRatioLineEdit->setText(QString("%1").arg(ratio));
}

void IKMonitor::changeShowUdpPortDataCheckBoxState(int idx, int state)
{
    if ( state == Qt::Checked )
    {
        UdpPOTPort[idx].DataViewer->setWindowTitle(QString("Udp PORT Channel %1 ").arg(idx+1));
        UdpPOTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        UdpPOTPort[idx].DataViewer->close();
    }
}

void IKMonitor::on_useCOMRadioButton_toggled(bool checked)
{
    if ( checked )
    {
        for ( int i=0; i<UDP_POT_NODES; i++ )
        {
            if ( UdpPOTPort[i].socket )
            {
                delete UdpPOTPort[i].socket ;
                UdpPOTPort[i].socket = NULL ;
            }
        }
    }
}

void IKMonitor::on_useUDPRadioButton_toggled(bool checked)
{
    if ( checked )
    {
        for ( int i=0; i<UDP_POT_NODES; i++ )
        {
            if ( UdpPOTPort[i].socket )
            {
                delete UdpPOTPort[i].socket ;
                UdpPOTPort[i].socket = NULL ;
            }
            UdpPOTPort[i].socket = new QUdpSocket(this) ;
            UdpPOTPort[i].socket->bind(QHostAddress::Any,UdpPortsNo[i]);
            connect(UdpPOTPort[i].socket, SIGNAL(readyRead()),this, SLOT(processPOTUdpPortData()));
        }
    }
}


void IKMonitor::on_testIKRadioButton_toggled(bool checked)
{
    if ( checked )
    {
        _3DViewer->initVirtualBody();
        WorkMode = TEST_IK ;
    }
}

void IKMonitor::on_testPOTRadioButton_toggled(bool checked)
{
    if ( checked )
    {
        _3DViewer->initVirtualBody();
        WorkMode = TEST_POT ;
    }
}



void IKMonitor::on_showUdpPort1DataCheckBox_stateChanged(int state)
{
    changeShowUdpPortDataCheckBoxState(0, state) ;
}
void IKMonitor::on_showUdpPort2DataCheckBox_stateChanged(int state)
{
    changeShowUdpPortDataCheckBoxState(1, state) ;
}
void IKMonitor::on_showUdpPort3DataCheckBox_stateChanged(int state)
{
    changeShowUdpPortDataCheckBoxState(2, state) ;
}
void IKMonitor::on_showUdpPort4DataCheckBox_stateChanged(int state)
{
    changeShowUdpPortDataCheckBoxState(3, state) ;
}
void IKMonitor::on_showUdpPort5DataCheckBox_stateChanged(int state)
{
    changeShowUdpPortDataCheckBoxState(4, state) ;
}
void IKMonitor::on_showUdpPort6DataCheckBox_stateChanged(int state)
{
    changeShowUdpPortDataCheckBoxState(5, state) ;
}


void IKMonitor::on_MocapPortConifgButton_clicked()
{
    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(MocapComPort,&dlg) ;
    window->move(0,0);
    dlg.resize(window->size());
    dlg.setWindowTitle(QString("Mocap Port Settings"));
    dlg.exec();

    delete window ;

    if ( MocapComPort->isOpen() )
    {
//        _3DViewer->isComport = true;
        ui()->MocapPortStatusLabel->setText(QString("串口已打开"));
    }
    else
    {
//        _3DViewer->isComport = false;
        ui()->MocapPortStatusLabel->setText(QString("串口已关闭"));
    }
}

void IKMonitor::on_showPOT1ComDataCheckBox_stateChanged(int state)
{
    int idx = 0 ;

    if ( state == Qt::Checked )
    {
        POTPort[idx].DataViewer->setWindowTitle(QString("POT%1 Port Data").arg(idx+1));
        POTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        POTPort[idx].DataViewer->close();
    }
}
void IKMonitor::on_showPOT2ComDataCheckBox_stateChanged(int state)
{
    int idx = 1 ;

    if ( state == Qt::Checked )
    {
        POTPort[idx].DataViewer->setWindowTitle(QString("POT%1 Port Data").arg(idx+1));
        POTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        POTPort[idx].DataViewer->close();
    }
}
void IKMonitor::on_showPOT3ComDataCheckBox_stateChanged(int state)
{
    int idx = 2 ;

    if ( state == Qt::Checked )
    {
        POTPort[idx].DataViewer->setWindowTitle(QString("POT%1 Port Data").arg(idx+1));
        POTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        POTPort[idx].DataViewer->close();
    }
}
void IKMonitor::on_showPOT4ComDataCheckBox_stateChanged(int state)
{
    int idx = 3 ;

    if ( state == Qt::Checked )
    {
        POTPort[idx].DataViewer->setWindowTitle(QString("POT%1 Port Data").arg(idx+1));
        POTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        POTPort[idx].DataViewer->close();
    }
}
void IKMonitor::on_showPOT5ComDataCheckBox_stateChanged(int state)
{
    int idx = 4 ;

    if ( state == Qt::Checked )
    {
        POTPort[idx].DataViewer->setWindowTitle(QString("POT%1 Port Data").arg(idx+1));
        POTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        POTPort[idx].DataViewer->close();
    }
}
void IKMonitor::on_showPOT6ComDataCheckBox_stateChanged(int state)
{
    int idx = 5 ;

    if ( state == Qt::Checked )
    {
        POTPort[idx].DataViewer->setWindowTitle(QString("POT%1 Port Data").arg(idx+1));
        POTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        POTPort[idx].DataViewer->close();
    }
}

void IKMonitor::on_showPOT7ComDataCheckBox_stateChanged(int state)
{
    int idx = 6 ;

    if ( state == Qt::Checked )
    {
        POTPort[idx].DataViewer->setWindowTitle(QString("POT%1 Port Data").arg(idx+1));
        POTPort[idx].DataViewer->show();
    }
    else if ( state == Qt::Unchecked )
    {
        POTPort[idx].DataViewer->close();
    }

}
void IKMonitor::on_EnablePOTOriFusionCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i] ->reset() ;
            POTSystem[i] ->LHPOT()->enableOriFusion();
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i] ->reset() ;
            UdpPOTSystem[i] ->LHPOT()->enableOriFusion();
        }
    }
    else if ( state == Qt::Unchecked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i]->LHPOT()->disableOriFusion();
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i]->LHPOT()->disableOriFusion();
        }
    }
}

void IKMonitor::on_EnablePOTPosFusionCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i]->reset() ;
            POTSystem[i]->LHPOT()->enablePosFusion();
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i]->reset() ;
            UdpPOTSystem[i]->LHPOT()->enablePosFusion();
        }
    }
    else if ( state == Qt::Unchecked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i]->LHPOT()->disablePosFusion();
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i]->LHPOT()->disablePosFusion();
        }

    }
}


void IKMonitor::on_POT1PortConifgButton_clicked()
{
    int idx = 0 ;
    QLabel* label = ui()->POT1PortStatusLabel ;

    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(POTPort[idx].Port,&dlg) ;
    window->move(0,0);
    dlg.setWindowTitle(QString("POT%1 Port Settings").arg(idx+1));
    dlg.resize(window->size());
    dlg.exec();

    delete window ;

    if ( POTPort[idx].Port->isOpen() )
    {
        label->setText(QString("串口已打开"));
    }
    else
    {
        label->setText(QString("串口已关闭"));
    }
}
void IKMonitor::on_POT2PortConifgButton_clicked()
{
    int idx = 1 ;
    QLabel* label = ui()->POT2PortStatusLabel ;

    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(POTPort[idx].Port,&dlg) ;
    window->move(0,0);
    dlg.setWindowTitle(QString("POT%1 Port Settings").arg(idx+1));
    dlg.resize(window->size());
    dlg.exec();

    delete window ;

    if ( POTPort[idx].Port->isOpen() )
    {
        label->setText(QString("串口已打开"));
    }
    else
    {
        label->setText(QString("串口已关闭"));
    }
}
void IKMonitor::on_POT3PortConifgButton_clicked()
{
    int idx = 2 ;
    QLabel* label = ui()->POT3PortStatusLabel ;

    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(POTPort[idx].Port,&dlg) ;
    window->move(0,0);
    dlg.setWindowTitle(QString("POT%1 Port Settings").arg(idx+1));
    dlg.resize(window->size());
    dlg.exec();

    delete window ;

    if ( POTPort[idx].Port->isOpen() )
    {
        label->setText(QString("串口已打开"));
    }
    else
    {
        label->setText(QString("串口已关闭"));
    }
}
void IKMonitor::on_POT4PortConifgButton_clicked()
{
    int idx = 3 ;
    QLabel* label = ui()->POT4PortStatusLabel ;

    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(POTPort[idx].Port,&dlg) ;
    window->move(0,0);
    dlg.setWindowTitle(QString("POT%1 Port Settings").arg(idx+1));
    dlg.resize(window->size());
    dlg.exec();

    delete window ;

    if ( POTPort[idx].Port->isOpen() )
    {
        label->setText(QString("串口已打开"));
    }
    else
    {
        label->setText(QString("串口已关闭"));
    }
}
void IKMonitor::on_POT5PortConifgButton_clicked()
{
    int idx = 4 ;
    QLabel* label = ui()->POT5PortStatusLabel ;

    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(POTPort[idx].Port,&dlg) ;
    window->move(0,0);
    dlg.setWindowTitle(QString("POT%1 Port Settings").arg(idx+1));
    dlg.resize(window->size());
    dlg.exec();

    delete window ;

    if ( POTPort[idx].Port->isOpen() )
    {
        label->setText(QString("串口已打开"));
    }
    else
    {
        label->setText(QString("串口已关闭"));
    }
}
void IKMonitor::on_POT6PortConifgButton_clicked()
{
    int idx = 5 ;
    QLabel* label = ui()->POT6PortStatusLabel ;

    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(POTPort[idx].Port,&dlg) ;
    window->move(0,0);
    dlg.setWindowTitle(QString("POT%1 Port Settings").arg(idx+1));
    dlg.resize(window->size());
    dlg.exec();

    delete window ;

    if ( POTPort[idx].Port->isOpen() )
    {
        label->setText(QString("串口已打开"));
    }
    else
    {
        label->setText(QString("串口已关闭"));
    }
}

void IKMonitor::on_POT7PortConifgButton_clicked()
{
    int idx = 6 ;
    QLabel* label = ui()->POT7PortStatusLabel ;

    QDialog dlg ;
    SerialConfigWindow *window = new SerialConfigWindow(POTPort[idx].Port,&dlg) ;
    window->move(0,0);
    dlg.setWindowTitle(QString("POT%1 Port Settings").arg(idx+1));
    dlg.resize(window->size());
    dlg.exec();

    delete window ;

    if ( POTPort[idx].Port->isOpen() )
    {
        label->setText(QString("串口已打开"));
    }
    else
    {
        label->setText(QString("串口已关闭"));
    }

}
void IKMonitor::processPOTComPortData()
{
    QSerialPort *port = qobject_cast<QSerialPort *>(sender()) ;
    if ( NULL == port )
        return ;

    int i ;
    for ( i=0; i<POT_COUNT; i++ )
    {
        if ( port == POTPort[i].Port )
            break ;
    }
    if ( i >= POT_COUNT )
        return ;

    QByteArray array = POTPort[i].Port->readAll() ;
    POTPort[i].DataViewer->addRecvData(array);

    POTPort[i].Data->append(array) ;
    PickupPOTDataFromComDataPkg (i) ;

    int j ;
    for (j=0; j<array.size(); j++)
    {
        SerialParser[i].feedByte(&(SerialParser[i]),array.at(j)) ;
    }
}

void IKMonitor::processPOTUdpPortData()
{
    QUdpSocket *socket = qobject_cast<QUdpSocket *>(sender()) ;
    if ( NULL == socket )
        return ;

    int i ;
    for ( i=0; i<UDP_POT_NODES; i++ )
    {
        if ( socket == UdpPOTPort[i].socket )
            break ;
    }
    if ( i >= UDP_POT_NODES )
        return ;


    while ( UdpPOTPort[i].socket->hasPendingDatagrams() )
    {
        QByteArray array ;
        array.resize(UdpPOTPort[i].socket->pendingDatagramSize());
        UdpPOTPort[i].socket->readDatagram(array.data(),array.size()) ;

        UdpPOTPort[i].DataViewer->addRecvData(array);
        UdpPOTPort[i].Data->append(array);
        PickupPOTDataFromUdpDataPkg(i) ;
    }
}


bool IKMonitor::BadDataChecked ( int nodeID, Quaternion q, Quaternion qg )
{

    if ( nodeID < 1 || nodeID > 7 )
    //if ( nodeID != 5 )
        return true ;

    if ( Quaternion::Dot(q,q) > 1.1 )
        return true ;

    if ( Quaternion::Dot(qg,qg) > 1.1 )
        return true ;

    return false ;
}

bool IKMonitor::DataRejected (uint8_t lhValid , int stationID)
{
    if ( lhValid && stationID == 1 && ui()->KeepOffStation1CheckBox->isChecked() ||
         lhValid && stationID == 2 && ui()->KeepOffStation2CheckBox->isChecked()   )
        return true ;
    else
        return false ;
}

void IKMonitor::PickupPOTDataFromComDataPkg (int idx)
{
    const int packLen = 26 ;
    if ( POTPort[idx].Data->size() >= packLen  )
    {
        int i ;
        int size = POTPort[idx].Data->size() - packLen  ;
        for ( i=0; i<=size; i++ )
        {
            uint8_t* data = (uint8_t*)(POTPort[idx].Data->data()) + i ;
            if ( *data != 'P'  )
            {
                continue ;
            }

            uint8_t checksum = 0 ;
            for (int k=0; k<packLen-1; k++)
            {
                checksum += data[k] ;
            }
            if ( data[packLen-1] != checksum )
                continue ;

            POTDataInfo[idx].FrameCount ++ ;
            POTDataInfo[idx].NodeID = ( data[1] & 0xf ) ;
            POTDataInfo[idx].KeyStatus =  (( data[1] >> 4 ) & 0xf) ;
            POTDataInfo[idx].StationID = ( (data[24] >> 6) & 0x1 ) + 1 ;
            POTDataInfo[idx].dataValid =  ((data[24] >> 7) & 0x1)  ;

            POTDataInfo[idx].ts = ((uint32_t)(data[20]) << 0)  |
                              ((uint32_t)(data[21]) << 8)  |
                              ((uint32_t)(data[22]) << 16)  |
                              ((uint32_t)(data[23]) << 24)  ;

            bool LhValid ;
            if ( POTDataInfo[idx].dataValid )
            {
                LhValid = true ;
                POTDataInfo[idx].AUXFLAG = false ;
            }
            else
            {
                LhValid = false ;
                POTDataInfo[idx].AUXFLAG = true  ;
            }

            int16_t Qx, Qy, Qz;
            int16_t Qgx, Qgy, Qgz;
            int32_t Px, Py, Pz ;
            *(uint16_t*)&(Qx) = ((uint16_t)(data[8]) << 0)  | ((uint16_t)(data[9]) << 8) ;
            *(uint16_t*)&(Qy) = ((uint16_t)(data[10]) << 0) | ((uint16_t)(data[11]) << 8) ;
            *(uint16_t*)&(Qz) = ((uint16_t)(data[12]) << 0) | ((uint16_t)(data[13]) << 8) ;
            *(uint16_t*)&(Qgx) = ((uint16_t)(data[14]) << 0) | ((uint16_t)(data[15]) << 8) ;
            *(uint16_t*)&(Qgy) = ((uint16_t)(data[16]) << 0) | ((uint16_t)(data[17]) << 8) ;
            *(uint16_t*)&(Qgz) = ((uint16_t)(data[18]) << 0) | ((uint16_t)(data[19]) << 8) ;


            uint32_t signMask[3] ;
            signMask[0] = signMask[1] = signMask[2] = 0 ;
            if ( (data[24]>>0)&0x2 )
              signMask[0] = 0xfffc0000 ;
            if ( (data[24]>>2)&0x2 )
              signMask[1] = 0xfffc0000 ;
            if ( (data[24]>>4)&0x2 )
              signMask[2] = 0xfffc0000 ;

            *(uint32_t*)&(Px) = ((uint32_t)(data[2]) << 0) | ((uint32_t)(data[3]) << 8) | ((uint32_t)((data[24]>>0)&0x3) << 16) | signMask[0] ;
            *(uint32_t*)&(Py) = ((uint32_t)(data[4]) << 0) | ((uint32_t)(data[5]) << 8) | ((uint32_t)((data[24]>>2)&0x3) << 16) | signMask[1] ;
            *(uint32_t*)&(Pz) = ((uint32_t)(data[6]) << 0) | ((uint32_t)(data[7]) << 8) | ((uint32_t)((data[24]>>4)&0x3) << 16) | signMask[2] ;

            float qw, qx, qy, qz, norm ;
            float px, py, pz ;
            qx = Qx / 10000.0 ;
            qy = Qy / 10000.0 ;
            qz = Qz / 10000.0 ;
            norm = qx*qx + qy*qy + qz*qz ;
            if ( norm > 1 )
              qw = 0 ;
            else
              qw = sqrt ( 1-norm ) ;
            px = Px / 10000.0 ;
            py = Py / 10000.0 ;
            pz = Pz / 10000.0 ;

            Position rawP ( px, py, pz);
            Quaternion rawQ (qw, qx, qy, qz) ;

            qx = Qgx / 10000.0 ;
            qy = Qgy / 10000.0 ;
            qz = Qgz / 10000.0 ;
            norm = qx*qx + qy*qy + qz*qz ;
            if ( norm > 1 )
              qw = 0 ;
            else
              qw = sqrt ( 1-norm ) ;
            Quaternion rawQg (qw, qx, qy, qz) ;

            bool badDataChecked = BadDataChecked(POTDataInfo[idx].NodeID, rawQ, rawQg ) ;
            if ( badDataChecked )
            {
               uint8_t aa[26] ;
               memcpy(aa,data,26) ;
               printf("bad data:\n");
               int i;
               for (i=0; i<26; i++)
                   printf("%2x ",aa[i]);
               printf("\n\tchecksum:\t");
               uint8_t sum = 0 ;
               for (i=0; i<25; i++)
                   sum += aa[i] ;
               printf("%2x\n",sum);

               printf("\tNodeID: %d\n",POTDataInfo[idx].NodeID);
               printf("\tKeyStatus: %x\n",POTDataInfo[idx].KeyStatus);
               printf("\tDataValid: %d\n",POTDataInfo[idx].dataValid);
               printf("\tStationID: %d\n",POTDataInfo[idx].StationID);
               printf("\tTS: %d\n",POTDataInfo[idx].ts);
               printf("\tPosition: %lf, %lf, %lf\n", px, py, pz);
               printf("\tQuat: %lf, %lf, %lf, %lf\n", rawQ.getW(), rawQ.getX(), rawQ.getY(), rawQ.getZ() ) ;
               printf("\tQuatGyro: %lf, %lf, %lf, %lf\n", rawQg.getW(), rawQg.getX(), rawQg.getY(), rawQg.getZ() ) ;
            }


            bool dataRejected = DataRejected (POTDataInfo[idx].dataValid, POTDataInfo[idx].StationID) ;

            if ( ! badDataChecked && ! dataRejected )
            {
                int k = POTDataInfo[idx].NodeID - 1 ;
                NodeKeyStatus [k] = POTDataInfo[idx].KeyStatus ;
                POTSystem[k]->processRaw(rawP,rawQ, rawQg, POTDataInfo[idx].StationID, POTDataInfo[idx].ts, LhValid );

                if (ui()->useCOMRadioButton->isChecked() )
                {
                    addSampleToOscilloscopes(POTSystem[k]) ;
                    _3DViewer->setMarkCube(k,POTSystem[k]->getPos(), POTSystem[k]->getOri().toEuler().RadToDegree());

                    if ( WorkMode == TEST_POT )
                    {
                        if ( POTDataInfo[idx].NodeID==ActivedPOTNodeID )
                        {
                            _3DViewer->IKResult.HipPos = POTSystem[k]->getPos();
                            _3DViewer->IKResult.HipOri = POTSystem[k]->getOri().toEuler().RadToDegree();
                        }
                    }
                    else if ( WorkMode == TEST_IK )
                    {
//                        if(POTSystem[k]->isCalibrated())

                        if(POTSystem[k]->LHPOT()->isCalibrated())
                        {
                            POT** pot ;
                            pot = POTSystem ;
                            int TestMode = 0;
#ifdef WholeBody
    TestMode = 1;
#endif
#ifdef BothArm
    TestMode = 2;
#endif
                            if (ui()->usePOT_RadioButton->isChecked() )
                                IKSrc = 1;
                            else if(ui()->useFK_RadioButton->isChecked())
                                IKSrc = 0;

                            inputAdaptor.process(POTDataInfo[idx].NodeID,  _3DViewer->humanIK, pot, TestMode, IKSrc);  // 0:FK-pos  1:POT-pos
                            _3DViewer->updateBonesOriWithIKresult(_3DViewer->FKResult);//, Vector3D(-1,0,0));

                           // inputAdaptor.process(POTDataInfo[idx].NodeID,  _3DViewer->humanIK, pot, TestMode, 1);  // 0:FK-pos  1:POT-pos
                           // _3DViewer->updateBonesOriWithIKresult(_3DViewer->IKResult);

                            Vector3D P;
                            Quaternion Q;

//////////////////////////////////
/// Debug LeftArm
//P = inputAdaptor.debugLeftArm.PNeck ;
//Q = inputAdaptor.debugLeftArm.QRoot;
//_3DViewer->MarkCubeDebugLeftArm[0].Pos = P ;
//_3DViewer->MarkCubeDebugLeftArm[0].Ori = Q.toEuler().RadToDegree() ;
//
//P = inputAdaptor.debugLeftArm.PShoulder;
//Q = inputAdaptor.debugLeftArm.QLinker;
//_3DViewer->MarkCubeDebugLeftArm[1].Pos = P ;
//_3DViewer->MarkCubeDebugLeftArm[1].Ori = Q.toEuler().RadToDegree() ;
//
//P = inputAdaptor.debugLeftArm.PElbow;
//Q = inputAdaptor.debugLeftArm.QThis;
//_3DViewer->MarkCubeDebugLeftArm[2].Pos = P ;
//_3DViewer->MarkCubeDebugLeftArm[2].Ori = Q.toEuler().RadToDegree() ;
//
//P = inputAdaptor.debugLeftArm.PWrist ;
//Q = Quaternion(1,0,0,0);
//_3DViewer->MarkCubeDebugLeftArm[3].Pos = P ;
//_3DViewer->MarkCubeDebugLeftArm[3].Ori = Q.toEuler().RadToDegree() ;
//////////////////////////////////
/// Debug LeftArm End

//////////////////////////////////
///Debug Neck
P = inputAdaptor.pNeck.LeftWrist;
Q = Quaternion(1,0,0,0);
_3DViewer->MarkCubeDebugLeftArm[0].Pos = P ;
_3DViewer->MarkCubeDebugLeftArm[0].Ori = Q.toEuler().RadToDegree() ;
_3DViewer->MarkCubeDebugLeftArm[0].Color = _3DViewer->MarkCubeFK[0].Color;

P = inputAdaptor.pNeck.RightWrist;
Q = Quaternion(1,0,0,0);
_3DViewer->MarkCubeDebugLeftArm[1].Pos = P ;
_3DViewer->MarkCubeDebugLeftArm[1].Ori = Q.toEuler().RadToDegree() ;
_3DViewer->MarkCubeDebugLeftArm[1].Color = _3DViewer->MarkCubeFK[1].Color;

P = inputAdaptor.pNeck.P_Neck;
Q = Quaternion(1,0,0,0);
_3DViewer->MarkCubeDebugLeftArm[2].Pos = P ;
_3DViewer->MarkCubeDebugLeftArm[2].Ori = Q.toEuler().RadToDegree() ;
_3DViewer->MarkCubeDebugLeftArm[2].Color = _3DViewer->MarkCubeFK[2].Color;


P = inputAdaptor.pNeck.Legs;
Q = Quaternion(1,0,0,0);
_3DViewer->MarkCubeDebugLeftArm[3].Pos = P ;
_3DViewer->MarkCubeDebugLeftArm[3].Ori = Q.toEuler().RadToDegree() ;
_3DViewer->MarkCubeDebugLeftArm[3].Color = _3DViewer->MarkCubeFK[3].Color;
//////////////////////////////////
///Debug Neck End

                            P = inputAdaptor.resultBuffer.fkResult_LeftWrist;
                            Q = Quaternion(1,0,0,0) ;
                            _3DViewer->MarkCubeFKResult[1].Pos = P ;
                            _3DViewer->MarkCubeFKResult[1].Ori = Q.toEuler().RadToDegree() ;

                            P = inputAdaptor.resultBuffer.fkResult_RightWrist;
                            Q = Quaternion(1,0,0,0) ;
                            _3DViewer->MarkCubeFKResult[2].Pos = P ;
                            _3DViewer->MarkCubeFKResult[2].Ori = Q.toEuler().RadToDegree() ;

                            P = inputAdaptor.resultBuffer.fkResult_Neck;
                            Q = Quaternion(1,0,0,0) ;
                            _3DViewer->MarkCubeFKResult[0].Pos = P ;
                            _3DViewer->MarkCubeFKResult[0].Ori = Q.toEuler().RadToDegree() ;



                            P = inputAdaptor.resultBuffer.fkP_LeftWrist;
                            Q = _3DViewer->humanIK.LatestJonitsRawData[0].Q ;
                            _3DViewer->MarkCubeFK[1].Pos = P ;
                            _3DViewer->MarkCubeFK[1].Ori = Q.toEuler().RadToDegree() ;

                            P = inputAdaptor.resultBuffer.fkP_RightWrist;
                            Q = _3DViewer->humanIK.LatestJonitsRawData[1].Q ;
                            _3DViewer->MarkCubeFK[2].Pos = P ;
                            _3DViewer->MarkCubeFK[2].Ori = Q.toEuler().RadToDegree() ;

                            P = inputAdaptor.resultBuffer.fkP_Neck;
                            Q = _3DViewer->humanIK.LatestJonitsRawData[4].Q ;
                            _3DViewer->MarkCubeFK[0].Pos = P ;
                            _3DViewer->MarkCubeFK[0].Ori = Q.toEuler().RadToDegree() ;

                            P = inputAdaptor.resultBuffer.fkP_LeftAnkle;
                            Q = _3DViewer->humanIK.LatestJonitsRawData[2].Q ;
                            _3DViewer->MarkCubeFK[3].Pos = P ;
                            _3DViewer->MarkCubeFK[3].Ori = Q.toEuler().RadToDegree() ;

                            P = inputAdaptor.resultBuffer.fkP_RightAnkle;
                            Q = _3DViewer->humanIK.LatestJonitsRawData[3].Q ;
                            _3DViewer->MarkCubeFK[4].Pos = P ;
                            _3DViewer->MarkCubeFK[4].Ori = Q.toEuler().RadToDegree() ;
                        }
                    }
                }
                TcpContentChanged = true ;
                _3DViewer->show3D();
            }
            else
            {
                POTDataInfo[idx].StationID = 0 ;
            }

            StatusContentChanged = true ;
        }

        POTPort[idx].Data->remove(0,i) ;
    }

}
void IKMonitor::PickupPOTDataFromUdpDataPkg(int idx)
{
    const int packLen = 20 ;
    if ( UdpPOTPort[idx].Data->size() >= packLen  )
    {
        int i ;
        int size = UdpPOTPort[idx].Data->size() - packLen  ;
        for ( i=0; i<=size; i++ )
        {
            uint8_t* data = (uint8_t*)(UdpPOTPort[idx].Data->data()) + i ;
            if ( *data != 'P'  )
            {
                continue ;
            }

            uint8_t checksum = 0 ;
            for (int k=0; k<packLen-1; k++)
            {
                checksum += data[k] ;
            }
            if ( data[packLen-1] != checksum )
                continue ;

            UdpPOTDataInfo[idx].FrameCount ++ ;
            UdpPOTDataInfo[idx].NodeID = ( data[1] & 0xf ) ;
            UdpPOTDataInfo[idx].KeyStatus =  (( data[1] >> 4 ) & 0xf) ;
            UdpPOTDataInfo[idx].StationID = ( (data[24] >> 6) & 0x1 ) + 1 ;
            UdpPOTDataInfo[idx].dataValid =  ((data[24] >> 7) & 0x1)  ;

            UdpPOTDataInfo[idx].ts = ((uint32_t)(data[20]) << 0)  |
                    ((uint32_t)(data[21]) << 8)  |
                    ((uint32_t)(data[22]) << 16)  |
                    ((uint32_t)(data[23]) << 24)  ;


            bool LhValid ;
            if ( UdpPOTDataInfo[idx].dataValid )
            {
                LhValid = true ;
                UdpPOTDataInfo[idx].AUXFLAG = false ;
            }
            else
            {
                LhValid = false ;
                UdpPOTDataInfo[idx].AUXFLAG = true  ;
            }

            int16_t Qx, Qy, Qz;
            int16_t Qgx, Qgy, Qgz;
            int32_t Px, Py, Pz ;
            *(uint16_t*)&(Qx)  =  ((uint16_t)(data[8]) << 0)  |  ((uint16_t)(data[9]) << 8) ;
            *(uint16_t*)&(Qy)  =  ((uint16_t)(data[10]) << 0)  |  ((uint16_t)(data[11]) << 8) ;
            *(uint16_t*)&(Qz)  =  ((uint16_t)(data[12]) << 0)  |  ((uint16_t)(data[13]) << 8) ;
            *(uint16_t*)&(Qgx)  =  ((uint16_t)(data[14]) << 0)  |  ((uint16_t)(data[15]) << 8) ;
            *(uint16_t*)&(Qgy)  =  ((uint16_t)(data[16]) << 0)  |  ((uint16_t)(data[17]) << 8) ;
            *(uint16_t*)&(Qgz)  =  ((uint16_t)(data[18]) << 0)  |  ((uint16_t)(data[19]) << 8) ;


            uint32_t signMask[3] ;
            signMask[0] = signMask[1] = signMask[2] = 0 ;
            if ( (data[24]>>0)&0x2 )
              signMask[0] = 0xfffc0000 ;
            if ( (data[24]>>2)&0x2 )
              signMask[1] = 0xfffc0000 ;
            if ( (data[24]>>4)&0x2 )
              signMask[2] = 0xfffc0000 ;

            *(uint32_t*)&(Px)  =  ((uint32_t)(data[2]) << 0)  |  ((uint32_t)(data[3]) << 8)  |  ((uint32_t)((data[18]>>0)&0x3) << 16) |  signMask[0] ;
            *(uint32_t*)&(Py)  =  ((uint32_t)(data[4]) << 0)  |  ((uint32_t)(data[5]) << 8) |  ((uint32_t)((data[18]>>2)&0x3) << 16) |  signMask[1] ;
            *(uint32_t*)&(Pz)  =  ((uint32_t)(data[6]) << 0)  |  ((uint32_t)(data[7]) << 8) |  ((uint32_t)((data[18]>>4)&0x3) << 16) |  signMask[2] ;

          float qw, qx, qy, qz, norm ;
          float px, py, pz ;
          qx = Qx / 10000.0 ;
          qy = Qy / 10000.0 ;
          qz = Qz / 10000.0 ;
          norm = qx*qx + qy*qy + qz*qz ;
          if ( norm > 1 )
            qw = 0 ;
          else
            qw = sqrt ( 1-norm ) ;
          px = Px / 10000.0 ;
          py = Py / 10000.0 ;
          pz = Pz / 10000.0 ;

            Position rawP ( px, py, pz);
            Quaternion rawQ (qw, qx, qy, qz) ;

            qx = Qgx / 10000.0 ;
            qy = Qgy / 10000.0 ;
            qz = Qgz / 10000.0 ;
            norm = qx*qx + qy*qy + qz*qz ;
            if ( norm > 1 )
              qw = 0 ;
            else
              qw = sqrt ( 1-norm ) ;
            Quaternion rawQg (qw, qx, qy, qz) ;


            bool nodeIDLegal = true ;
            int k = UdpPOTDataInfo[idx].NodeID - 1 ;
            if ( k > sizeof(UdpPOTDataInfo)/sizeof(UdpPOTDataInfo[0]) - 1 )
            {
qDebug()<<"Unexpected NodeID "<<UdpPOTDataInfo[idx].NodeID<< " in UdpChannel  "<< idx+1 ;
                nodeIDLegal = false ;
            }



//            if ( UdpPOTDataInfo[idx].dataValid && nodeIDLegal )
            if ( nodeIDLegal )
            {
                NodeKeyStatus [k] = UdpPOTDataInfo[idx].KeyStatus ;
                UdpPOTSystem[k]->processRaw(rawP, rawQ, rawQg, POTDataInfo[idx].StationID, POTDataInfo[idx].ts, LhValid);

                if ( ui()->useUDPRadioButton->isChecked() )
                {
                    addSampleToOscilloscopes(UdpPOTSystem[k]) ;
                    _3DViewer->setMarkCube(k,UdpPOTSystem[k]->getPos(), UdpPOTSystem[k]->getOri().toEuler().RadToDegree());

                    if ( WorkMode == TEST_POT )
                    {
                        if ( UdpPOTDataInfo[idx].NodeID==ActivedPOTNodeID )
                        {
                            _3DViewer->IKResult.HipPos = UdpPOTSystem[k]->getPos();
                            _3DViewer->IKResult.HipOri = UdpPOTSystem[k]->getOri().toEuler().RadToDegree();
                        }
                    }
                    else if ( WorkMode == TEST_IK )
                    {
//                        if(UdpPOTSystem[k]->isCalibrated())
//                        {
//                            IK_TestArm();
//                        }
                        if(UdpPOTSystem[k]->LHPOT()->isCalibrated())
                        {
                            POT** pot ;
                            pot = UdpPOTSystem ;

                            int TestMode;
                            TestMode = 0;
                             inputAdaptor.process(POTDataInfo[idx].NodeID,  _3DViewer->humanIK, pot, TestMode, 0);
                            //inputAdaptor.process(UdpPOTDataInfo[idx].NodeID,  _3DViewer->humanIK, pot, IKSrc); // 0:FK-pos  1:POT-pos
                            _3DViewer->updateBonesOriWithIKresult(_3DViewer->IKResult);
                        }

                    }
                }
                TcpContentChanged = true ;
                _3DViewer->show3D();
            }
            else
            {
                UdpPOTDataInfo[idx].StationID = 0 ;
            }


            StatusContentChanged = true ;

        }

        UdpPOTPort[idx].Data->remove(0,i) ;
    }
}

void IKMonitor::TcpSendTestPkg()
{

    QString TcpStr;
    TcpStr += "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ";

    TcpStr += ui()->TCP_PX_lineEdit->text();//.QString; //lineEdit_InputHeight->text().QString::toFloat()
    TcpStr += " ";
    TcpStr += ui()->TCP_PY_lineEdit->text();//.QString;
    TcpStr += " ";
    TcpStr += ui()->TCP_PZ_lineEdit->text();//.QString;
    TcpStr += " 0 0 20||";

    QByteArray ba = TcpStr.toLatin1();
    const char *c_str2 = ba.data();
    this->tcpSocket->write(c_str2);
}

void IKMonitor::TcpSendPkg()
{
    Vector3D  V ;
    EulerAngle E;
    QString TcpStr;

//    const HumanIK::OutputFrame & body  = _3DViewer->humanIK.output()  ;
    const Virtual3dCharacterViewer::Character3DBody & body  =  _3DViewer->FKResult ;

//    Float delta = ui()->TCP_PY_lineEdit->text().toDouble();

    V = body.HipPos;
//    V.setY(V.getY() + delta);
    TcpStr += QString::number(V.getX());    //"1"; //QString::number(V.X);
    TcpStr += " ";
    TcpStr += QString::number(V.getY());    //"1.13"; //QString::number(V.Y);
    TcpStr += " ";
    TcpStr += QString::number(V.getZ());    //QString::number(V.getZ());
    TcpStr += " ";

    E = body.HipOri;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    E = body.RightThighOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    E = body.RightShankOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    // Right Foot
    TcpStr += QString("0.00 0.00 0.00");
    TcpStr += " ";

    E = body.LeftThighOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    E = body.LeftShankOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    //                  Left Foot      S0             S1
    TcpStr += QString("0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00");
    TcpStr += " ";
    E = body.ChestOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";
    //                  S3             Neck
    TcpStr += QString("0.00 0.00 0.00 0.00 0.00 0.00");
    TcpStr += " ";

    E = body.HeadOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    E = body.RightShoulderOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    E = body.RightUpperArmOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    E = body.RightLowerArmOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";

    E = body.RightHandOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";
    E = body.LeftShoulderOri ;
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";
    E = body.LeftUpperArmOri ;
    //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";
    E = body.LeftLowerArmOri ;
    //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += " ";
    E = body.LeftHandOri ;
    //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    TcpStr += QString::number(E.getYaw());
    TcpStr += " ";
    TcpStr += QString::number(E.getPitch());
    TcpStr += " ";
    TcpStr += QString::number(E.getRoll());
    TcpStr += QString("||");
    //QString str1 = "Hello";
    //str1 += " world";
    QByteArray ba = TcpStr.toLatin1();
    const char *c_str2 = ba.data();
    this->tcpSocket->write(c_str2);
}

void IKMonitor::processMocapComPortData()
{
    QByteArray array = MocapComPort->readAll() ;
    MocapPortDataViewer->addRecvData(array);

    MocapComData->append(array) ;
    PickupMocapDataFromComDataPkg () ;
}

void IKMonitor::updateHumanIKModel ()
{
    int idx ;
    idx = 0 ;

    HumanIK::RawDataFrame rawData ;
    HumanIK &humanIK = _3DViewer->humanIK ;

    POT** pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem ;
    else
        pot = UdpPOTSystem ;

    idx = 5 - 1 ;
    rawData.id = HumanIK::Joint_Neck;
    rawData.Pos = pot[idx]->getPos() ;
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    idx = 1 - 1 ;
    rawData.id = HumanIK::Joint_LeftWrist ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    idx = 2 - 1 ;
    rawData.id = HumanIK::Joint_RightWrist ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    idx = 3 - 1 ;
    rawData.id = HumanIK::Joint_LeftAnkle ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    idx = 4 - 1 ;
    rawData.id = HumanIK::Joint_RightAnkle ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    int TestMode = 0;
#ifdef WholeBody
    TestMode = 1;
#endif
#ifdef BothArm
    TestMode = 2;
#endif
    humanIK.ModelAdjustment(TestMode);
    _3DViewer->FKResult.Model = humanIK.getModel() ;
    _3DViewer->IKResult.Model = humanIK.getModel() ;

    inputAdaptor.setFKModel(humanIK.getModel());
    inputAdaptor.setFKOffset(humanIK.getTOffset());
}

void IKMonitor::PickupMocapDataFromComDataPkg ()
{
    const int idmap[12] = {0,1,2,3,6,7,8,11,12,13,15,16} ;
    const int packLen = 30 ;
    if ( MocapComData->size() >= packLen  )
    {
        int i ;
        int size = MocapComData->size() - packLen  ;
        for ( i=0; i<=size; i++ )
        {
            uint8_t* data = (uint8_t*)(MocapComData->data()) + i ;
            uint8_t id = *(data+1) ;
            if ( *data != '$' || ! mocapIMU->isSlaveIDValid(idmap[id])
                 || *(data+28) != 0x0d || *(data+29) != 0x0a )
            {
                continue ;
            }

            MocapDataInfo.FrameCount ++ ;
            i += packLen - 1 ;

            data +=2 ;
            int16_t  D[13] ;
            uint8_t* d = (uint8_t*)D ;
            for (int i=0; i<13; i++)
            {
                d[2*i] = data[2*i + 1] ;
                d[2*i+1] = data[2*i] ;
            }

            int16_t qw = D[0] ;
            int16_t qx = D[1] ;
            int16_t qy = D[2] ;
            int16_t qz = D[3] ;

            int16_t ax = D[4] ;
            int16_t ay = D[5] ;
            int16_t az = D[6] ;

            int16_t gx = D[7] ;
            int16_t gy = D[8] ;
            int16_t gz = D[9] ;

            int16_t mx = D[10] ;
            int16_t my = D[11] ;
            int16_t mz = D[12] ;

            cQuaternion q ;
            cEulerAngle e ;
            q.w = (float)qw/10000 ;
            q.x = (float)qx/10000 ;
            q.y = (float)qy/10000 ;
            q.z = (float)qz/10000 ;
            cQuatToEuler ( &q , &e ) ;
            int16_t roll = e.roll * 180 / PI * 100;
            int16_t pitch = e.pitch * 180 / PI * 100 ;
            int16_t yaw = e.yaw * 180 / PI * 100 ;

            Quaternion Q ( q.w, q.x, q.y, q.z ) ;
            mocapIMU->getSlaveNode(idmap[id])->setQraw(Q);
            mocapIMU->compute();
            EulerAngle Eu = mocapIMU->getSlaveNode(idmap[id])->getEuler();
            setEulerFK ( idmap[id], Eu ) ;
            //_3DViewer->show3D();
            if ( FK2IKReady && FK_isCali)
            {
                FK2IKReady = false ;
                FK_to_IK ();
            }
           /* if (StatusReady)
            {
                StatusReady = false ;
                updateStatusPanel();
            }*/

            //_3DViewer->show3D();

            if(this->TcpSend){
                Vector3D  V ;
                EulerAngle E;
                QString TcpStr;

                V = _3DViewer->IKResult.HipPos;//IKResult.HipPos ;
                //TcpStr += QString("%1 %2 %3").ArgCoordinate(V.X).ArgCoordinate(V.Y).ArgCoordinate(V.Z);
                TcpStr += "1"; //QString::number(V.X);
                TcpStr += " ";
                TcpStr += "1.13"; //QString::number(V.Y);
                TcpStr += " ";
                TcpStr += QString::number(V.getZ());
                TcpStr += " ";

                E = _3DViewer->IKResult.HipOri;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                E = _3DViewer->IKResult.RightThighOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                E = _3DViewer->IKResult.RightShankOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                // Right Foot
                TcpStr += QString("0.00 0.00 0.00");
                TcpStr += " ";

                E = _3DViewer->IKResult.LeftThighOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                E = _3DViewer->IKResult.LeftShankOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                //                  Left Foot      S0             S1
                TcpStr += QString("0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00");
                TcpStr += " ";
                E = _3DViewer->IKResult.ChestOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";
                //                  S3             Neck
                TcpStr += QString("0.00 0.00 0.00 0.00 0.00 0.00");
                TcpStr += " ";

                E = _3DViewer->IKResult.HeadOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                E = _3DViewer->IKResult.RightShoulderOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                E = _3DViewer->IKResult.RightUpperArmOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                E = _3DViewer->IKResult.RightLowerArmOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";

                E = _3DViewer->IKResult.RightHandOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";
                E = _3DViewer->IKResult.LeftShoulderOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";
                E = _3DViewer->IKResult.LeftUpperArmOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";
                E = _3DViewer->IKResult.LeftLowerArmOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += " ";
                E = _3DViewer->IKResult.LeftHandOri ;
                //TcpStr += QString("%1 %2 %3").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
                TcpStr += QString::number(E.getYaw());
                TcpStr += " ";
                TcpStr += QString::number(E.getPitch());
                TcpStr += " ";
                TcpStr += QString::number(E.getRoll());
                TcpStr += QString("||");
                //QString str1 = "Hello";
                //str1 += " world";
                QByteArray ba = TcpStr.toLatin1();
                const char *c_str2 = ba.data();
                this->tcpSocket->write(c_str2);
            }
            else{
                //_3DViewer->show3D();
            }
        }
        MocapComData->remove(0,i) ;
    }
}

void IKMonitor::on_resetSightButton_clicked()
{
    _3DViewer->WorldInCamera.Ori = EulerAngle(0,0,0) ;
    _3DViewer->WorldInCamera.Pos = Vector3D(0,0,-2) ;
}

void IKMonitor::on_resetRobotButton_clicked()
{
    _3DViewer->initVirtualBody();
}

void IKMonitor::onTcpSend()
{
    if ( TcpSend )  // for test
    {
        //TcpSendTestPkg();
        TcpSendPkg();
        TcpContentChanged = false ;
    }
}

void IKMonitor::updateFPS()
{
    int idx ;
    for (idx=0; idx<POT_COUNT; idx++)
    {
        POTDataInfo[idx].FPS = POTDataInfo[idx].FrameCount - POTDataInfo[idx].preFrameCount ;
        POTDataInfo[idx].preFrameCount = POTDataInfo[idx].FrameCount ;
    }
    for (idx=0; idx<UDP_POT_NODES; idx++)
    {
        UdpPOTDataInfo[idx].FPS = UdpPOTDataInfo[idx].FrameCount - UdpPOTDataInfo[idx].preFrameCount ;
        UdpPOTDataInfo[idx].preFrameCount = UdpPOTDataInfo[idx].FrameCount ;
    }


    POT* pot = NULL  ;
    if ( ui()->useCOMRadioButton->isChecked() )
    {
        pot = POTSystem[ActivedPOTNodeID - 1] ;
    }
    else if ( ui()->useUDPRadioButton->isChecked() )
    {
        pot = UdpPOTSystem[ActivedPOTNodeID - 1] ;
    }
    if ( pot )
    {
        int n = pot->pLHPOT()->stationNum() ;
        for ( int i=0; i<n; i++ )
        {
            pot->pLHPOT()->LatestRaw[i].FPS = pot->pLHPOT()->LatestRaw[i].Frames - pot->pLHPOT()->LatestRaw[i].PreFrames ;
            pot->pLHPOT()->LatestRaw[i].PreFrames = pot->pLHPOT()->LatestRaw[i].Frames ;
        }
    }


    ui()->MocapFPSLabel->setText(QString("FPS: %1").arg(MocapDataInfo.FPS));
    ui()->POT1FPSLabel->setText(QString("FPS: %1").arg(POTDataInfo[0].FPS));
    ui()->POT2FPSLabel->setText(QString("FPS: %1").arg(POTDataInfo[1].FPS));
    ui()->POT3FPSLabel->setText(QString("FPS: %1").arg(POTDataInfo[2].FPS));
    ui()->POT4FPSLabel->setText(QString("FPS: %1").arg(POTDataInfo[3].FPS));
    ui()->POT5FPSLabel->setText(QString("FPS: %1").arg(POTDataInfo[4].FPS));
    ui()->POT6FPSLabel->setText(QString("FPS: %1").arg(POTDataInfo[5].FPS));
    ui()->POT7FPSLabel->setText(QString("FPS: %1").arg(POTDataInfo[6].FPS));

    ui()->UdpPOT1FPSLabel->setText(QString("FPS: %1").arg(UdpPOTDataInfo[0].FPS));
    ui()->UdpPOT2FPSLabel->setText(QString("FPS: %1").arg(UdpPOTDataInfo[1].FPS));
    ui()->UdpPOT3FPSLabel->setText(QString("FPS: %1").arg(UdpPOTDataInfo[2].FPS));
    ui()->UdpPOT4FPSLabel->setText(QString("FPS: %1").arg(UdpPOTDataInfo[3].FPS));
    ui()->UdpPOT5FPSLabel->setText(QString("FPS: %1").arg(UdpPOTDataInfo[4].FPS));
    ui()->UdpPOT6FPSLabel->setText(QString("FPS: %1").arg(UdpPOTDataInfo[5].FPS));
    //ui()->UdpPOT7FPSLabel->setText(QString("FPS: %1").arg(UdpPOTDataInfo[6].FPS));

    MocapDataInfo.FPS = MocapDataInfo.FrameCount - MocapDataInfo.preFrameCount ;
    MocapDataInfo.preFrameCount = MocapDataInfo.FrameCount ;
}

void IKMonitor::prepareUpdateStatusPanel()
{
    if ( StatusContentChanged )
    {
        StatusContentChanged = false ;
        updateStatusPanel();
    }
}

void IKMonitor::prepareUpdateFK2IK()
{
    FK2IKReady = true ;
}

void IKMonitor::resizeEvent(QResizeEvent * event)
{
    _3DViewer->setGeometry( 10, 20, ui()->_3DPlayerGroupBox->width()-20, ui()->_3DPlayerGroupBox->height()-30 );
}

void IKMonitor::updateStatusPanel()
{
#define argP(x) arg(((int)((x)*10000))/10000.0,5,'g')
#define argQ(x) arg(((int)((x)*1000))/1000.0,5,'g')
#define argTM(x) arg(((int)((x)*1000))/1000.0,5,'g')
#define ArgCoordinate(d) arg(((int)((d)*10000))/10000.0, 5, 'g')
#define ArgAngle(a) arg(((int)((a)*100))/100.0, 6, 'g')
    QString str ;
    Vector3D  V ;
    Quaternion Q;
    EulerAngle E ;
    int i ;
    str.clear();
    int idx = ActivedPOTNodeID-1 ;

    POT* pot ;
    Matrix TM ;
//////////////////////////////
////  ui()->POTStatusText
//////////////////////////////
    str += QString("******** TransMat ********\n") ;
    if ( ui()->useCOMRadioButton->isChecked() )
    {
        pot = POTSystem[ActivedPOTNodeID-1] ;
    }
    else if( ui()->useUDPRadioButton->isChecked() )
    {
        pot = UdpPOTSystem[ActivedPOTNodeID-1] ;
    }
    TM = pot->LHPOT()->getTransMat(1) ;
    str += QString("Transform Matrix 1:\n") ;
    str += QString("%1 %2 %3 %4\n").argTM(TM(0,0)).argTM(TM(0,1)).argTM(TM(0,2)).argTM(TM(0,3)) ;
    str += QString("%1 %2 %3 %4\n").argTM(TM(1,0)).argTM(TM(1,1)).argTM(TM(1,2)).argTM(TM(1,3)) ;
    str += QString("%1 %2 %3 %4\n").argTM(TM(2,0)).argTM(TM(2,1)).argTM(TM(2,2)).argTM(TM(2,3)) ;
    TM = pot->LHPOT()->getTransMat(2) ;
    str += QString("Transform Matrix 2:\n") ;
    str += QString("%1 %2 %3 %4\n").argTM(TM(0,0)).argTM(TM(0,1)).argTM(TM(0,2)).argTM(TM(0,3)) ;
    str += QString("%1 %2 %3 %4\n").argTM(TM(1,0)).argTM(TM(1,1)).argTM(TM(1,2)).argTM(TM(1,3)) ;
    str += QString("%1 %2 %3 %4\n").argTM(TM(2,0)).argTM(TM(2,1)).argTM(TM(2,2)).argTM(TM(2,3)) ;

    str += QString("ActivedPOTNodeID is : %1\n").arg(ActivedPOTNodeID) ;
    str += QString("Latest LighthouseValid is : %1\n").arg(pot->d->LatestLhValid) ;
    if ( pot->d->LatestLhValid )
        str += QString("Latest StationID is : %1\n").arg(pot->pLHPOT()->LatestStationID) ;
    else
        str += QString("Latest StationID is : -\n") ;
    str += QString("LatestTimestamp is : %1\n").arg(pot->pLHPOT()->LatestTimeStamp)  ;
    str += QString("Latest KeyStatus is : 0x%1\n").arg((uint32_t)(NodeKeyStatus[idx]),2,16) ;
    str += QString("FPS of Station 1 : %1\n").arg(pot->pLHPOT()->LatestRaw[0].FPS) ;
    str += QString("FPS of Station 2 : %1\n").arg(pot->pLHPOT()->LatestRaw[1].FPS) ;

    V = pot->pLHFusion()->PosOffset ;
    Q = pot->pLHFusion()->OriOffset ;
    str += QString("POS_OFFSET:\n%1  %2  %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;
    str += QString("ORI_DIFF  :\n%1    %2  %3  %4\n").argQ(Q.getW()).argQ(Q.getX()).argQ(Q.getY()).argQ(Q.getZ()) ;
    str += "\n" ;

    str += "******** POT RAW DATA ********\n";
    V = pot->LHPOT()->getLatestRawPos(1);
    Q = pot->LHPOT()->getLatestRawOri(1);
    str += QString("LatestRawP1: %1 %2 %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;
    str += QString("LatestRawQ1: %1 %2 %3 %4\n").argQ(Q.getW()).argQ(Q.getX()).argQ(Q.getY()).argQ(Q.getZ()) ;
    V = pot->LHPOT()->getLatestRawPos(2);
    Q = pot->LHPOT()->getLatestRawOri(2);
    str += QString("LatestRawP2: %1 %2 %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;
    str += QString("LatestRawQ2: %1 %2 %3 %4\n").argQ(Q.getW()).argQ(Q.getX()).argQ(Q.getY()).argQ(Q.getZ()) ;

    if(pot->LHPOT()->isCalibrated())
    {
        str += "******** POT MAPPED DATA (getHead*) ********\n";
        str += QString("Current Position:\n%1 %2 %3\n").argP(pot->getPos().getX()).argP(pot->getPos().getY()).argP(pot->getPos().getZ()) ;
        str += QString("Current Rotation:\n%1 %2 %3 %4\n\n").argQ(pot->getOri().getW()).argQ(pot->getOri().getX()).argQ(pot->getOri().getY()).argQ(pot->getOri().getZ()) ;
        str += "\n" ;
    }
    if ( ui()->useCOMRadioButton->isChecked() )
    {
        str += QString("**********Left ARM********\n") ;
        str += QString("PN: %1 %2 %3\n").argP(POTSystem[0]->getPos().getX()).argP(POTSystem[0]->getPos().getY()).argP(POTSystem[0]->getPos().getZ()) ;
        str += QString("LatestRawQ1: %1 %2 %3 %4\n\n").argQ(POTSystem[0]->getOri().getW()).argQ(POTSystem[0]->getOri().getX()).argQ(POTSystem[0]->getOri().getY()).argQ(POTSystem[0]->getOri().getZ()) ;
        str += QString("***********Right ARM********\n") ;
        str += QString("PN: %1 %2 %3\n").argP(POTSystem[1]->getPos().getX()).argP(POTSystem[1]->getPos().getY()).argP(POTSystem[1]->getPos().getZ()) ;
        str += QString("LatestRawQ1: %1 %2 %3 %4\n").argQ(POTSystem[1]->getOri().getW()).argQ(POTSystem[1]->getOri().getX()).argQ(POTSystem[1]->getOri().getY()).argQ(POTSystem[1]->getOri().getZ()) ;
    }
    else
    {
        str += QString("**********Left ARM********\n") ;
        str += QString("PN: %1 %2 %3\n").argP(UdpPOTSystem[0]->getPos().getX()).argP(UdpPOTSystem[0]->getPos().getY()).argP(UdpPOTSystem[0]->getPos().getZ()) ;
        str += QString("LatestRawQ1: %1 %2 %3 %4\n\n").argQ(UdpPOTSystem[0]->getOri().getW()).argQ(UdpPOTSystem[0]->getOri().getX()).argQ(UdpPOTSystem[0]->getOri().getY()).argQ(UdpPOTSystem[0]->getOri().getZ()) ;
        str += QString("***********Right ARM********\n") ;
        str += QString("PN: %1 %2 %3\n").argP(UdpPOTSystem[1]->getPos().getX()).argP(UdpPOTSystem[1]->getPos().getY()).argP(UdpPOTSystem[1]->getPos().getZ()) ;
        str += QString("LatestRawQ1: %1 %2 %3 %4\n").argQ(UdpPOTSystem[1]->getOri().getW()).argQ(UdpPOTSystem[1]->getOri().getX()).argQ(UdpPOTSystem[1]->getOri().getY()).argQ(UdpPOTSystem[1]->getOri().getZ()) ;
    }
    ui()->POTStatusText->setText(str);
    str.clear();

//////////////////////////////
////  ui()->IKStatusText
//////////////////////////////
    //str += "Selected Joint is : " ;
    //str += _3DViewer->JointName(_3DViewer->SelectedJointID) ;

    str += "      ****** IK RESULT (Euler Angle)******\n " ;
    str += "BODY\n" ;
    V = _3DViewer->IKResult.HipPos ;
    str += QString("HipPos:           %1, %2, %3 \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
    E = _3DViewer->IKResult.HipOri ;
    str += QString("HipOri:           %1, %2, %3  \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.ChestOri ;
    str += QString("ChestOri:         %1, %2, %3  \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

    str += "\nLeft Arm\n" ;
    E = _3DViewer->IKResult.LeftShoulderOri;
    str += QString("LeftShoulderOri:  %1, %2, %3  \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.LeftUpperArmOri ;
    str += QString("LeftUpperArmOri:  %1, %2, %3  \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.LeftLowerArmOri ;
    str += QString("LeftLowerArmOri:  %1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

    str += "\nRight Arm\n" ;
    E = _3DViewer->IKResult.RightShoulderOri;
    str += QString("RightShoulderOri: %1, %2, %3  \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.RightUpperArmOri ;
    str += QString("RightUpperArmOri: %1, %2, %3  \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.RightLowerArmOri ;
    str += QString("RightLowerArmOri: %1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

    str += "\nLeft Leg\n" ;
    E = _3DViewer->IKResult.LeftThighRootOri ;
    str += QString("LeftThighRootOri: %1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.LeftThighOri ;
    str += QString("LeftThighOri:     %1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.LeftShankOri ;
    str += QString("LeftShankOri:     %1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

    str += "\nRight Leg\n" ;
    E = _3DViewer->IKResult.RightThighRootOri ;
    str += QString("RightThighRootOri:%1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.RightThighOri ;
    str += QString("RightThighOri:    %1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
    E = _3DViewer->IKResult.RightShankOri ;
    str += QString("RightShankOri:    %1, %2, %3 \n").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

      /////////////
      ///////Compare Joint Position Between Target and IK Result
      /// /////////
//    str += "Raw Data of Joints :\n\n" ;
//    V = _3DViewer->IKResult.HipPosRaw ;
//    str += QString("  Hip:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.HeadPosRaw ;
//    str += QString("  Head:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.NeckPosRaw ;
//    str += QString("  Neck:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.LeftWristPosRaw ;
//    str += QString("  LeftWrist:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.RightWristPosRaw ;
//    str += QString("  RightWrist:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.LeftAnklePosRaw ;
//    str += QString("  LeftAnkle:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.RightAnklePosRaw ;
//    str += QString("  RightAnkle:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//
//    str += "Result Data of Joints :\n\n" ;
//    V = _3DViewer->IKResult.HipPos ;
//    str += QString("  Hip:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.HeadPos ;
//    str += QString("  Head:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.NeckPos ;
//    str += QString("  Neck:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.LeftWristPos ;
//    str += QString("  LeftWrist:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.RightWristPos ;
//    str += QString("  RightWrist:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.LeftAnklePos ;
//    str += QString("  LeftAnkle:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());
//    V = _3DViewer->IKResult.RightAnklePos ;
//    str += QString("  RightAnkle:\n    %1, %2, %3, \n").ArgCoordinate(V.getX()).ArgCoordinate(V.getY()).ArgCoordinate(V.getZ());

    Position pJ ;
    str += QString("\n\nNeckPos in Different Chains : \n") ;
    pJ = _3DViewer->humanIK.getFabrik()[5].getJoints()[0].getPosition()  ;
    str += QString("inChain5 : %1, %2, %3 \n").ArgCoordinate(pJ.getX()).ArgCoordinate(pJ.getY()).ArgCoordinate(pJ.getZ()) ;
    pJ = _3DViewer->humanIK.getFabrik()[4].getJoints()[0].getPosition()  ;
    str += QString("inChain4 : %1, %2, %3 \n").ArgCoordinate(pJ.getX()).ArgCoordinate(pJ.getY()).ArgCoordinate(pJ.getZ()) ;
    pJ = _3DViewer->humanIK.getFabrik()[1].getJoints()[0].getPosition()  ;
    str += QString("inChain1 : %1, %2, %3 \n").ArgCoordinate(pJ.getX()).ArgCoordinate(pJ.getY()).ArgCoordinate(pJ.getZ()) ;
    pJ = _3DViewer->humanIK.getFabrik()[0].getJoints()[0].getPosition()  ;
    str += QString("inChain0 : %1, %2, %3 \n").ArgCoordinate(pJ.getX()).ArgCoordinate(pJ.getY()).ArgCoordinate(pJ.getZ()) ;

    str += "Result Data of Joints :\n" ;
    Fabrik*f = _3DViewer->humanIK.getFabrik() + 0 ;
    Joint *j = f->getJoints() ;
    int n = f->getJointsCount() ;
    Position p0 = j[0].getPosition() / 100;
    Position p1 = j[1].getPosition() / 100;
    Position p2 = j[2].getPosition() / 100;
    Position p3 = j[3].getPosition() / 100;
    str += QString("joint0: %1, %2, %3 \n").ArgCoordinate(p0.getX()).ArgCoordinate(p0.getY()).ArgCoordinate(p0.getZ());
    str += QString("joint1: %1, %2, %3 \n").ArgCoordinate(p1.getX()).ArgCoordinate(p1.getY()).ArgCoordinate(p1.getZ());
    str += QString("joint2: %1, %2, %3 \n").ArgCoordinate(p2.getX()).ArgCoordinate(p2.getY()).ArgCoordinate(p2.getZ());
    str += QString("joint3: %1, %2, %3 \n").ArgCoordinate(p3.getX()).ArgCoordinate(p3.getY()).ArgCoordinate(p3.getZ());
    ui()->IKStatusText->setText(str);
//////////////////////////////
////  ui()->OtherStatusText
//////////////////////////////

    str.clear();
    POT **ppot ;
    if ( ui()->useCOMRadioButton->isChecked() )
    {
        ppot = POTSystem ;
    }
    else if( ui()->useUDPRadioButton->isChecked() )
    {
        ppot = UdpPOTSystem ;
    }


    str += QString("\nCurrent Pos:\n") ;
    for ( i=0 ; i<5; i++ )
    {
        str += QString("Node%1:").arg(i+1);
        V = ppot[i]->getPos() ;
        str += QString("%1 %2 %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;
    }
    str += QString("\nCurrent Quat:\n") ;
    for ( i=0 ; i<5; i++ )
    {
        str += QString("Node%1:").arg(i+1);
        Q = ppot[i]->getOri() ;
        str += QString("%1 %2 %3 %4\n").argQ(Q.getW()).argQ(Q.getX()).argQ(Q.getY()).argQ(Q.getZ()) ;
    }

    str += QString("\n\n\nOn Tpose:\n") ;
    str += QString("T-Neck:");
    str += QString("FK: %1 %2 %3\n").argP(inputAdaptor.humanFK.Pinit.Pinit_Neck.getX()).argP(inputAdaptor.humanFK.Pinit.Pinit_Neck.getY()).argP(inputAdaptor.humanFK.Pinit.Pinit_Neck.getZ()) ;
    str += QString("POT: %1 %2 %3\n").argP(_3DViewer->humanIK.Pinit.Pinit_Neck.getX()).argP(_3DViewer->humanIK.Pinit.Pinit_Neck.getY()).argP(_3DViewer->humanIK.Pinit.Pinit_Neck.getZ()) ;
    str += QString("fk_PNeck: %1 %2 %3\n").argP(inputAdaptor.resultBuffer.fkP_Neck.getX()).argP(inputAdaptor.resultBuffer.fkP_Neck.getY()).argP(inputAdaptor.resultBuffer.fkP_Neck.getZ()) ;
    str += QString("IK HIP POS: %1 %2 %3\n").argP(_3DViewer->humanIK.output().HipPos.getX()).argP(_3DViewer->humanIK.output().HipPos.getY()).argP(_3DViewer->humanIK.output().HipPos.getZ());
    str += QString("IK HIP QQQ: %1 %2 %3 %4\n").argQ(_3DViewer->humanIK.output().HipQuat.getW()).argQ(_3DViewer->humanIK.output().HipQuat.getX()).argQ(_3DViewer->humanIK.output().HipQuat.getY()).argQ(_3DViewer->humanIK.output().HipQuat.getZ());


//    str += QString("\n\n\nDEBUG LEFT ARM\n\n");
//    Q = inputAdaptor.debugLeftArm.QRoot ;
//    str += QString("QNeck: %1 %2 %3 %4\n").argQ(Q.getW()).argQ(Q.getX()).argQ(Q.getY()).argQ(Q.getZ()) ;
//    Q = inputAdaptor.debugLeftArm.QLinker;
//    str += QString("QUpArm: %1 %2 %3 %4\n").argQ(Q.getW()).argQ(Q.getX()).argQ(Q.getY()).argQ(Q.getZ()) ;
//    Q = inputAdaptor.debugLeftArm.QThis;
//    str += QString("QFoArm: %1 %2 %3 %4\n\n").argQ(Q.getW()).argQ(Q.getX()).argQ(Q.getY()).argQ(Q.getZ()) ;

//    V = inputAdaptor.debugLeftArm.PNeck ;
//    str += QString("NECK    : %1 %2 %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;

//    V = inputAdaptor.debugLeftArm.PShoulder;
//    str += QString("Shoulder: %1 %2 %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;

//    V = inputAdaptor.debugLeftArm.PElbow;
//    str += QString("Elbow   : %1 %2 %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;

//    V = inputAdaptor.debugLeftArm.PWrist ;
//    str += QString("Wrist   : %1 %2 %3\n").argP(V.getX()).argP(V.getY()).argP(V.getZ()) ;







    ui()->OtherStatusText->setText(str);
}






void IKMonitor::recordDevFile()
{
    if(StartRecord)
    {
        QByteArray data;
        QString QL_str, EA_str, RAW_str ;
        Vector3D P ;
        EulerAngle E;
        Quaternion QL, q;
        POT** pot ;
        if ( ui()->useCOMRadioButton->isChecked() )
            pot = POTSystem ;
        else
            pot = UdpPOTSystem ;

        P = pot[0]->getPos();
        q = pot[0]->getOri();
        RAW_str += QString("%1 %2 %3 ").argP(P.getX()).argP(P.getY()).argP(P.getZ());
        RAW_str += QString("%1 %2 %3 %4 ").argQ(q.getW()).argQ(q.getX()).argQ(q.getY()).argQ(q.getZ());


        P = pot[1]->getPos();
        q = pot[1]->getOri();
        RAW_str += QString("%1 %2 %3 ").argP(P.getX()).argP(P.getY()).argP(P.getZ());
        RAW_str += QString("%1 %2 %3 %4 ").argQ(q.getW()).argQ(q.getX()).argQ(q.getY()).argQ(q.getZ());


        P = pot[2]->getPos();
        q = pot[2]->getOri();
        RAW_str += QString("%1 %2 %3 ").argP(P.getX()).argP(P.getY()).argP(P.getZ());
        RAW_str += QString("%1 %2 %3 %4 ").argQ(q.getW()).argQ(q.getX()).argQ(q.getY()).argQ(q.getZ());


        P = pot[3]->getPos();
        q = pot[3]->getOri();
        RAW_str += QString("%1 %2 %3 ").argP(P.getX()).argP(P.getY()).argP(P.getZ());
        RAW_str += QString("%1 %2 %3 %4 ").argQ(q.getW()).argQ(q.getX()).argQ(q.getY()).argQ(q.getZ());


        P = pot[4]->getPos();
        q = pot[4]->getOri();
        RAW_str += QString("%1 %2 %3 ").argP(P.getX()).argP(P.getY()).argP(P.getZ());
        RAW_str += QString("%1 %2 %3 %4||").argQ(q.getW()).argQ(q.getX()).argQ(q.getY()).argQ(q.getZ());


        P = _3DViewer->humanIK.output().HipPos;
        QL_str += QString("%1 %2 %3 ").argP(P.getX()).argP(P.getY()).argP(P.getZ());
        EA_str += QString("%1 %2 %3 ").argP(P.getX()).argP(P.getY()).argP(P.getZ());

        QL = _3DViewer->humanIK.output().HipQuat;
        E = _3DViewer->humanIK.output().HipOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().RightThighQuat;
        E = _3DViewer->humanIK.output().RightThighOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().RightShankQuat;
        E = _3DViewer->humanIK.output().RightShankOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
        //Right Foot
        QL_str += QString("1 0 0 0 ");
        EA_str += QString("0 0 0 ");

        QL = _3DViewer->humanIK.output().LeftThighQuat;
        E = _3DViewer->humanIK.output().LeftThighOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().LeftShankQuat;
        E = _3DViewer->humanIK.output().LeftShankOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());
        //Left Foot   S0  S1
        EA_str += QString("0 0 0 0 0 0 0 0 0 ");
        QL_str += QString("1 0 0 0 1 0 0 0 1 0 0 0 ");

        QL = _3DViewer->humanIK.output().ChestQuat;
        E = _3DViewer->humanIK.output().ChestOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        // S3   Neck
        EA_str += QString("0 0 0 0 0 0 ");
        QL_str += QString("1 0 0 0 1 0 0 0 ");

        QL = _3DViewer->humanIK.output().HeadQuat;
        E = _3DViewer->humanIK.output().HeadOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().RightShoulderQuat;
        E = _3DViewer->humanIK.output().RightShoulderOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().RightUpperArmQuat;
        E = _3DViewer->humanIK.output().RightUpperArmOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().RightLowerArmQuat;
        E = _3DViewer->humanIK.output().RightLowerArmOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().RightHandQuat;
        E = _3DViewer->humanIK.output().RightHandOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().LeftShoulderQuat;
        E = _3DViewer->humanIK.output().LeftShoulderOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().LeftUpperArmQuat;
        E = _3DViewer->humanIK.output().LeftUpperArmOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().LeftLowerArmQuat;
        E = _3DViewer->humanIK.output().LeftLowerArmOri;
        QL_str += QString("%1 %2 %3 %4 ").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3 ").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        QL = _3DViewer->humanIK.output().LeftHandQuat;
        E = _3DViewer->humanIK.output().LeftHandOri;
        QL_str += QString("%1 %2 %3 %4||").argQ(QL.getW()).argQ(QL.getX()).argQ(QL.getY()).argQ(QL.getZ());
        EA_str += QString("%1 %2 %3||").ArgAngle(E.getYaw()).ArgAngle(E.getPitch()).ArgAngle(E.getRoll());

        EulerAngle_File.write(EA_str.toLatin1());
        QuatLocal_File.write(QL_str.toLatin1());
        RawData_File.write(RAW_str.toLatin1());

    }
}


void IKMonitor::on_TposeButton_clicked()
{
    if ( WorkMode == TEST_IK )
    {
        HumanIK &humanIK = _3DViewer->humanIK ;
        HumanIK::InitQuat Q4IK;
        HumanIK::InitPos  P4IK;
        HumanFK::InitQuat Q4FK;
        HumanFK::InitPos  P4FK;
//Set IK/FK Tpose Calibrate Flag
        humanIK.setCalibrate(true);
        inputAdaptor.setFKCalibrateFlag(true);

        POT** pot ;
        if ( ui()->useCOMRadioButton->isChecked() )
            pot = POTSystem ;
        else
            pot = UdpPOTSystem ;
        Q4IK.Qinit_LeftWrist  = Q4FK.Qinit_LeftWrist  = pot[0]->getOri();
        Q4IK.Qinit_RightWrist = Q4FK.Qinit_RightWrist = pot[1]->getOri();
        Q4IK.Qinit_LeftAnkle  = Q4FK.Qinit_LeftAnkle  = pot[2]->getOri();
        Q4IK.Qinit_RightAnkle = Q4FK.Qinit_RightAnkle = pot[3]->getOri();
        Q4IK.Qinit_Neck       = Q4FK.Qinit_Neck       = pot[4]->getOri();
        Q4FK.Qinit_LeftElbow  = pot[5]->getOri();
        Q4FK.Qinit_RightElbow = pot[6]->getOri();
        humanIK.setQinit(Q4IK);

        P4IK.Pinit_LeftWrist  = P4FK.Pinit_LeftWrist  = pot[0]->getPos();
        P4IK.Pinit_RightWrist = P4FK.Pinit_RightWrist = pot[1]->getPos();
        P4IK.Pinit_LeftAnkle  = P4FK.Pinit_LeftAnkle  = pot[2]->getPos();
        P4IK.Pinit_RightAnkle = P4FK.Pinit_RightAnkle = pot[3]->getPos();
        P4IK.Pinit_Neck       = P4FK.Pinit_Neck       = pot[4]->getPos();
        P4FK.Pinit_LeftElbow  = pot[5]->getPos();
        P4FK.Pinit_RightElbow = pot[6]->getPos();
        humanIK.setPinit(P4IK);
        inputAdaptor.setFKQinit(Q4FK);
        inputAdaptor.setFKPinit(P4FK);
        inputAdaptor.initResultBuffer(P4FK);
        //inputAdaptor.setTposeP(pot[4]->getHeadPos());
        updateHumanIKModel () ;
    }
/*  Mems Mocap
    int i ;
    for ( i=1; i<=17; i++ )
    {
        Quaternion T = mocapIMU->getSlaveNode(i)->getQraw() ;
        mocapIMU->getSlaveNode(i)->setQT(T);
    }
*/
}

void IKMonitor::on_AposeButton_clicked()
{
    int i ;
    for ( i=1; i<=17; i++ )
    {
        Quaternion A = mocapIMU->getSlaveNode(i)->getQraw() ;
        mocapIMU->getSlaveNode(i)->setQA(A);
    }
    mocapIMU->calibrate();
    this->FK_isCali = true;
}
void IKMonitor::FK_to_IK ()
{
    HumanIK::RawDataFrame rawData ;
    Vector3D offset(-2,0,0) ;
    int8_t id;

    Virtual3dCharacterViewer::Character3DBody &body = _3DViewer->FKResult ;
    HumanIK &humanIK = _3DViewer->humanIK ;


    id = 11 ; // Hip Q
    rawData.id = HumanIK::Joint_Neck ;
    rawData.Pos = body.NeckPos + offset ;
    rawData.Q = mocapIMU->getSlaveNode(id)->getQW();
    humanIK.input(rawData) ;


    id = 2 ;
    rawData.id = HumanIK::Joint_LeftWrist ;
    rawData.Pos = body.LeftWristPos + offset ;
    rawData.Q = mocapIMU->getSlaveNode(id)->getQW();
    humanIK.input(rawData) ;

    id = 7 ;
    rawData.id = HumanIK::Joint_RightWrist ;
    rawData.Pos = body.RightWristPos + offset ;
    rawData.Q = mocapIMU->getSlaveNode(id)->getQW();
    humanIK.input(rawData) ;


    id = 13 ;
    rawData.id = HumanIK::Joint_LeftAnkle ;
    rawData.Pos = body.LeftAnklePos + offset ;
    rawData.Q = mocapIMU->getSlaveNode(id)->getQW();
    humanIK.input(rawData) ;


    id = 16 ;
    rawData.id = HumanIK::Joint_RightAnkle ;
    rawData.Pos = body.RightAnklePos + offset ;
    rawData.Q = mocapIMU->getSlaveNode(id)->getQW();
    humanIK.input(rawData) ;


    id = 9 ;
    rawData.id = HumanIK::Joint_Head ;
    rawData.Pos = body.HeadPos + offset ;
    rawData.Q = mocapIMU->getSlaveNode(id)->getQW();
    humanIK.input(rawData) ;

    humanIK.compute();

    _3DViewer->updateBonesOriWithIKresult(_3DViewer->IKResult);
}

void IKMonitor::IK_TestLeftArm()
{
    int idx ;
    idx = 0 ;

    HumanIK::RawDataFrame rawData ;
    HumanIK &humanIK = _3DViewer->humanIK ;
    POT** pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem ;
    else
        pot = UdpPOTSystem ;

    idx = 5 - 1 ;
    rawData.id = HumanIK::Joint_Neck;
    rawData.Pos = pot[idx]->getPos() ;
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;


    idx = 1 - 1 ;
    rawData.id = HumanIK::Joint_LeftWrist ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    idx = 2 - 1 ;
    rawData.id = HumanIK::Joint_RightWrist ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;
    humanIK.compute( HumanIK::CALC_LEFT_ARM );
    humanIK.compute( HumanIK::CALC_RIGHT_ARM );

//// Module3 NA
    idx = 3 - 1 ;
    rawData.id = HumanIK::Joint_LeftAnkle ;
    rawData.Pos = Position(0,0,0);   //pot[idx]->getPos();
    rawData.Q = Quaternion(1,0,0,0); //pot[idx]->getOri();
    humanIK.input(rawData) ;
//// Module4 NA
    idx = 4 - 1 ;
    rawData.id = HumanIK::Joint_RightAnkle ;
    rawData.Pos = Position(0,0,0);   //pot[idx]->getPos();
    rawData.Q = Quaternion(1,0,0,0); //pot[idx]->getOri();
    humanIK.input(rawData) ;
    humanIK.getFabrik()[4].removeChild(humanIK.getFabrik() + 2);

    humanIK.compute( HumanIK::CALC_Execpt_HEAD );

    //// Set Values for NA Modules
    humanIK.LatestOutput.LeftFootOri = EulerAngle(0,0,0);
    humanIK.LatestOutput.LeftShankOri = EulerAngle(0,0,0);
    humanIK.LatestOutput.LeftThighOri = EulerAngle(0,0,0);
    humanIK.LatestOutput.LeftThighRootOri = EulerAngle(0,0,0);
    humanIK.LatestOutput.RightFootOri = EulerAngle(0,0,0);
    humanIK.LatestOutput.RightShankOri = EulerAngle(0,0,0);
    humanIK.LatestOutput.RightThighOri = EulerAngle(0,0,0);
    humanIK.LatestOutput.RightThighRootOri = EulerAngle(0,0,0);
////    humanIK.compute( HumanIK::CALC_LEFT_ARM + HumanIK::CALC_RIGHT_ARM);
    _3DViewer->updateBonesOriWithIKresult(_3DViewer->IKResult);
}

void IKMonitor::IK_TestArm()
{
    int idx ;
    idx = 0 ;

    HumanIK::RawDataFrame rawData ;
    HumanIK &humanIK = _3DViewer->humanIK ;
    POT** pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem ;
    else
        pot = UdpPOTSystem ;

//    idx = 5 - 1 ;
    idx = 5 - 1 ;
    rawData.id = HumanIK::Joint_Neck;
    rawData.Pos = pot[idx]->getPos() ;
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;


    idx = 1 - 1 ;
    rawData.id = HumanIK::Joint_LeftWrist ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    idx = 2 - 1 ;
    rawData.id = HumanIK::Joint_RightWrist ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;


// Module3 NA
    idx = 3 - 1 ;
    rawData.id = HumanIK::Joint_LeftAnkle ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;


    idx = 4 - 1 ;
    rawData.id = HumanIK::Joint_RightAnkle ;
    rawData.Pos = pot[idx]->getPos();
    rawData.Q = pot[idx]->getOri();
    humanIK.input(rawData) ;

    humanIK.getFabrik()[4].removeChild(humanIK.getFabrik() + 2);

    humanIK.compute( HumanIK::CALC_Execpt_HEAD );

//    humanIK.LatestOutput.LeftFootOri = EulerAngle(0,0,0);
//    humanIK.LatestOutput.LeftShankOri = EulerAngle(0,0,0);
//    humanIK.LatestOutput.LeftThighOri = EulerAngle(0,0,0);
//    humanIK.LatestOutput.LeftThighRootOri = EulerAngle(0,0,0);
//    humanIK.compute( HumanIK::CALC_LEFT_ARM + HumanIK::CALC_RIGHT_ARM);
    _3DViewer->updateBonesOriWithIKresult(_3DViewer->IKResult);
}


void IKMonitor::on_pushButton_Calc_clicked()
{
    Vector3D offset ;
    int station1, station2 ;
    getCaliOffset ( offset ) ;
    getCaliStationID(station1, station2);

    POT* pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem[ActivedPOTNodeID-1] ;
    else
        pot = UdpPOTSystem[ActivedPOTNodeID-1] ;

    pot->LHPOT()->calibrate(station1,CaliRaw[0]) ;
    pot->LHPOT()->calibrate(station2,CaliRaw[1]) ;



    int i, j ;
    Matrix Mat(6,4) ;
    const Matrix& Mat1 = pot->LHPOT()->getTransMat(station1) ;
    const Matrix& Mat2 = pot->LHPOT()->getTransMat(station2) ;
    for ( i=0; i<3; i++ )
    {
        for ( j=0; j<4; j++ )
        {
            Mat(i,j) = Mat1(i,j) ;
            Mat(i+3,j) = Mat2(i,j) ;
        }
    }

    Mat.save2file("TransMat.txt");
}

void IKMonitor::getCaliOffset ( Vector3D& offset )
{
    bool ok ;
    double d;

    d = ui()->CaliOffsetXLineEdit->text().toDouble(&ok) ;
    if ( ! ok )
    {
        d = 0.0 ;
        ui()->CaliOffsetXLineEdit->setText(QString("%1").arg(d));
    }
    offset.setX(d);

    d = ui()->CaliOffsetYLineEdit->text().toDouble(&ok) ;
    if ( ! ok )
    {
        d = 0.0 ;
        ui()->CaliOffsetYLineEdit->setText(QString("%1").arg(d));
    }
    offset.setY(d);

    d = ui()->CaliOffsetZLineEdit->text().toDouble(&ok) ;
    if ( ! ok )
    {
        d = 0.0 ;
        ui()->CaliOffsetZLineEdit->setText(QString("%1").arg(d));
    }
    offset.setZ(d);

}

void IKMonitor::getCaliStationID ( int& station1, int& station2 )
{
    bool ok1, ok2 ;
    station1 = ui()->Station1LineEdit->text().toInt(&ok1) ;
    station2 = ui()->Station2LineEdit->text().toInt(&ok2) ;
    if ( ! ok1 )
    {
        station1 = 1 ;
        ui()->Station1LineEdit->setText(QString("%1").arg(station1));
    }
    if ( ! ok2 )
    {
        station2 = 2 ;
        ui()->Station2LineEdit->setText(QString("%1").arg(station2));
    }
}

void IKMonitor::on_pushButton_GetA_clicked()
{
    POT* pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem[ActivedPOTNodeID-1] ;
    else
        pot = UdpPOTSystem[ActivedPOTNodeID-1] ;

    int station1, station2 ;
    getCaliStationID ( station1, station2 ) ;
    int idx1 = station1 - 1 ;
    int idx2 = station2 - 1 ;

    CaliRaw[0].A = pot->pLHPOT()->LatestRaw[idx1].Pos ;
    CaliRaw[1].A = pot->pLHPOT()->LatestRaw[idx2].Pos ;

    Vector3D result ;
    result = CaliRaw[0].A ;
    ui()->label_A_info_1->setText(QString("A1: %1  %2  %3").arg(result.getX()).arg(result.getY()).arg(result.getZ()));
    result = CaliRaw[1].A ;
    ui()->label_A_info_2->setText(QString("A2: %1  %2  %3").arg(result.getX()).arg(result.getY()).arg(result.getZ()));
}

void IKMonitor::on_pushButton_GetO_clicked()
{
    POT* pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem[ActivedPOTNodeID-1] ;
    else
        pot = UdpPOTSystem[ActivedPOTNodeID-1] ;

    int station1, station2 ;
    getCaliStationID ( station1, station2 ) ;
    int idx1 = station1 - 1 ;
    int idx2 = station2 - 1 ;

    CaliRaw[0].O = pot->pLHPOT()->LatestRaw[idx1].Pos ;
    CaliRaw[1].O = pot->pLHPOT()->LatestRaw[idx2].Pos ;

    Vector3D result;
    result = CaliRaw[0].O ;
    ui()->label_O_info_1->setText(QString("O1: %1  %2  %3").arg(result.getX()).arg(result.getY()).arg(result.getZ()));

    result = CaliRaw[1].O ;
    ui()->label_O_info_2->setText(QString("O2: %1  %2  %3").arg(result.getX()).arg(result.getY()).arg(result.getZ()));
}

void IKMonitor::on_pushButton_GetB_clicked()
{
    POT* pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem[ActivedPOTNodeID-1] ;
    else
        pot = UdpPOTSystem[ActivedPOTNodeID-1] ;

    int station1, station2 ;
    getCaliStationID ( station1, station2 ) ;
    int idx1 = station1 - 1 ;
    int idx2 = station2 - 1 ;

    CaliRaw[0].B = pot->pLHPOT()->LatestRaw[idx1].Pos ;
    CaliRaw[1].B = pot->pLHPOT()->LatestRaw[idx2].Pos ;


    Vector3D result;
    result = CaliRaw[0].B;
    ui()->label_B_info_1->setText(QString("B1: %1  %2  %3").arg(result.getX()).arg(result.getY()).arg(result.getZ()));
    result = CaliRaw[1].B;
    ui()->label_B_info_2->setText(QString("B2: %1  %2  %3").arg(result.getX()).arg(result.getY()).arg(result.getZ()));
}





void IKMonitor::setupPosScope(Oscilloscope*scope, const QString& name)
{
    scope->core()->channel(0)->setSamplesType(OscilloscopeChannel::Original_FLOAT32);
    scope->core()->channel(1)->setSamplesType(OscilloscopeChannel::Original_FLOAT32);
    scope->core()->channel(2)->setSamplesType(OscilloscopeChannel::Original_FLOAT32);

    scope->core()->channel(0)->setColor(qRgb(255,0,0));
    scope->core()->channel(1)->setColor(qRgb(0,0,255));
    scope->core()->channel(2)->setColor(qRgb(255,255,0));

    scope->core()->channel(0)->setYRange(4);
    scope->core()->channel(1)->setYRange(4);
    scope->core()->channel(2)->setYRange(4);

    scope->core()->channel(0)->setShow(true) ;
    scope->core()->channel(1)->setShow(true) ;
    scope->core()->channel(2)->setShow(true) ;

    scope->setWindowTitle(name);
    scope->setScreenTitle(name);
    scope->panel()->setChannelConfigWindowTitle((name+QString(" 通道设置")).toLatin1().data());
    scope->core()->channel(0)->setDescription((name+QString(" X轴")).toLatin1().data());
    scope->core()->channel(1)->setDescription((name+QString(" Y轴")).toLatin1().data());
    scope->core()->channel(2)->setDescription((name+QString(" Z轴")).toLatin1().data());
}

void IKMonitor::setupOriScope(Oscilloscope*scope, const QString& name)
{
    scope->core()->channel(0)->setSamplesType(OscilloscopeChannel::Original_FLOAT32);
    scope->core()->channel(1)->setSamplesType(OscilloscopeChannel::Original_FLOAT32);
    scope->core()->channel(2)->setSamplesType(OscilloscopeChannel::Original_FLOAT32);

    scope->core()->channel(0)->setColor(qRgb(255,0,0));
    scope->core()->channel(1)->setColor(qRgb(0,0,255));
    scope->core()->channel(2)->setColor(qRgb(255,255,0));

    scope->core()->channel(0)->setYRange(180);
    scope->core()->channel(1)->setYRange(90);
    scope->core()->channel(2)->setYRange(180);

    scope->core()->channel(0)->setShow(true) ;
    scope->core()->channel(1)->setShow(true) ;
    scope->core()->channel(2)->setShow(true) ;

    scope->setWindowTitle(name);
    scope->setScreenTitle(name);
    scope->panel()->setChannelConfigWindowTitle((name+QString(" 通道设置")).toLatin1().data());
    scope->core()->channel(0)->setDescription((name+QString(" Yaw")).toLatin1().data());
    scope->core()->channel(1)->setDescription((name+QString(" Pitch")).toLatin1().data());
    scope->core()->channel(2)->setDescription((name+QString(" Roll")).toLatin1().data());
}

void IKMonitor::on_POT_Calibrate_pushButton_clicked()
{
    Matrix LoadMatCombo = Matrix(6,4);
    POT* pot ;
    if ( ui()->useCOMRadioButton->isChecked() )
        pot = POTSystem[ActivedPOTNodeID-1] ;
    else
        pot = UdpPOTSystem[ActivedPOTNodeID-1] ;

    if(LoadMatCombo.LoadMatFile("TransMat.txt"))
    {
        if(pot->LoadLHTransMats( LoadMatCombo ))
        {
            TM1Avail = true;
            TM2Avail = true;

            int idx ;
            if ( ui()->useCOMRadioButton->isChecked() )
            {
                for ( idx=0; idx<POT_COUNT; idx++ )
                {
                    POTSystem[idx]->setLHTransMat(pot->getLHTransMat(1),1);
                    POTSystem[idx]->setLHTransMat(pot->getLHTransMat(2),2);
                }
            }
            else
            {

                for ( idx=0; idx<UDP_POT_NODES; idx++ )
                {
                    UdpPOTSystem[idx]->setLHTransMat(pot->getLHTransMat(1),1);
                    UdpPOTSystem[idx]->setLHTransMat(pot->getLHTransMat(2),2);
                }
            }
        }
    }

    if(!TM1Avail && !TM2Avail)
    {
        printf("CAUTIOUS! No Correct TransMat Found!\n");
    }

}

void IKMonitor::on_StartRecord_pushButton_clicked()
{
    if(StartRecord)
    {
       printf(" Please Stop Recording First\n");
        return;
    }
    else
    {
        if (!EulerAngle_File.open(QIODevice::ReadWrite ))
        {
            printf("EA.txt open Failed\n");
            return;
        }
        if (!QuatLocal_File.open(QIODevice::ReadWrite ))
        {
            printf("QL.txt open Failed\n");
            return;
        }
        if (!RawData_File.open(QIODevice::ReadWrite))
        {
            printf("raw.txt open Failed\n");
            return;
        }
        StartRecord = true;
    }
}

void IKMonitor::on_StopRecord_pushButton_clicked()
{
    if(StartRecord == false)
    {
        printf("Not Recording, Operation Invalid\n");
    }
    else
    {
        RawData_File.close();
        QuatLocal_File.close();
        EulerAngle_File.close();
    }
}

void IKMonitor::on_ConnectDefaultPOTComPortsButton_clicked()
{
    QSerialPort * port ;


    int n = 0 ;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();

        if ( info.description().contains("CH340",Qt::CaseInsensitive) )
        {
            if ( n < 7 )
            {
                POTPort[n].Port->setPortName(info.portName()) ;
                n ++ ;
            }
        }
    }

    QLabel* Labels[7] = {
        ui()->POT1PortStatusLabel,
        ui()->POT2PortStatusLabel,
        ui()->POT3PortStatusLabel,
        ui()->POT4PortStatusLabel,
        ui()->POT5PortStatusLabel,
        ui()->POT6PortStatusLabel,
        ui()->POT7PortStatusLabel
    } ;


    for ( int i=0; i<n; i++ )
    {
        port = POTPort[i].Port ;
        if ( port->isOpen() )
            port->close();

        port->setBaudRate(460800);
        if ( port->open(QIODevice::ReadWrite) )
        {
            Labels[i]->setText(QString("%1已打开").arg(port->portName()));
        }
        else
        {
            QMessageBox::warning(this,"error detected", QString("%1 open failed \n").arg(port->portName()), QMessageBox::Ok, QMessageBox::Ok) ;
            Labels[i]->setText(QString("%1已关闭").arg(port->portName()));
        }
    }
}

void IKMonitor::on_DisconnectAllPOTComPortsButton_clicked()
{
    QSerialPort * port ;
    QLabel* Labels[7] = {
        ui()->POT1PortStatusLabel,
        ui()->POT2PortStatusLabel,
        ui()->POT3PortStatusLabel,
        ui()->POT4PortStatusLabel,
        ui()->POT5PortStatusLabel,
        ui()->POT6PortStatusLabel,
        ui()->POT7PortStatusLabel
    } ;
    for ( int i=0; i<7; i++ )
    {
        port = POTPort[i].Port ;
        if ( port->isOpen() )
            port->close();

        if ( port->portName().isEmpty() )
            Labels[i]->setText(QString("串口已关闭"));
        else
            Labels[i]->setText(QString("%1已关闭").arg(port->portName()));
    }
}

void IKMonitor::on_PollPeriod_lineEdit_editingFinished()
{
    QString str = ui()->PollPeriod_lineEdit->text() ;
    bool OK ;
    int period = str.toInt(&OK) ;
    if ( ! OK || period < 1 )
    {
 //       QMessageBox (QMessageBox::Warning,QString("WARNING"),QString("slave ID must be a integer between 0 and 255 ! \n set to default value instead ."),QMessageBox::Ok,this);
        period = 10 ;
        ui()->PollPeriod_lineEdit->setText(QString("%1").arg(period));
    }

    PollTimer->stop();
    PollTimer->setInterval(period);
    PollTimer->start();
}



void IKMonitor::on_startPollCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        PollTimer->start();
    }
    else if ( state == Qt::Unchecked )
    {
        PollTimer->stop();
    }
}


void IKMonitor::onPollTimerout()
{
    if ( ui()->startPollCheckBox->isChecked() )
    {
        SerialDataPKG PKG ;
        CmdParam_GetSensor PARAM ;
        PARAM.SlaveAddr = ActivedPOTNodeID ;
        makeSerialDataPKG(&PKG,(const uint8_t*)"command",CMD_GET_SENSOR,&PARAM,sizeof(PARAM));
        if ( POTPort[0].Port->isOpen() )
        {
            POTPort[0].Port->write((const char*)&PKG, PKG.pkgLen) ;
        }
    }
}


void IKMonitor::SerialParserHandlerAdaptor ( uint16_t CmdOrReplyCode, void* param, void* _parser )
{
    SerialDataParser* parser = (SerialDataParser*)_parser ;
    IKMonitor* This = (IKMonitor*)(parser->aux) ;
    This->SerialParserHandler ( CmdOrReplyCode, param, parser) ;
}

void IKMonitor::SerialParserHandler (uint16_t CmdOrReplyCode, void* _param, SerialDataParser* parser)
{
    switch(CmdOrReplyCode)
    {
    case REPLY_SET_HUB_MODE:
        do {
            ReplyParam_SetHubMode* param = (ReplyParam_SetHubMode*)_param ;
            if ( param->hubMODE == HubMode_Normal )
            {
                ui()->CurModeLabel->setText(QString("正常模式"));
            }
            else if( param->hubMODE == HubMode_Developer )
            {
                ui()->CurModeLabel->setText(QString("开发者模式"));
            }

        } while (0) ;
        break ;

    case REPLY_GET_SENSOR:
        do {
            ReplyParam_GetSensor* param = (ReplyParam_GetSensor*)_param ;
            int POTIdx = param->SlaveAddr - 1 ;
            int i ;

            POTDataParser20 parser20 ;
            initPOTDataParser20 ( & parser20 ) ;
            for ( i=0; i<sizeof(param->SensorData); i++ )
                parser20.feedByte ( &parser20, param->SensorData[i] ) ;
            if ( parser20.outputReady )
            {
                parser20.outputReady = 0 ;
                Vector3D Pos(parser20.var.fPx, parser20.var.fPy, parser20.var.fPz) ;
                Quaternion Quat1(parser20.var.fQw, parser20.var.fQx, parser20.var.fQy, parser20.var.fQz) ;
                Quaternion Quatg(parser20.var.fQgw, parser20.var.fQgx, parser20.var.fQgy, parser20.var.fQgz) ;
                //POTSystem[POTIdx]->processRaw(Pos,Quat1,Quatg,parser20.var.LightHouseID,parser20.var.ts,parser20.var.dataValid);
                Quat1 = Quat1 * Quatg ;
            }

        } while (0) ;
        break ;

    default:
        break ;

    }
}

void IKMonitor::on_RegressRatioLineEdit_editingFinished()
{
    QString str = ui()->RegressRatioLineEdit->text() ;
    bool OK ;
    double RegressRatio = str.toDouble(&OK) ;
    if ( ! OK || RegressRatio < 0 || RegressRatio > 1 )
    {
 //       QMessageBox (QMessageBox::Warning,QString("WARNING"),QString("slave ID must be a integer between 0 and 255 ! \n set to default value instead ."),QMessageBox::Ok,this);
        RegressRatio = 0.05 ;
        ui()->RegressRatioLineEdit->setText(QString("%1").arg(RegressRatio));
    }

    for (int i=0; i<POT_COUNT; i++)
    {
        POTSystem[i]->d->GyroRegressRatio = RegressRatio ;
    }
}

void IKMonitor::on_UsingGyroCheckBox_stateChanged(int state)
{
    bool UsingGyro ;
    if ( state == Qt::Checked )
    {
        UsingGyro  =  true ;
    }
    else if ( state == Qt::Unchecked )
    {
        UsingGyro = false ;
    }

    for (int i=0; i<POT_COUNT; i++)
    {
        POTSystem[i]->d->UsingGyro = UsingGyro ;
    }
}

void IKMonitor::on_OriRecoverPushButton_clicked()
{
    for (int i=0; i<POT_COUNT; i++)
    {
        POTSystem[i]->d->FinalQuat = POTSystem[i]->LHPOT()->getOri() ;
    }
}

void IKMonitor::on_EnablePOTPosSmoothCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i]->LHFusion()->setPosSmooth(true);
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i]->LHFusion()->setPosSmooth(true);
        }
    }
    else if ( state == Qt::Unchecked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i]->LHFusion()->setPosSmooth(false);
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i]->LHFusion()->setPosSmooth(false);
        }
    }
}

void IKMonitor::on_EnablePOTOriSmoothCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i]->LHFusion()->setOriSmooth(true);
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i]->LHFusion()->setOriSmooth(true);
        }
    }
    else if ( state == Qt::Unchecked )
    {
        for (int i=0; i<POT_COUNT; i++)
        {
            POTSystem[i]->LHFusion()->setOriSmooth(false);
        }
        for (int i=0; i<UDP_POT_NODES; i++)
        {
            UdpPOTSystem[i]->LHFusion()->setOriSmooth(false);
        }
    }

}

void IKMonitor::on_TsLSBLineEdit_editingFinished()
{

    QString str = ui()->TsLSBLineEdit->text() ;
    bool OK ;
    double TsLSB = str.toDouble(&OK) ;
    if ( ! OK || TsLSB < 0  )
    {
 //       QMessageBox (QMessageBox::Warning,QString("WARNING"),QString("slave ID must be a integer between 0 and 255 ! \n set to default value instead ."),QMessageBox::Ok,this);
        TsLSB = 0.001 ;
        ui()->TsLSBLineEdit->setText(QString("%1").arg(TsLSB));
    }

    for (int i=0; i<POT_COUNT; i++)
    {
        POTSystem[i]->pLHPOT()->TS_LSB = TsLSB ;
    }
}

void IKMonitor::on_EnableAuxCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        inputAdaptor.EnableAux = true ;
    }
    else if ( state == Qt::Unchecked )
    {
        inputAdaptor.EnableAux = false ;
    }
}

void IKMonitor::on_comboBox_SelectCube_currentIndexChanged(int index)
{
    int R = (int)(_3DViewer->MarkCubeFK[index].Color.getX()*100) -1;
    int G = (int)(_3DViewer->MarkCubeFK[index].Color.getY()*100) -1;
    int B = (int)(_3DViewer->MarkCubeFK[index].Color.getZ()*100) -1;
    ui()->horizontalSlider_R->setValue(R);
    ui()->horizontalSlider_G->setValue(G);
    ui()->horizontalSlider_B->setValue(B);
}

void IKMonitor::on_horizontalSlider_R_valueChanged(int value)
{
    int id = ui()->comboBox_SelectCube->currentIndex();
    _3DViewer->MarkCubeFK[id].Color.setX((value+1.0)/100.0);
}

void IKMonitor::on_horizontalSlider_G_valueChanged(int value)
{
    int id = ui()->comboBox_SelectCube->currentIndex();
    _3DViewer->MarkCubeFK[id].Color.setY((value+1.0)/100.0);
}

void IKMonitor::on_horizontalSlider_B_valueChanged(int value)
{
    int id = ui()->comboBox_SelectCube->currentIndex();
    _3DViewer->MarkCubeFK[id].Color.setZ((value+1.0)/100.0);
}

void IKMonitor::on_NormalModeRadioButton_toggled(bool checked)
{
    if ( checked )
    {
        CurHubMode = HubMode_Normal ;

        SerialDataPKG PKG ;
        CmdParam_SetHubMode Param ;
        Param.hubMODE = CurHubMode ;
        makeSerialDataPKG(&PKG,(const uint8_t*)"command",CMD_SET_HUB_MODE,&Param,sizeof(Param)) ;
        if ( POTPort[0].Port->isOpen() )
        {
            POTPort[0].Port->write((const char*)&PKG, PKG.pkgLen) ;
        }
    }
}

void IKMonitor::on_DevModeRadioButton_toggled(bool checked)
{
    if ( checked )
    {
        CurHubMode = HubMode_Developer ;

        SerialDataPKG PKG ;
        CmdParam_SetHubMode Param ;
        Param.hubMODE = CurHubMode ;
        makeSerialDataPKG(&PKG,(const uint8_t*)"command",CMD_SET_HUB_MODE,&Param,sizeof(Param)) ;
        if ( POTPort[0].Port->isOpen() )
        {
            POTPort[0].Port->write((const char*)&PKG, PKG.pkgLen) ;
        }
    }
}

void IKMonitor::on_NotifyHubModeButton_clicked()
{
    SerialDataPKG PKG ;
    CmdParam_SetHubMode Param ;
    Param.hubMODE = CurHubMode ;
    makeSerialDataPKG(&PKG,(const uint8_t*)"command",CMD_SET_HUB_MODE,&Param,sizeof(Param)) ;
    if ( POTPort[0].Port->isOpen() )
    {
        POTPort[0].Port->write((const char*)&PKG, PKG.pkgLen) ;
    }
}


void IKMonitor::on_ToggleStationsPushButton_clicked()
{
    ui()->KeepOffStation1CheckBox->toggle();
    ui()->KeepOffStation2CheckBox->toggle();
}

void IKMonitor::on_ShowPOTNode7CheckBox_stateChanged(int state)
{
    int idx = 7 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx);
    }

}

void IKMonitor::on_ShowAllFKResultsCheckBox_stateChanged(int state)
{
    bool checked ;
    if ( state == Qt::Checked )
    {
        checked = true ;
    }
    else if ( state == Qt::Unchecked )
    {
        checked = false ;
    }

    ui()->ShowFKResult1CheckBox->setChecked(checked);
    ui()->ShowFKResult2CheckBox->setChecked(checked);
    ui()->ShowFKResult3CheckBox->setChecked(checked);

}

void IKMonitor::on_ShowAllFKResultsOffsetCheckBox_stateChanged(int state)
{
    bool checked ;
    if ( state == Qt::Checked )
    {
        checked = true ;
    }
    else if ( state == Qt::Unchecked )
    {
        checked = false ;
    }

    ui()->ShowFKResultOffset1CheckBox->setChecked(checked);
    ui()->ShowFKResultOffset2CheckBox->setChecked(checked);
    ui()->ShowFKResultOffset3CheckBox->setChecked(checked);
    ui()->ShowFKResultOffset4CheckBox->setChecked(checked);
    ui()->ShowFKResultOffset5CheckBox->setChecked(checked);
}

void IKMonitor::on_ShowFKResultOffset1CheckBox_stateChanged(int state)
{
    int idx = 1 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 1);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 1);
    }
}

void IKMonitor::on_ShowFKResultOffset2CheckBox_stateChanged(int state)
{
    int idx = 2 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 1);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 1);
    }
}
void IKMonitor::on_ShowFKResultOffset3CheckBox_stateChanged(int state)
{
    int idx = 3 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 1);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 1);
    }
}
void IKMonitor::on_ShowFKResultOffset4CheckBox_stateChanged(int state)
{
    int idx = 4 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 1);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 1);
    }
}
void IKMonitor::on_ShowFKResultOffset5CheckBox_stateChanged(int state)
{
    int idx = 5 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 1);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 1);
    }
}


void IKMonitor::on_ShowFKResult1CheckBox_stateChanged(int state)
{
    int idx = 1 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 2);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 2);
    }
}

void IKMonitor::on_ShowFKResult2CheckBox_stateChanged(int state)
{
    int idx = 2 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 2);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 2);
    }
}
void IKMonitor::on_ShowFKResult3CheckBox_stateChanged(int state)
{
    int idx = 3 - 1 ;
    if ( state == Qt::Checked )
    {
        _3DViewer->enableMarkCube(idx, 2);
    }
    else if ( state == Qt::Unchecked )
    {
        _3DViewer->disableMarkCube(idx, 2);
    }
}


void IKMonitor::on_ShowDebugArmCheckBox_stateChanged(int state)
{
    if ( state == Qt::Checked )
    {
        for (int i=0; i<4; i++)
            _3DViewer->enableMarkCube(i, 3);
    }
    else if ( state == Qt::Unchecked )
    {
        for (int i=0; i<4; i++)
            _3DViewer->disableMarkCube(i, 3);
    }
}
