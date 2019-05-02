#ifndef BRDFREPLACEMENT_H
#define BRDFREPLACEMENT_H

#include "math.h"
#include "assert.h"

#include "Eigen/Dense"
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


class BrdfReplacement
{
public:
    BrdfReplacement();
    std::vector<Eigen::Vector3f> sample(std::vector<Eigen::Vector3f> inpainting, std::vector<Eigen::Vector3f> mask, std::vector<Eigen::Vector3f> directions, std::vector<Eigen::Vector3f> normals, std::vector<Eigen::Vector3f> sampledColors, int rows, int cols);
    void importanceSampling(int rows, int cols, std::vector<Eigen::Vector3f>& directionVectors, std::vector<Eigen::Vector3f>& sampledColors, std::vector<Eigen::Vector3f> inpainting);
    std::vector<Eigen::Vector3f> replaceBrdf(std::vector<Eigen::Vector3f> inpainting, std::vector<Eigen::Vector3f> mask, std::vector<Eigen::Vector3f> normals,int rows, int cols);

    Eigen::Vector3f m_diffuse;
    Eigen::Vector3f m_specular;
};

#endif // BRDFREPLACEMENT_H
