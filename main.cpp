#include <QApplication>

#include<QDebug>

#include "math.h"
#include "IKMonitor.h"
#include "Virtual3dCharacterViewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Matrix Mat = Matrix(6,4);
    Mat.LoadMatFile("TransMat.txt");
    Mat.printMat();
    IKMonitor *w = new IKMonitor(NULL);
//    Virtual3dCharacterViewer* w = new Virtual3dCharacterViewer(NULL,false,false) ;
    w->show();
    return app.exec();
}

