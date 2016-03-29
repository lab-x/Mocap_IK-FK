#include "MiniScopesClub.h"




void MiniScopesClub::on_ResetAllButton_clicked()
{
    int i ;
    for ( i=0; i<capacity(); i++ )
        if ( getMiniScope(i)->scope() )
            getMiniScope(i)->scope()->core()->resetAllChannels();
}

void MiniScopesClub::on_LockAllButton_clicked()
{
    int i ;
    for ( i=0; i<capacity(); i++ )
        if ( getMiniScope(i)->scope() )
            getMiniScope(i)->scope()->core()->lockAllChannels();
}


