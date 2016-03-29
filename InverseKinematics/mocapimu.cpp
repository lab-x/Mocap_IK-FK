#include "InverseKinematics/mocapimu.h"

static float RtoD = 180.0 / 3.1415926 ;
static float DtoR = 3.1415926 / 180.0 ;


MocapIMU::MocapIMU(){
    //Nodes[17]
    Nodes[0]  = MemsNode(1, 2 );
    Nodes[1]  = MemsNode(2, 3 );

    Nodes[2]  = MemsNode(3, 4 );
    Nodes[3]  = MemsNode(4, 11);
    Nodes[4]  = MemsNode(5, 11);
    Nodes[5]  = MemsNode(6, 5 );
    Nodes[6]  = MemsNode(7, 6 );
    Nodes[7]  = MemsNode(8, 7 );
    Nodes[8]  = MemsNode(9, 10);
    Nodes[9]  = MemsNode(10,11);
    Nodes[10] = MemsNode(11,11);
    Nodes[11] = MemsNode(12,11);
    Nodes[12] = MemsNode(13,12);
    Nodes[13] = MemsNode(14,13);
    Nodes[14] = MemsNode(15,11);
    Nodes[15] = MemsNode(16,15);
    Nodes[16] = MemsNode(17,16);

   for(int i = 0; i<17; i++){
        QW[i] = Quaternion();
        QL[i] = Quaternion();
        EulerOut[i] = Vector3D(0, 0, 0);
    }
    HipPosition = Position(0, 0, 0);
}



void MocapIMU::calibrate(){
    int i;
    for(i =0; i<17; i++){
      /*  if(Nodes[i].getQT().getW() != Nodes[i].getQT().getW() && i!=3 && i!=4 && i!=8 && i!=9 && i!=13 && i!=16){
            Nodes[i].setCaliFlag(0);
        }
        else{*/
            Nodes[i].setCaliFlag(1);
            if(i<8){
                Nodes[i].setQinit(Nodes[i].getQT().conj());
            }
            else{
                Nodes[i].setQinit(Nodes[i].getQA().conj());
            }
            if(i == 8 ||i == 9 ||i == 10 ){
                Quaternion Zref = Quaternion(0, 0, 0, 1);
                Quaternion ZWO = Zref * Nodes[i].getQinit();
                Quaternion ZW = Nodes[i].getQT() * ZWO;
                Quaternion YW = Quaternion(0,ZW.getX(),ZW.getY(),0);
                YW.normalize();
                Vector3D SS = Vector3D(YW.getX()*0.5, (YW.getY()+1)*0.5, YW.getZ()*0.5);
                float w = SS.getLength() ;
                Vector3D S = SS.getNormalized() ;
                Vector3D Yref = Vector3D(0, 1, 0);
                Vector3D Z = Vector3D::Cross(Yref, S);
                Nodes[i].setQCali(Quaternion(w,Z.getX(),Z.getY(),Z.getZ()));
            }
            else if(i < 4 || i == 11 || i == 12){
                Quaternion invAraw = Nodes[i].getQA().conj();
                Quaternion dQ = Nodes[i].getQT() * invAraw;
                Quaternion  YW = Quaternion(0, dQ.getX(), dQ.getY(), 0);
                YW.normalize();
                Vector3D SS = Vector3D(YW.getX()*0.5, (YW.getY()+1)*0.5, YW.getZ()*0.5);
                float w = SS.getLength();
                SS.normalize();
                Vector3D Yref = Vector3D(0, 1, 0);
                Vector3D Z = Vector3D::Cross(Yref, SS);
                Nodes[i].setQCali(Quaternion(w, Z.getX(),Z.getY(),Z.getZ()));
            }
            else if(i == 4 || i == 5 ||i == 6 ||i == 7 ||i == 14 ||i == 15){
                Quaternion invTraw = Nodes[i].getQT().conj();
                Quaternion dQ = Nodes[i].getQA() * invTraw;
                Quaternion  YW = Quaternion(0, dQ.getX(), dQ.getY(), 0);
                YW.normalize();
                Vector3D SS = Vector3D(YW.getX()*0.5, (YW.getY()+1)*0.5, YW.getZ()*0.5);
                float w = SS.getLength();
                SS.normalize();
                Vector3D Yref = Vector3D(0, 1, 0);
                Vector3D Z = Vector3D::Cross(Yref, SS);
                Nodes[i].setQCali(Quaternion(w, Z.getX(),Z.getY(),Z.getZ()));
            }
            if(Nodes[i].getQCali().getW() != Nodes[i].getQCali().getW()){
                printf("QCali-->NaN \n");
                Nodes[i].setQCali(Quaternion());
                Nodes[i].setCaliFlag(0);
            }
       // }
    }
}

void MocapIMU::genQW(){
    int i;
    for(i = 0; i < 17; i++){
        Quaternion tmp0 = Nodes[i].getQraw() * Nodes[i].getQinit();
        if(i == 3 || i ==4)// || i == 9 || i == 8){
        {
            Nodes[i].setQW(Nodes[10].getQW());
        }
        /*if(i == 13){
            Nodes[i].setQW(Nodes[12].getQW());
        }
        if(i ==16){
            Nodes[i].setQW(Nodes[15].getQW());
        }*/
        else{
            Quaternion tmp1 = tmp0 * Nodes[i].getQCali();
            Quaternion tmp2 = Nodes[i].getQCali().conj() * tmp1;
            Nodes[i].setQW(Quaternion(tmp2.getW(), -1*tmp2.getX(), tmp2.getZ(),tmp2.getY()));
        }
    }
}

void MocapIMU::genQL(){
    int i;

    for(i = 0; i < 17; i++){
        int P_Index = Nodes[i].getPID()-1;
        if(Nodes[i].getCaliFlag() == 0){
            continue;
        }
        if(i == 10){
            Nodes[i].setQL(Nodes[i].getQW());
        }
        else{
            Quaternion P_QW = Nodes[P_Index].getQW();
            Nodes[i].setQL(P_QW.conj() * Nodes[i].getQW());
        }
        Nodes[i].setEuler(Nodes[i].getQL().toEuler().RadToDegree());
    }
}

bool MocapIMU::CalibrationDone(){
    for (int i = 0; i<17; i++){
        if(i == 3||i == 4||i== 3||i == 8||i == 9||i == 13||i == 16){
            continue;
        }
        if(Nodes[i].getCaliFlag() == 0){
            return false;
        }
    }
    return true;
}


void MocapIMU::compute()
{
    genQW();
    genQL();
}

