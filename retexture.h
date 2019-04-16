#ifndef RETEXTURE_H
#define RETEXTURE_H
#include "Eigen/Dense"
#include <vector>
#include "imagereader.h"

using namespace Eigen;

class Retexture
{
public:
    Retexture();
    void calculate(std::vector<Vector3f> T, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask);
    void setF(float f);
    void setS(float s);

private:

    float m_f = 0.0f;

    float m_s = 50.0f;


};

#endif // RETEXTURE_H
