#ifndef RETEXTURE_H
#define RETEXTURE_H
#include "Eigen/Dense"
#include <vector>

using namespace Eigen;

class Retexture
{
public:
    Retexture();
    void calculate(std::vector<Vector3f> T, std::vector<Vector3f> image, std::vector<Vector3f> &result);
    void setF(float f);
    void setS(float s);

private:

    float m_f = 0.5f;
    float m_s = 0.5f;
};

#endif // RETEXTURE_H
