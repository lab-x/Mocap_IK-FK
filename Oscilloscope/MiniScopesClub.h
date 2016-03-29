#ifndef MINISCOPESCLUB
#define MINISCOPESCLUB


//#include "Oscilloscope/ui_MiniScopesClub.h"
#include "ui_MiniScopesClub.h"


#define MiniScopesClubCapacity 8
class MiniScopesClub : public QWidget
{
    Q_OBJECT

public:
    MiniScopesClub () {
        ui()->setupUi(this);
        ScopeWindows[0] = ui()->scopeWidget1 ;
        ScopeWindows[1] = ui()->scopeWidget2 ;
        ScopeWindows[2] = ui()->scopeWidget3 ;
        ScopeWindows[3] = ui()->scopeWidget4 ;
        ScopeWindows[4] = ui()->scopeWidget5 ;
        ScopeWindows[5] = ui()->scopeWidget6 ;
        ScopeWindows[6] = ui()->scopeWidget7 ;
        ScopeWindows[7] = ui()->scopeWidget8 ;
    }
    Ui::miniScopesContainer* ui(){return &UI;}
    int capacity() { return MiniScopesClubCapacity; }
    OscilloscopeMiniWindow* getMiniScope(int i) {
        if ( i > capacity() )
            return NULL ;
        return ScopeWindows[i] ;
    }

public slots:
    void on_ResetAllButton_clicked();
    void on_LockAllButton_clicked();

private:
    Ui::miniScopesContainer UI ;
    OscilloscopeMiniWindow* ScopeWindows[MiniScopesClubCapacity] ;
};







#endif // MINISCOPESCLUB

