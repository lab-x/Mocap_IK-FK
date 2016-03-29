#ifndef OSCILOSCOPEMINIWINDOW
#define OSCILLOSCOPEMINIWINDOW

#include<QWidget>
#include <QTimer>
#include"ui_OscilloscopeMiniWindow.h"
#include"Oscilloscope.h"

class OscilloscopeMiniWindow : public QWidget
{
    Q_OBJECT

public:
    OscilloscopeMiniWindow ( QWidget* parent = 0, Qt::WindowFlags f = 0 ) : QWidget(parent,f) {
        ui()->setupUi(this);
        timer = new QTimer(this);
        timer->setInterval(40);
        connect(timer,SIGNAL(timeout()),this,SLOT(updateScopeWindow()));
        timer->start();
        setLabelName(0, QString("X:")) ;
        setLabelName(1, QString("Y:")) ;
        setLabelName(2, QString("Z:")) ;
        setTitleName(QString("-------")) ;
        Scope = NULL ;
    }
    Oscilloscope * scope() {return Scope;}
    void setScope(Oscilloscope *s) {
        Scope = s ;
        setTitleName(Scope->windowTitle());
        ui()->scopeGroupBox->setTitle(Scope->windowTitle());
        ui()->scopeWidget->setCore(Scope->core());
    }
    QString& titleName() {
        return TitleName ;
    }
    void setTitleName(const QString& str) {
        TitleName = str ;
    }


public:
    Ui::scopeMiniWindow* ui() {return &UI;}
    QString& labelName(int i) {
        return LabelName[i] ;
    }
    void setLabelName(int i, const QString& str) {
        LabelName[i] = str ;
    }
    void setLabelName(const QString& str1, const QString& str2, const QString& str3 ) {
        LabelName[0] = str1 ;
        LabelName[1] = str2 ;
        LabelName[2] = str3 ;
    }


public slots:
    void updateScopeWindow() ;
    void on_scopeCheckBox_stateChanged ( int state ) ;

private:

    Ui::scopeMiniWindow UI;

    Oscilloscope * Scope ;

    QTimer* timer ;

    QString LabelName[3] ;

    QString TitleName ;
};



#endif // OSCILLOSCOPEMINIWINDOW

