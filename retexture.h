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
    void calculate(std::vector<Vector3f> T, std::vector<Vector3f> background, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask);
    std::vector<Vector3f> applyGaussianFilter(std::vector<Vector3f> inpainted, int width, int height, int frosty);
    void calculateMixedMaterial(std::vector<Vector3f> glass,std::vector<Vector3f> notGlass, std::vector<Vector3f> background, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask, ImageReader materialMask,ImageReader glassColors);
    void setF(float f);
    void setS(float s);

private:

    float m_f = 0.0f;

    float m_s = 50.0f; //glass:50, material:20

    int m_frosty = 5;


};

#endif // RETEXTURE_H
