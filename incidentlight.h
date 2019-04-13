#ifndef INCIDENTLIGHT_H
#define INCIDENTLIGHT_H
#include <vector>
#include "Eigen/Dense"
#include "imagereader.h"

using namespace Eigen;

class incidentlight
{
public:
    incidentlight();
    std::vector<Vector3f> inPaint(ImageReader mask);
};

#endif // INCIDENTLIGHT_H
