#ifndef CAUSTICMAKER_H
#define CAUSTICMAKER_H

#include <Eigen/Dense>
#include <vector>
class CausticMaker
{
public:
    CausticMaker(std::vector<Eigen::Vector3f> caustic,std::vector<Eigen::Vector3f> originalImage, int rows, int cols);
    std::vector<Eigen::Vector3f> project(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    Eigen::Vector3f mapCoordinate(Eigen::Matrix3f transform, float x, float y);
    Eigen::Matrix3f basis(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

    std::vector<Eigen::Vector3f> m_caustic;
    std::vector<Eigen::Vector3f> m_image;
    int m_rows;
    int m_cols;
};

#endif // CAUSTICMAKER_H
