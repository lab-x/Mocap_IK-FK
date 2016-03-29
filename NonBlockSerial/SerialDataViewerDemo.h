#ifndef SerialDataViewerDemo__h
#define SerialDataViewerDemo__h


#include <QObject>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QString>
#include <QByteArray>
#include <QTimer>

#include <QSerialPort>


class SerialDataViewer: public QWidget
{
	Q_OBJECT
public:
    SerialDataViewer(QSerialPort * port , bool ableToAccessPort=true , QWidget * parent = 0, Qt::WindowFlags f = 0) ;
        
    
    
private:
    QSerialPort * port ;
    bool AbleToAccessPort ;

public slots:
    void updateRevcString () ;
    void setSendButtonActive(qint64 NonEffectParam);
    
public slots:
	void on_clearRecvButton_clicked();
	void on_sendButton_clicked();
	void on_showHexCheckBox_stateChanged(int state) ;
	void on_sendHexCheckBox_stateChanged(int state) ;


public:
    void addRecvData (const QByteArray &array);
public slots:
    void showRecvData ();
private:
    void addRecvString(const QByteArray &array);


// UI depend
private:
    QVBoxLayout *serialDataViewerLayout;
    QTextEdit *recvTextEdit;
    QHBoxLayout *recvControlLayout;
    QPushButton *clearRecvButton;
    QCheckBox *showHexCheckBox;
    QCheckBox *lockRecvCheckBox ;
    QTextEdit *sendTextEdit;
    QHBoxLayout *sendControlLayout;
    QPushButton *sendButton;
    QCheckBox *sendHexCheckBox;
    QByteArray recvData ;
    QString strToShow ;
    QTimer updateTimer ;

    bool recvStrChanged ;

    void UI_setup();
    void retranslateUi();
};


#endif
