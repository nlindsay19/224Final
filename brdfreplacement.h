#ifndef BRDFREPLACEMENT_H
#define BRDFREPLACEMENT_H

#include "math.h"
#include "assert.h"
#include <QImage>
#include "Eigen/Dense"
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseQR>
#include <Eigen/Sparse>

class BrdfReplacement
{
public:
    BrdfReplacement();
    typedef Eigen::Triplet<float> Triple;

    std::vector<Eigen::Vector3f> sample(std::vector<Eigen::Vector3f> inpainting, std::vector<Eigen::Vector3f> mask, std::vector<Eigen::Vector3f> directions, std::vector<Eigen::Vector3f> normals, std::vector<Eigen::Vector3f> sampledColors, int rows, int cols);
    void importanceSampling(int rows, int cols, std::vector<Eigen::Vector3f>& directionVectors, std::vector<Eigen::Vector3f>& sampledColors, std::vector<Eigen::Vector3f> inpainting);
    std::vector<Eigen::Vector3f> replaceBrdf(std::vector<Eigen::Vector3f> inpainting, std::vector<Eigen::Vector3f> mask, std::vector<Eigen::Vector3f> normals,int rows, int cols);

    std::vector<Eigen::Vector3f> paintEnvMap(std::vector<Eigen::Vector3f> inpainting, std::vector<Eigen::Vector3f> mask, std::vector<Eigen::Vector3f> normals,int rows, int cols);

    Eigen::Vector3f m_diffuse;
    Eigen::Vector3f m_specular;

    std::map<int, std::vector<int>> pixelToSampleIds;
    void writeCoefficientsToFile(std::string filename, Eigen::MatrixXf data, int num);
    void writeDesiredToFile(std::string filename, Eigen::VectorXf data, int dataNum);
    void saveEnvmap(std::vector<Eigen::Vector3f> sampledColors);
    void getNewEnvmap(std::string filename, Eigen::VectorXf &envmapChannel);
    std::vector<int> us;
    std::vector<int> vs;
    Eigen::MatrixXf reds;
    Eigen::MatrixXf greens;
    Eigen::MatrixXf blues;

    int m_solve = 0;
    int m_maskArea = 0;
    int sampleNum = 0;
};

#endif // BRDFREPLACEMENT_H
