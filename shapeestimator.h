#ifndef SHAPEESTIMATOR_H
#define SHAPEESTIMATOR_H
#include "imagereader.h"
#include "bilateralfilter.h"
#include "Eigen/Dense"

class ShapeEstimator
{
public:
    ShapeEstimator();

    void sigmoidalCompression(std::vector<float> &pixelLuminances, float sigma);
    std::vector<float> computePixelLuminance(ImageReader imageIn, ImageReader mask, float &sigma);
    QImage estimateShape(ImageReader imageIn, ImageReader mask, std::vector<float>& depthMap);

    float gradientReshape(float x);
    float gradientReshapeRecursive(float in, int itr);
    std::vector<Eigen::Vector3f> gradientField(ImageReader imageIn, std::vector<float> &pixelLuminances);
    void sigmoidalInversion(std::vector<float> &pixelLuminances, float sigma);

    void cropMask(ImageReader mask, std::vector<float> &pixelLuminances);
};

#endif // SHAPEESTIMATOR_H
