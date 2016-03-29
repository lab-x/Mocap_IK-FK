#include <QDebug>
#include <QMessageBox>
#include <QScrollBar>
#include "SerialDataViewerDemo.h"



// only support UTF-8 well .  the sending strings would be converted to utf8, and the programme
// supposed all received strings were in utf8 format .

SerialDataViewer::SerialDataViewer(QSerialPort * Port , bool ableToAccessPort ,
                                   QWidget * parent, Qt::WindowFlags f) : QWidget ( parent , f )
{
	UI_setup() ;

    AbleToAccessPort = ableToAccessPort ;
    port = Port ;
    if ( NULL == port )
        AbleToAccessPort = false ;

    recvTextEdit->setReadOnly(true);

    if (AbleToAccessPort)
    {
        connect(port,SIGNAL(readyRead()),this,SLOT(updateRevcString()));
        connect(port,SIGNAL(bytesWritten(qint64)),this,SLOT(setSendButtonActive(qint64)));
    }
    else
    {
        sendButton->setEnabled(false);
        sendTextEdit->setReadOnly(true);
        sendTextEdit->setEnabled(false);
        sendHexCheckBox->setEnabled(false);
    }

    updateTimer.setInterval(30);
    connect (&updateTimer,SIGNAL(timeout()),this,SLOT(showRecvData()));
    updateTimer.start();

    recvStrChanged = true ;
}

void SerialDataViewer::setSendButtonActive(qint64 NonEffectParam)
{
//    sendButton->setAcceptDrops(true);
    qDebug()<<"one frame send over" ;
    sendButton->setText(QApplication::translate("SerialDataViewer", "Send", 0));
    sendButton->setEnabled(true);
}


void SerialDataViewer::updateRevcString()
{
    if (!AbleToAccessPort)
    {
         return ;
    }

    QByteArray array ;
    array = port->readAll();
    addRecvString(array);
}


void SerialDataViewer::on_sendButton_clicked()
{
    if (!AbleToAccessPort)
    {
         return ;
    }


    if ( ! (port->isOpen()) )
        return ;


	QString str = sendTextEdit->toPlainText() ;
	QByteArray array  ;

//	QByteArray ba =  str.toLocal8Bit();				
//	QByteArray ba = str.toLatin1();
	QByteArray ba = str.toUtf8();


	if ( sendHexCheckBox->checkState() == Qt::Unchecked ) 
	{
		array += ba ;
	}

	else
	{
		const char ch[] = {'0','1','2','3','4','5','6','7','8','9',	// last index: 9
							'A','B','C','D','E','F',' ', 			// last index: 16
							'a','b','c','d','e','f'};			// last index: 22
		unsigned char curByte ;
		bool halfByte = false ;
		bool err = false ;
		int len = ba.size();
		unsigned char * data = (unsigned char *)(ba.data()) ;

		int i , j ;
		for ( i=0; i<len ; i++ )
		{
			for ( j = 0 ; j < sizeof (ch) ; j ++ )
			{
				if ( ch[j] == data[i] )				
					break ;
			}

			if ( j < sizeof (ch) )
			{
				if ( j > 16)
					j -= 7 ;	// j <=16
				if ( ch[j] == ' ' )
				{
					if ( halfByte )
					{
						array.append ( curByte ) ;		
						halfByte = false ;
					}
				}
				else
				{
					if ( halfByte )
					{
						curByte = (curByte<<4) | (j&0x0f) ;
						array.append ( curByte ) ;	
						halfByte = false ;
					}
					else
					{
						curByte = (j&0x0f) ;
						halfByte = true ;
					}
				}
			}
			else
			{
				QMessageBox::warning(this,"error detected", "invalid character found in the input data \n", QMessageBox::Ok, QMessageBox::Ok) ;
				array.clear();
				err = true ;
				break ;
			}
		}

		if ( !err && halfByte )
		{
			array.append ( curByte ) ;		
			halfByte = false ;
		}
	}

	// send data .
    if ( array.size() > 0 )
    {
        port->QSerialPort::write(array);
        sendButton->setEnabled(false);
        sendButton->setText(QApplication::translate("SerialDataViewer", "WaitingSendOver...", 0));
    }
/*

	// for test the addRecvString(const QByteArray &Array) .
	if ( array.size() != 0 )
	{
		addRecvString(array) ;
	}
*/
/*	
	// for test the addRecvString(const QString & Str) .
	if ( str.size() != 0 )
	{
		addRecvString(str) ;
	}
*/
}

void SerialDataViewer::on_clearRecvButton_clicked()
{
    recvData.clear();
    strToShow.clear();
    recvStrChanged = true;
    showRecvData () ;
}

void SerialDataViewer::on_showHexCheckBox_stateChanged(int state)
{
//	qDebug() << ( state ? "showHexCheckBoxChecked" : "showHexCheckBoxDeChecked" ) ;
    strToShow.clear();
    addRecvString ( recvData );
}
void SerialDataViewer::on_sendHexCheckBox_stateChanged(int state)
{
//	qDebug() << ( state ? "sendHexCheckBoxChecked" : "sendHexCheckBoxDeChecked" ) ;
}



void SerialDataViewer::addRecvString(const QByteArray &Array)
{
    if ( Array.isEmpty() )
        return ;

    recvStrChanged = true ;

	if ( showHexCheckBox->checkState() == Qt::Unchecked )
	{
        QByteArray array  ;
        for ( int i=0; i<Array.size(); i++ )
        {
            if ( Array.at(i) == '\\' )
            {
                array.append('\\') ;
                array.append('\\') ;
            }
            else if ( Array.at(i) == '\0' )
            {
                array.append('\\') ;
                array.append('0') ;
            }
            else if ( Array.at(i) == '\r' ) //
            {
                // append nothing.
            }
            else if ( Array.at(i) == '-' )
            {
                array.append('\\') ;
                array.append('-') ;
            }
            else if ( Array.at(i) >= 0x20 && Array.at(i) <= 0x7e ||
                      Array.at(i) == '\t' || Array.at(i) == '\n' )
            {
                array.append(Array.at(i)) ;
            }
            else
            {
                array.append('-') ;
            }
        }
        strToShow += array ;		//	the same as "str += QStirng::fromUtf8(array);" .
	}
	else
	{
		const char ch1[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
		int len = Array.size();
		const unsigned char*data = (unsigned char*)(Array.data());
		for (int i=0;i<len;i++)
		{ 
            strToShow += ch1 [ data[i]>>4 ] ;
            strToShow += ch1 [ data[i]&0x0f ] ;
            strToShow += ' ' ;
		}
	}

    if ( strToShow.size() > 10*1024 )
        strToShow = strToShow.right(5*1024);
//    recvTextEdit->setText(strToShow);
//	//recvTextEdit->moveCursor(QTextCursor::End);
//	recvTextEdit->verticalScrollBar()->setValue(recvTextEdit->verticalScrollBar()->maximumHeight());

}

void SerialDataViewer::addRecvData (const QByteArray &array)
{
    if ( lockRecvCheckBox->isChecked() )
        return ;

    recvData += array ;
    if (recvData.size() > 100*1024)
        recvData = recvData.right(50*1024) ;

    addRecvString(array);
}

void SerialDataViewer::showRecvData ()
{
    if ( isVisible() && recvStrChanged )
    {
        recvStrChanged = false ;
        recvTextEdit->setText(strToShow);
        recvTextEdit->verticalScrollBar()->setValue(recvTextEdit->verticalScrollBar()->maximumHeight());
    }
}

void SerialDataViewer::UI_setup()
{
	SerialDataViewer* serialDataViewer = this ;

    if (objectName().isEmpty())
        setObjectName(QStringLiteral("serialDataViewer"));
    resize(381, 441);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy);

    serialDataViewerLayout = new QVBoxLayout(serialDataViewer);
    serialDataViewerLayout->setObjectName(QStringLiteral("serialDataViewerLayout"));
    serialDataViewerLayout->setContentsMargins(0, 0, 0, 0);
    recvTextEdit = new QTextEdit(serialDataViewer);
    recvTextEdit->setObjectName(QStringLiteral("recvTextEdit"));
    recvTextEdit->setReadOnly(true);

    serialDataViewerLayout->addWidget(recvTextEdit);

    recvControlLayout = new QHBoxLayout();
    recvControlLayout->setObjectName(QStringLiteral("recvControlLayout"));
    clearRecvButton = new QPushButton(serialDataViewer);
    clearRecvButton->setObjectName(QStringLiteral("clearRecvButton"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(clearRecvButton->sizePolicy().hasHeightForWidth());
    clearRecvButton->setSizePolicy(sizePolicy1);
    clearRecvButton->setMinimumSize(QSize(0, 30));
    clearRecvButton->setMaximumSize(QSize(16777214, 30));

    recvControlLayout->addWidget(clearRecvButton);

    showHexCheckBox = new QCheckBox(serialDataViewer);
    showHexCheckBox->setObjectName(QStringLiteral("showHexCheckBox"));
    sizePolicy1.setHeightForWidth(showHexCheckBox->sizePolicy().hasHeightForWidth());
    showHexCheckBox->setSizePolicy(sizePolicy1);
    showHexCheckBox->setMinimumSize(QSize(0, 30));
    showHexCheckBox->setMaximumSize(QSize(16777214, 30));

    recvControlLayout->addWidget(showHexCheckBox);

    lockRecvCheckBox = new QCheckBox(serialDataViewer);
    lockRecvCheckBox->setObjectName(QStringLiteral("lockRecvCheckBox"));
    sizePolicy1.setHeightForWidth(lockRecvCheckBox->sizePolicy().hasHeightForWidth());
    lockRecvCheckBox->setSizePolicy(sizePolicy1);
    lockRecvCheckBox->setMinimumSize(QSize(0, 30));
    lockRecvCheckBox->setMaximumSize(QSize(16777214, 30));

    recvControlLayout->addWidget(lockRecvCheckBox);



    serialDataViewerLayout->addLayout(recvControlLayout);

    sendTextEdit = new QTextEdit(serialDataViewer);
    sendTextEdit->setObjectName(QStringLiteral("sendTextEdit"));
    sizePolicy1.setHeightForWidth(sendTextEdit->sizePolicy().hasHeightForWidth());
    sendTextEdit->setSizePolicy(sizePolicy1);
    sendTextEdit->setMaximumSize(QSize(16777215, 60));

    serialDataViewerLayout->addWidget(sendTextEdit);

    sendControlLayout = new QHBoxLayout();
    sendControlLayout->setObjectName(QStringLiteral("sendControlLayout"));
    sendButton = new QPushButton(serialDataViewer);
    sendButton->setObjectName(QStringLiteral("sendButton"));
    sizePolicy1.setHeightForWidth(sendButton->sizePolicy().hasHeightForWidth());
    sendButton->setSizePolicy(sizePolicy1);
    sendButton->setMinimumSize(QSize(0, 30));
    sendButton->setMaximumSize(QSize(16777214, 30));

    sendControlLayout->addWidget(sendButton);

    sendHexCheckBox = new QCheckBox(serialDataViewer);
    sendHexCheckBox->setObjectName(QStringLiteral("sendHexCheckBox"));
    sizePolicy1.setHeightForWidth(sendHexCheckBox->sizePolicy().hasHeightForWidth());
    sendHexCheckBox->setSizePolicy(sizePolicy1);
    sendHexCheckBox->setMinimumSize(QSize(0, 30));
    sendHexCheckBox->setMaximumSize(QSize(16777214, 30));

    sendControlLayout->addWidget(sendHexCheckBox);


    serialDataViewerLayout->addLayout(sendControlLayout);


    retranslateUi();

    QMetaObject::connectSlotsByName(serialDataViewer);	
}



void SerialDataViewer::retranslateUi()
{
    setWindowTitle(QApplication::translate("SerialDataViewer", "Serial data viewer", 0));
    clearRecvButton->setText(QApplication::translate("SerialDataViewer", "ClearRecv", 0));
    lockRecvCheckBox->setText(QApplication::translate("SerialDataViewer", "Lock Recv", 0));
    showHexCheckBox->setText(QApplication::translate("SerialDataViewer", "Show Hex", 0));
    sendButton->setText(QApplication::translate("SerialDataViewer", "Send", 0));
    sendHexCheckBox->setText(QApplication::translate("SerialDataViewer", "Send Hex", 0));
}
