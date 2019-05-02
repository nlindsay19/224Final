#include "brdfreplacement.h"
#include "math.h"
#include "assert.h"

#include "Eigen/Dense"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace Eigen;

BrdfReplacement::BrdfReplacement()
{

}

void BrdfReplacement::importanceSampling(int rows, int cols, std::vector<Vector3f> &directionVectors, std::vector<Vector3f> &sampledColors, std::vector<Vector3f> inpainting){
    float xC = float(cols)/2;
    float yC = float(rows)/2;
    float R = fmin(xC, yC);
    for(int i = 0; i < 400; i ++){
        float x = float(rand())/RAND_MAX;
        float y = float(rand())/RAND_MAX;
        x = (x - 0.5) * 2;
        y = (y - 0.5) * 2;
        Vector3f D = Vector3f(x, y, 1 - sqrtf(x * x + y * y));
        if(float(rand())/RAND_MAX < 0.5){
             D = Vector3f(x, y, -1 + sqrtf(x * x + y * y));
        }
        directionVectors.push_back(D.normalized());
        int u = x * R + xC;
        int v = y * R + yC;
        sampledColors.push_back(inpainting[v * cols + u]);

    }
}

std::vector<Vector3f> BrdfReplacement::sample(std::vector<Vector3f> inpainting, std::vector<Vector3f> mask, std::vector<Vector3f> directions, std::vector<Vector3f> normals, std::vector<Vector3f> sampledColors, int rows, int cols){
    int indexCounter = 0;
    float xC = float(cols)/2;
    float yC = float(rows)/2;

    std::vector<Vector3f> brdfReplacement;
    Vector3f color = m_diffuse;
    Vector3f specular = m_specular;
    float n = 5;
    for(int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){
            if(mask[indexCounter][0] > 100){
                Vector3f lPrime = Vector3f(0,0,0);
                int sampleCount = 0;
                for(int sample = 0; sample < sampledColors.size(); sample++){

                    int objectInd = y * cols + x;
                    Vector3f sampleDir = directions[sample].normalized();
                    Vector3f li = sampledColors[sample] / 255.0;

                    Vector3f V = Vector3f(x - xC, rows - y - yC, 100); // theres something goin on here
                    V = V.normalized();
                    Vector3f objectNormal = normals[objectInd];
                    objectNormal = objectNormal.normalized();

                    if(sampleDir.dot(objectNormal) > 0){
                        //rotate d, v over alpha, axis
                        float nDotL = fmin(1.0,sampleDir.dot(objectNormal));
                        Vector3f refl = (sampleDir) - 2 * (objectNormal.dot(sampleDir)) * objectNormal;
                        refl = -refl.normalized();
                        Vector3f coeff = color/(M_PI) + specular/(M_PI) * (n + 2) * pow(fmax(0, refl.dot(V)), n);
                        lPrime[0] += fmin(li[0] * coeff[0] * nDotL, 1.0);
                        lPrime[1] += fmin(li[1] * coeff[1] * nDotL, 1.0);
                        lPrime[2] += fmin(li[2] * coeff[2] * nDotL, 1.0);

                        sampleCount += 1;
                    }
                }
                if(sampleCount > 0){
                    float pdf = 1.0f/(M_PI * 2.0f);
                    lPrime = 255 *  (lPrime/pdf) / sampleCount;
                }
                brdfReplacement.push_back(lPrime);

            } else {
                brdfReplacement.push_back(inpainting[indexCounter]);
            }
            indexCounter += 1;
        }
    }
    std::cout << "replaced brdf" << std::endl;
    return brdfReplacement;
}

std::vector<Vector3f> BrdfReplacement::replaceBrdf(std::vector<Vector3f> inpainting, std::vector<Vector3f> mask, std::vector<Vector3f> normals,int rows, int cols){
    std::vector<Vector3f> directions;
    std::vector<Vector3f> sampledColors;
    importanceSampling(rows, cols, directions, sampledColors, inpainting);
    return sample(inpainting, mask, directions, normals, sampledColors, rows, cols);
}
