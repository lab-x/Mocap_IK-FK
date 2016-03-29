
#include "math.h"

#include "MathToolsC/cQuaternion.h"

// 四元数直乘
cQuaternion* cMultQuat(cQuaternion* __left, cQuaternion* __right,cQuaternion*result )
{ 
              cQuaternion _left = *__left ;
              cQuaternion _right = *__right ;
              cQuaternion* left = &_left ;
              cQuaternion* right = &_right ;
  
              cFloat d1, d2, d3, d4;
  
              d1 =  left->w * right->w; 
              d2 = -left->x * right->x; 
              d3 = -left->y * right->y; 
              d4 = -left->z * right->z; 
              result->w = d1+ d2+ d3+ d4; 
  
              d1 =  left->w * right->x; 
              d2 =  right->w * left->x; 
              d3 =  left->y * right->z; 
              d4 = -left->z * right->y; 
              result->x =  d1+ d2+ d3+ d4; 
  
              d1 =  left->w * right->y; 
              d2 =  right->w * left->y; 
              d3 =  left->z * right->x; 
              d4 = -left->x * right->z; 
              result->y =  d1+ d2+ d3+ d4; 
  
              d1 =  left->w * right->z; 
              d2 =  right->w * left->z; 
              d3 =  left->x * right->y; 
              d4 = -left->y * right->x; 
              result->z =  d1+ d2+ d3+ d4; 
              

              return result ;
} 
  
cQuaternion* cConjQuat(cQuaternion* q, cQuaternion* qConj)
{
    qConj->w = q->w ;
    qConj->x = - q->x ;
    qConj->y = - q->y ;
    qConj->z = - q->z ;
    return qConj ;
}

cQuaternion* cNormalizeQuat (cQuaternion* q, cQuaternion* qNorm)
{
    cFloat norm = 0.0 ;
    cFloat normInv ;
    int i ;
    for ( i=0; i<4; i++ )
        norm += (q->v[i]) * (q->v[i]) ;
    norm = cSqrt (norm) ;
    normInv = 1.0 / norm ;

    for ( i=0; i<4; i++ )
        qNorm->v[i] = q->v[i] * normInv ;
    return qNorm ;
}

cVector3D* cRotVectorByQuat ( cVector3D *v, cQuaternion*q, cVector3D *vRot)
{
    cQuaternion V, VRot, Q, Qinv ;
    cVectorToQuat(v,&V);
    Q = *q ;
    cConjQuat(&Q,&Qinv) ;

    cMultQuat(&Q,&V,&V) ;
    cMultQuat(&V,&Qinv,&V) ;

    *vRot = V.V ;
    return vRot ;
}



//// 构造旋转四元数
cQuaternion* cMakeRotQuat(cFloat radian, cFloat AxisX, cFloat AxisY, cFloat AxisZ,cQuaternion*result)
{ 
              cFloat norm;
              cFloat ccc, sss;
              
              result->w = result->x = result->y = result->z = 0.0f; 
  
              norm = AxisX *  AxisX +  AxisY *  AxisY +  AxisZ *  AxisZ; 
              if(norm <= 0.0f) 
                return ; 
  
              norm = 1.0f / cSqrt(norm);
              AxisX *= norm; 
              AxisY *= norm; 
              AxisZ *= norm; 
  
              ccc = cos(0.5f * radian); 
              sss = sin(0.5f * radian); 
  
              result->w = ccc; 
              result->x = sss * AxisX; 
              result->y = sss * AxisY; 
              result->z = sss * AxisZ; 
  
              return result;
} 
  
//// 构造向量四元数
cQuaternion* cXYZToQuat(cFloat PosX, cFloat PosY, cFloat PosZ,cQuaternion*result)
{ 
              result->w = 0.0f; 
              result->x = PosX; 
              result->y = PosY; 
              result->z = PosZ;   
              return result ;
} 
  

cQuaternion* cVectorToQuat (cVector3D *v, cQuaternion*result )
{
    result->W = 0.0f;
    result->V = *v;
    return result ;
}

// 四元数与欧拉角的互相转换。
// 使用下面两个函数时，须按照头文件对欧拉角三个角的约定来定义欧拉角和四元数。



// 欧拉角转四元数

// 根据欧拉转动计算四元数的变换如下:
// q = qyaw qpitch qroll
// 其中
// qroll = [cos(y/2), (sin(y/2), 0, 0)];
// qpitch = [cos(q/2), (0, sin(q/2), 0)];
// qyaw = [cos(f/2), (0, 0, sin(f/2)];
cQuaternion* cEulerToQuat( cFloat roll, cFloat pitch, cFloat yaw, cQuaternion* result)
{
 cFloat cr, cp, cy, sr, sp, sy, cpcy, spsy;
 
 //计算求四元数时使用到的所有三角值
 cr = cos(roll / 2);
 cp = cos(pitch / 2);
 cy = cos(yaw / 2);

 sr = sin(roll/2);
 sp = sin(pitch/2);
 sy = sin(yaw/2);
 cpcy = cp * cy;
 spsy = sp * sy;

 //组合这些值,生成四元数的向量和w
 result->w = cr*cpcy + sr*spsy;
 result->x = sr*cpcy - cr*spsy;
 result->y = cr*sp*cy + sr*cp*sy;
 result->z = cr*cp*sy - sr*sp*cy;

 return result ;
}




// 四元数转欧拉角

cEulerAngle* cQuatToEuler ( cQuaternion*quat , cEulerAngle* angle )
{
#define PIOver2 (3.1415926535897932384626433832795f/2.0f)   
#define PI 3.1415926535897932384626433832795f

   cFloat w = quat->w ;
   cFloat x = quat->x ;
   cFloat y = quat->y ;
   cFloat z = quat->z ;

   cFloat test = w*y - z*x ;
   if (test > 0.4999999f)
   {
    angle->yaw = 2.0f * atan2(z, w);
    if ( angle->yaw < -PI )  angle->yaw += 2.0*PI ;
    else if ( angle->yaw > PI )  angle->yaw -= 2.0*PI ;
    angle->pitch = PIOver2;
    angle->roll = 0.0f;
	return ;
   }
   if (test < -0.4999999f)
   {
    angle->yaw = 2.0f * atan2(z, w);
    if ( angle->yaw < -PI )  angle->yaw += 2.0f*PI ;
    else if ( angle->yaw > PI )  angle->yaw -= 2.0f*PI ;
    angle->pitch = -PIOver2;
    angle->roll = 0.0f;
	return ;
   }
   cFloat sqx = x * x;
   cFloat sqy = y * y;
   cFloat sqz = z * z;

#define CLAMP(x , min , max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : x))
   angle->yaw = atan2(2.0f * w * z + 2.0f * x * y , 1.0f - 2.0f * sqy - 2.0f * sqz);
   angle->pitch = asin( CLAMP(2.0f * test,-1.0f,1.0f));
   angle->roll = atan2(2.0f * w * x  + 2.0f * y * z, 1.0f - 2.0f * sqx - 2.0f * sqy);

   return angle ;
}

