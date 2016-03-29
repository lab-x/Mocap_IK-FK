#ifndef __IK_MONITOR_H__
#define __IK_MONITOR_H__



#include <QWidget>
#include <QTimer>
#include <QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QFile>

#include "ui_IKTestWindow.h"
#include "stdint.h"
#include "Virtual3dCharacterViewer.h"
#include "SerialConfigWindowDemo.h"
#include "SerialDataViewerDemo.h"
#include "InverseKinematics/mocapimu.h"

#include "Oscilloscope/Oscilloscope.h"
#include "Oscilloscope/MiniScopesClub.h"

#include "POT/POT.h"
#include "POT/POTPrivate.h"
#include "POT/LightHousePOT.h"
#include "POT/LightHousePOTPrivate.h"
#include "POT/LightHouseFusion.h"
#include "POT/LightHouseFusionPrivate.h"

//#define WholeBody
#define BothArm
extern "C"
{
    #include "MathToolsC/cQuaternion.h"
    #include "SerialProtocol.h"
}


class IKMonitor : public QWidget
{
    Q_OBJECT

private:
    enum {
        TEST_IK,
        TEST_POT
    } WorkMode ;
    void IK_TestArm();
    void IK_TestLeftArm();

    void updateHumanIKModel ();

    QFile QuatLocal_File;
    QFile EulerAngle_File;
    QFile RawData_File;

    bool StartRecord;
    void recordDevFile() ;

public:
    IKMonitor(QWidget *parent = 0) ;
    ~IKMonitor();
    Ui::IKTestWindow * ui()  {return &UI;}

protected:
    void resizeEvent(QResizeEvent * event) ;

public slots:
    void processMocapComPortData() ;
    void processPOTComPortData() ;
    void processPOTUdpPortData() ;


    void updateStatusPanel() ;
    void on_showMocapComDataCheckBox_stateChanged(int state);
    void on_showOscilloscopeCheckBox_stateChanged(int state);
    void on_MocapPortConifgButton_clicked();


    void on_ShowIKCheckBox_stateChanged(int state);
    void on_ShowFKCheckBox_stateChanged(int state);
    void on_ShowAllPOTNodesCheckBox_stateChanged(int state);
    void on_ShowPOTNode1CheckBox_stateChanged(int state);
    void on_ShowPOTNode2CheckBox_stateChanged(int state);
    void on_ShowPOTNode3CheckBox_stateChanged(int state);
    void on_ShowPOTNode4CheckBox_stateChanged(int state);
    void on_ShowPOTNode5CheckBox_stateChanged(int state);
    void on_ShowPOTNode6CheckBox_stateChanged(int state);

    void on_POT1PortConifgButton_clicked();
    void on_POT2PortConifgButton_clicked();
    void on_POT3PortConifgButton_clicked();
    void on_POT4PortConifgButton_clicked();
    void on_POT5PortConifgButton_clicked();
    void on_POT6PortConifgButton_clicked();

    void on_showPOT1ComDataCheckBox_stateChanged(int state);
    void on_showPOT2ComDataCheckBox_stateChanged(int state);
    void on_showPOT3ComDataCheckBox_stateChanged(int state);
    void on_showPOT4ComDataCheckBox_stateChanged(int state);
    void on_showPOT5ComDataCheckBox_stateChanged(int state);
    void on_showPOT6ComDataCheckBox_stateChanged(int state);


    void on_EnableTcpSendCheckBox_stateChanged(int state);
    void on_EnablePOTOriFusionCheckBox_stateChanged(int state);
    void on_EnablePOTPosFusionCheckBox_stateChanged(int state);
    void on_EnablePOTPosSmoothCheckBox_stateChanged(int state);
    void on_TposeButton_clicked();

    void on_showUdpPort1DataCheckBox_stateChanged(int state);
    void on_showUdpPort2DataCheckBox_stateChanged(int state);
    void on_showUdpPort3DataCheckBox_stateChanged(int state);
    void on_showUdpPort4DataCheckBox_stateChanged(int state);
    void on_showUdpPort5DataCheckBox_stateChanged(int state);
    void on_showUdpPort6DataCheckBox_stateChanged(int state);

    void on_activedPOTNodeLineEdit_editingFinished() ;
    void on_ConfidenceDampingTimeLineEdit_editingFinished() ;
    void on_OffsetDampingTimeLineEdit_editingFinished() ;
    void on_POTSmoothRatioLineEdit_editingFinished() ;

    void changeShowUdpPortDataCheckBoxState(int idx, int state);

    void on_useCOMRadioButton_toggled(bool checked) ;
    void on_useUDPRadioButton_toggled(bool checked) ;
    void on_testIKRadioButton_toggled(bool checked) ;
    void on_testPOTRadioButton_toggled(bool checked) ;


void on_AposeButton_clicked();
    void on_resetSightButton_clicked();
    void on_resetRobotButton_clicked();
    void prepareUpdateStatusPanel();
    void prepareUpdateFK2IK();

private slots:
    void newListen();
    void acceptConnection();
    void displayError(QAbstractSocket::SocketError);
    void updateFPS();
    void onTcpSend();
    void onPollTimerout() ;

    void on_pushButton_Calc_clicked();

    void on_pushButton_GetA_clicked();

    void on_pushButton_GetO_clicked();

    void on_pushButton_GetB_clicked();

    void on_POT_Calibrate_pushButton_clicked();

    void on_StartRecord_pushButton_clicked();

    void on_StopRecord_pushButton_clicked();

    void on_ConnectDefaultPOTComPortsButton_clicked();

    void on_DisconnectAllPOTComPortsButton_clicked();

    void on_PollPeriod_lineEdit_editingFinished();

    void on_startPollCheckBox_stateChanged(int arg1);

    void on_RegressRatioLineEdit_editingFinished();

    void on_UsingGyroCheckBox_stateChanged(int arg1);

    void on_OriRecoverPushButton_clicked();

    void on_EnablePOTOriSmoothCheckBox_stateChanged(int arg1);

    void on_TsLSBLineEdit_editingFinished();

    void on_EnableAuxCheckBox_stateChanged(int arg1);

    void on_comboBox_SelectCube_currentIndexChanged(int index);

    void on_horizontalSlider_R_valueChanged(int value);

    void on_horizontalSlider_G_valueChanged(int value);

    void on_horizontalSlider_B_valueChanged(int value);

    void on_showPOT7ComDataCheckBox_stateChanged(int arg1);

    void on_POT7PortConifgButton_clicked();

    void on_NormalModeRadioButton_toggled(bool checked);

    void on_DevModeRadioButton_toggled(bool checked);

    void on_NotifyHubModeButton_clicked();


    void on_ToggleStationsPushButton_clicked();

    void on_ShowPOTNode7CheckBox_stateChanged(int arg1);

    void on_ShowAllFKResultsCheckBox_stateChanged(int arg1);

    void on_ShowAllFKResultsOffsetCheckBox_stateChanged(int arg1);

    void on_ShowFKResultOffset1CheckBox_stateChanged(int arg1);
    void on_ShowFKResultOffset2CheckBox_stateChanged(int arg1);
    void on_ShowFKResultOffset3CheckBox_stateChanged(int arg1);
    void on_ShowFKResultOffset4CheckBox_stateChanged(int arg1);
    void on_ShowFKResultOffset5CheckBox_stateChanged(int arg1);


    void on_ShowFKResult1CheckBox_stateChanged(int arg1);
    void on_ShowFKResult2CheckBox_stateChanged(int arg1);
    void on_ShowFKResult3CheckBox_stateChanged(int arg1);

    void on_ShowDebugArmCheckBox_stateChanged(int arg1);

private :

    InputAdaptor inputAdaptor;
    Ui::IKTestWindow UI ;
    Virtual3dCharacterViewer* _3DViewer ;
    MocapIMU* mocapIMU ;

    QTimer *StatusUpdateTimer ;
    bool StatusContentChanged ;
    QTimer *FK2IKUpdateTimer ;
    bool FK2IKReady ;

    QTimer* FPSupdateTimer ;

    QSerialPort *MocapComPort ;
    SerialConfigWindow* MocapPortConfigWindow ;
    SerialDataViewer* MocapPortDataViewer ;
    QByteArray *MocapComData ;
    void PickupMocapDataFromComDataPkg () ;
    struct {
        uint32_t FrameCount ;
        uint32_t preFrameCount ;
        uint32_t FPS ;
    } MocapDataInfo;



    bool TM1Avail;
    bool TM2Avail;

    bool BadDataChecked ( int nodeID, Quaternion q, Quaternion qg ) ;
    bool DataRejected (uint8_t lhValid , int stationID) ;

    void PickupPOTDataFromComDataPkg (int idx);
    QTime POTTime ;


#define POT_COUNT 7
    POT* POTSystem[POT_COUNT];
    struct ComPortSuit {
        QSerialPort *Port ;
        SerialConfigWindow* ConfigWindow ;
        SerialDataViewer* DataViewer ;
        QByteArray *Data ;
    } POTPort[POT_COUNT];

    struct {
        uint8_t NodeID ;
        uint8_t dataValid ;
        uint8_t KeyStatus ;
        int StationID ;
        //uint32_t SensorMask ;
        uint32_t ts ;
        uint32_t FrameCount ;
        uint32_t preFrameCount ;
        uint32_t FPS ;
        bool AUXFLAG;
    } POTDataInfo[POT_COUNT];


    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    QTimer* TcpSendTimer ;
    bool TcpContentChanged ;
    bool TcpSend;
    void TcpSendPkg() ;

    void TcpSendTestPkg() ;
    bool FK_isCali;
    void FK_to_IK ();


    void setEulerFK ( int8_t id, EulerAngle Euler )
    {
        EulerAngle *e ;
        switch ( id )
        {
        case 1:     e = & (_3DViewer->FKResult.LeftHandOri) ; break ;
        case 2:     e = & (_3DViewer->FKResult.LeftLowerArmOri) ; break ;
        case 3:     e = & (_3DViewer->FKResult.LeftUpperArmOri) ; break ;
        case 4:     e = & (_3DViewer->FKResult.LeftShoulderOri) ; break ;
        case 5:     e = & (_3DViewer->FKResult.RightShoulderOri) ; break ;
        case 6:     e = & (_3DViewer->FKResult.RightUpperArmOri) ; break ;
        case 7:     e = & (_3DViewer->FKResult.RightLowerArmOri) ; break ;
        case 8:     e = & (_3DViewer->FKResult.RightHandOri) ; break ;
        case 9:     e = & (_3DViewer->FKResult.HeadOri) ; break ;
        case 10:     e = & (_3DViewer->FKResult.ChestOri) ; break ;
        case 11:     e = & (_3DViewer->FKResult.HipOri) ; break ;
        case 12:     e = & (_3DViewer->FKResult.LeftThighOri) ; break ;
        case 13:     e = & (_3DViewer->FKResult.LeftShankOri) ; break ;
        case 14:     e = & (_3DViewer->FKResult.LeftFootOri) ; break ;
        case 15:     e = & (_3DViewer->FKResult.RightThighOri) ; break ;
        case 16:     e = & (_3DViewer->FKResult.RightShankOri) ; break ;
        case 17:     e = & (_3DViewer->FKResult.RightFootOri) ; break ;

//        case 1:     e = & (_3DViewer->FKResult.LeftHandOri) ; break ;
//        case 2:     e = & (_3DViewer->FKResult.LeftLowerArmOri) ; break ;
//        case 3:     e = & (_3DViewer->FKResult.LeftUpperArmOri) ; break ;
//        case 4:     e = & (_3DViewer->FKResult.RightUpperArmOri) ; break ;
//        case 5:     e = & (_3DViewer->FKResult.RightLowerArmOri) ; break ;
//        case 6:     e = & (_3DViewer->FKResult.RightHandOri) ; break ;
//        case 7:     e = & (_3DViewer->FKResult.HipOri) ; break ;
//        case 8:     e = & (_3DViewer->FKResult.LeftThighOri) ; break ;
//        case 9:     e = & (_3DViewer->FKResult.LeftShankOri) ; break ;
//        case 10:     e = & (_3DViewer->FKResult.RightThighOri) ; break ;
//        case 11:     e = & (_3DViewer->FKResult.RightShankOri) ; break ;


        default:    e = NULL ; break ;
        }

        *e = Euler ;
    }
    static const double PI = 3.1415926 ;



    //////////  UDP ports //////////
#define UDP_POT_NODES 6
//#define UDP_POT_NODES 4
    int ActivedPOTNodeID ;
    static uint16_t UdpPortsNo[UDP_POT_NODES] ;
    POT* UdpPOTSystem[UDP_POT_NODES];
    struct UdpPortSuit {
        SerialDataViewer* DataViewer ;
        QByteArray *Data ;
        QUdpSocket*socket ;
    } UdpPOTPort[UDP_POT_NODES];
    struct {
        uint8_t NodeID ;
        uint8_t dataValid ;
        uint8_t KeyStatus ;
        int StationID ;
        //uint32_t SensorMask ;
        uint32_t ts ;
        uint32_t FrameCount ;
        uint32_t preFrameCount ;
        uint32_t FPS ;
        bool AUXFLAG;
    } UdpPOTDataInfo[UDP_POT_NODES];
    void PickupPOTDataFromUdpDataPkg(int idx);


    uint8_t NodeKeyStatus [ UDP_POT_NODES > POT_COUNT ? UDP_POT_NODES : POT_COUNT ] ;

    /************************************************************************/
    /************************************************************************/
    /**** Osilloscopes related code start  */

        Oscilloscope* FusedPosScope ;
        Oscilloscope* MappedPosScope ;
        Oscilloscope* Station1_MappedPosScope ;
        Oscilloscope* Station2_MappedPosScope ;
        Oscilloscope* FusedOriScope ;
        Oscilloscope* MappedOriScope ;
        Oscilloscope* Station1_MappedOriScope ;
        Oscilloscope* Station2_MappedOriScope ;

        void setupPosScope(Oscilloscope*scope, const QString& name);
        void setupOriScope(Oscilloscope*scope, const QString& name);

        void addSampleToOscilloscopes(POT* pot)
        {
            uint32_t TS = pot->pLHPOT()->LatestTimeStamp ;
            Vector3D P ;
            EulerAngle e ;


            P = pot->LHPOT()->getPos() ;
            FusedPosScope->core()->channel(0)->addSample(P.getX(), TS) ;
            FusedPosScope->core()->channel(1)->addSample(P.getY(), TS) ;
            FusedPosScope->core()->channel(2)->addSample(P.getZ(), TS) ;


            P = pot->pLHPOT()->LatestMappedP ;
            MappedPosScope->core()->channel(0)->addSample(P.getX(), TS) ;
            MappedPosScope->core()->channel(1)->addSample(P.getY(), TS) ;
            MappedPosScope->core()->channel(2)->addSample(P.getZ(), TS) ;


            e = pot->LHPOT()->getOri().toEuler().RadToDegree() ;
            FusedOriScope->core()->channel(0)->addSample(e.getYaw()  , TS) ;
            FusedOriScope->core()->channel(1)->addSample(e.getPitch(), TS) ;
            FusedOriScope->core()->channel(2)->addSample(e.getRoll() , TS) ;


            e = pot->pLHPOT()->LatestMappedQ.toEuler().RadToDegree() ;
            MappedOriScope->core()->channel(0)->addSample(e.getYaw()  , TS) ;
            MappedOriScope->core()->channel(1)->addSample(e.getPitch(), TS) ;
            MappedOriScope->core()->channel(2)->addSample(e.getRoll() , TS) ;


            P = pot->pLHPOT()->LatestMappedP ;
            e = pot->pLHPOT()->LatestMappedQ.toEuler().RadToDegree() ;
            int stationID = pot->pLHPOT()->LatestStationID ;
            if ( stationID == 1 )
            {
                Station1_MappedPosScope->core()->channel(0)->addSample(P.getX(), TS) ;
                Station1_MappedPosScope->core()->channel(1)->addSample(P.getY(), TS) ;
                Station1_MappedPosScope->core()->channel(2)->addSample(P.getZ(), TS) ;

                Station1_MappedOriScope->core()->channel(0)->addSample(e.getYaw()  , TS) ;
                Station1_MappedOriScope->core()->channel(1)->addSample(e.getPitch(), TS) ;
                Station1_MappedOriScope->core()->channel(2)->addSample(e.getRoll() , TS) ;
            }

            else if ( stationID == 2 )
            {
                Station2_MappedPosScope->core()->channel(0)->addSample(P.getX(), TS) ;
                Station2_MappedPosScope->core()->channel(1)->addSample(P.getY(), TS) ;
                Station2_MappedPosScope->core()->channel(2)->addSample(P.getZ(), TS) ;

                Station2_MappedOriScope->core()->channel(0)->addSample(e.getYaw(), TS) ;
                Station2_MappedOriScope->core()->channel(1)->addSample(e.getPitch(), TS) ;
                Station2_MappedOriScope->core()->channel(2)->addSample(e.getRoll(), TS) ;
            }
        }

        // deploy the Oscilloscopes and setup .
        MiniScopesClub* ScopesClub ;
        void deployOscilloscopes() {

            ScopesClub = new MiniScopesClub() ;
            MiniScopesClub* club = ScopesClub ;

            FusedPosScope = new Oscilloscope();
            setupPosScope(FusedPosScope,QString("融合后位置"));
            club->getMiniScope(5)->setScope(FusedPosScope);
            club->getMiniScope(5)->setLabelName(QString("X:"),QString("Y:"),QString("Z:")) ;

            MappedPosScope = new Oscilloscope();
            setupPosScope(MappedPosScope,QString("映射后位置"));
            club->getMiniScope(2)->setScope(MappedPosScope);
            club->getMiniScope(2)->setLabelName(QString("X:"),QString("Y:"),QString("Z:")) ;


            Station1_MappedPosScope = new Oscilloscope();
            setupPosScope(Station1_MappedPosScope,QString("1号基站映射后位置"));
            club->getMiniScope(0)->setScope(Station1_MappedPosScope);
            club->getMiniScope(0)->setLabelName(QString("X:"),QString("Y:"),QString("Z:")) ;


            Station2_MappedPosScope = new Oscilloscope();
            setupPosScope(Station2_MappedPosScope,QString("2号基站映射后位置"));
            club->getMiniScope(3)->setScope(Station2_MappedPosScope);
            club->getMiniScope(3)->setLabelName(QString("X:"),QString("Y:"),QString("Z:")) ;



            FusedOriScope = new Oscilloscope();
            setupOriScope(FusedOriScope,QString("融合后姿态"));
            club->getMiniScope(7)->setScope(FusedOriScope);
            club->getMiniScope(7)->setLabelName(QString("Y:"),QString("P:"),QString("R:")) ;


            MappedOriScope = new Oscilloscope();
            setupOriScope(MappedOriScope,QString("映射后姿态"));
            club->getMiniScope(6)->setScope(MappedOriScope);
            club->getMiniScope(6)->setLabelName(QString("Y:"),QString("P:"),QString("R:")) ;


            Station1_MappedOriScope = new Oscilloscope();
            setupOriScope(Station1_MappedOriScope,QString("1号基站映射后姿态"));
            club->getMiniScope(1)->setScope(Station1_MappedOriScope);
            club->getMiniScope(1)->setLabelName(QString("Y:"),QString("P:"),QString("R:")) ;


            Station2_MappedOriScope = new Oscilloscope();
            setupOriScope(Station2_MappedOriScope,QString("2号基站映射后姿态"));
            club->getMiniScope(4)->setScope(Station2_MappedOriScope);
            club->getMiniScope(4)->setLabelName(QString("Y:"),QString("P:"),QString("R:")) ;

        }

        void undeployOscilloscopes() {
            // delete Club first
            delete ScopesClub ;

            // then delete Oscilloscopes
            delete FusedPosScope ;
            delete MappedPosScope ;
            delete Station1_MappedPosScope ;
            delete Station2_MappedPosScope ;
            delete FusedOriScope ;
            delete MappedOriScope ;
            delete Station1_MappedOriScope ;
            delete Station2_MappedOriScope ;
        }

    /**** Osilloscopes related code end  */
    /************************************************************************/
    /************************************************************************/

private:

        QTimer* PollTimer ;
        SerialDataParser SerialParser[POT_COUNT] ;
        void SerialParserHandler (uint16_t CmdOrReplyCode, void* param, SerialDataParser* parser) ;
        static void SerialParserHandlerAdaptor ( uint16_t CmdOrReplyCode, void* param, void* parser ) ;


        int CurHubMode ;

        LightHousePOT::CaliInput CaliRaw[2] ;
        void getCaliStationID ( int& station1, int& station2 ) ;
        void getCaliOffset ( Vector3D& offset ) ;
        int IKSrc;
} ;


#endif

