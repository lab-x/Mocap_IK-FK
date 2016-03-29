#ifndef _OSCILLOSCOPE_H_
#define _OSCILLOSCOPE_H_

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include <QTimer>
#include <QString>

#include "OscilloscopeChannel.h"
#include "OscilloscopeCore.h"
#include "OscilloscopeScreen.h"
#include "OscilloscopePanel.h"


class Oscilloscope : public QWidget
{
    Q_OBJECT

public :
    Oscilloscope(OscilloscopeCore *core=NULL, bool functionWindowVisible = false ,QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~Oscilloscope();

    OscilloscopeCore * core() {return Core;}
    OscilloscopeScreen* screen() {return Screen;}
    OscilloscopePanel * panel() {return Panel;}

    void setScreenTitle(QString str){ScreenTitle=str;scopeSceenParent->setTitle(ScreenTitle);}
    QString screenTitle(){return ScreenTitle;}

    void setFuntionWindowVisible(bool visible)
    {
        FunctionWindowVisible = visible ;
        int height = minScopeScreenParentHeight + getFuncWidgetsParentHeight() + 3*WidgetsGap ;
        if ( Panel->height() > height )
            height = Panel->height() ;
        int width = Panel->width() + minScopeScreenParentWidth + 2*WidgetsGap ;
        setMinimumSize(width,height);
    }
    bool funtionWindowVisible(){return FunctionWindowVisible ;}


private:

    OscilloscopeCore * Core ;
    bool CoreIsLocal ;

    OscilloscopeScreen *Screen ;
    OscilloscopePanel * Panel ;

    QString ScreenTitle ;

    // UI related .

public:

    QWidget *scopeConfigWidgetsParent;
    QGroupBox *scopeSceenParent;
    QGroupBox *functionWidgetsParent;



protected:
    void resizeEvent(QResizeEvent*event);
    void closeEvent(QCloseEvent *event) ;

private:
    QTimer * ScanTimer ;
    void uiSetup(bool functionWindowVisible);

    bool FunctionWindowVisible ;

private:    // UI size
    static const int WidgetsGap = 10 ;

    static const int minScopeScreenParentWidth = 670 ;
    static const int minScopeScreenParentHeight = 510 ;

    static const int functionWidgetsParentWidth = minScopeScreenParentWidth ;
    static const int functionWidgetsParentHeight = 150 ;


    int getFuncWidgetsParentWidth()
    {
        if ( FunctionWindowVisible )
            return functionWidgetsParentWidth ;
        else
            return 0 ;
    }

    int getFuncWidgetsParentHeight()
    {
        if ( FunctionWindowVisible )
            return functionWidgetsParentHeight ;
        else
            return 0 ;
    }

};


#endif
