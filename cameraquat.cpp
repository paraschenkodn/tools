#include "cameraquat.h"

CameraQuat::CameraQuat()
{
q.setVector(0.0f,0.0f,0.0f);
q.setScalar(1.0f);
pos.setX(0.0f);pos.setY(0.0f);pos.setZ(0.0f);
}
