#ifndef CAMERAQUAT_H
#define CAMERAQUAT_H

#define M_PI 3.14159265358979

class CameraQuat
{
public:
    CameraQuat();

    Vector3f pos;
    Quaternion q;

    void apply(){
      Vector3f dir = q.applyToVector(Vector3f(0,0,-1))+pos;
      Vector3f top = q.applyToVector(Vector3f(0,1,0));
      gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, top.x, top.y, top.z);
    }
    /*ALL VALUES IN GRAD*/
    void turnLR(float fi){
      Vector3f top = q.applyToVector(Vector3f(0,1,0));
      double fi2 = (double)fi*M_PI/360.0;
      q=Quaternion(top*(float)sin(fi2), (float)cos(fi2))*q;
    }

    void turnUD(float fi){
      Vector3f right =  q.applyToVector(Vector3f(1,0,0));
      double fi2 = (double)fi*M_PI/360.0;
      q=Quaternion(right*(float)sin(fi2), (float)cos(fi2))*q;
    }

    void twirl(float fi){
      Vector3f dir =  q.applyToVector(Vector3f(0,0,-1));
      double fi2 = (double)fi*M_PI/360.0;
      q=Quaternion(dir*(float)sin(fi2), (float)cos(fi2))*q;
    }

    void turnOX(float fi){
      double fi2 = (double)fi*M_PI/360.0;
      q=Quaternion(Vector3f(1,0,0)*(float)sin(fi2), (float)cos(fi2))*q;
    }

    void turnOY(float fi){
      double fi2 = (double)fi*M_PI/360.0;
      q=Quaternion(Vector3f(0,1,0)*(float)sin(fi2), (float)cos(fi2))*q;
    }

    void turnOZ(float fi){
      double fi2 = (double)fi*M_PI/360.0;
      q=Quaternion(Vector3f(0,0,1)*(float)sin(fi2), (float)cos(fi2))*q;
    }

    void moveFB(float dist)
    {
      Vector3f mdir =  q.applyToVector(Vector3f(0,0,-1));
      mdir.normalize();
      pos += mdir*dist;
    }

    void moveLR(float dist)
    {
      Vector3f mdir =  q.applyToVector(Vector3f(1,0,0));
      mdir.normalize();
      pos += mdir*dist;
    }

    void moveUD(float dist)
    {
      Vector3f mdir =  q.applyToVector(Vector3f(0,1,0));
      mdir.normalize();
      pos += mdir*dist;
    }

};

#endif // CAMERAQUAT_H
