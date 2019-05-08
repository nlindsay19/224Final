
#include "brdfreplacement.h"
#include "math.h"
#include "assert.h"

#include "Eigen/Dense"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Eigen/SparseCholesky>
#include <Eigen/Sparse>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <QProcess>
#include <QProcessEnvironment>

using namespace Eigen;
using namespace std;
BrdfReplacement::BrdfReplacement()
{
    sampleNum = 400;

}

void BrdfReplacement::importanceSampling(int rows, int cols, std::vector<Vector3f> &directionVectors, std::vector<Vector3f> &sampledColors, std::vector<Vector3f> inpainting){
    float xC = float(cols)/2;
    float yC = float(rows)/2;
    float R = fmin(xC, yC);
    for(int i = 0; i < sampleNum; i ++){
        float x = (i % 20)/20.0f;//float(rand())/RAND_MAX;
        float y = (i / 20)/20.0f;//float(rand())/RAND_MAX;
        x = (x - 0.5f) * 2;
        y = (y - 0.5f) * 2;
        Vector3f D = Vector3f(x, y, 1.0f - sqrtf(x * x + y * y));
        if(float(rand())/RAND_MAX < 0.5f){
             D = Vector3f(x, y, -1.0f + sqrtf(x * x + y * y));
        }
        directionVectors.push_back(D.normalized());
        int u = x * R + xC;
        int v = (y * R + yC);

        us.push_back(u);
        vs.push_back(v);
        sampledColors.push_back(inpainting[v * cols + u]);
    }

    MatrixXf l_reds(m_maskArea, sampleNum);
    reds = l_reds;

    MatrixXf l_blues(m_maskArea, sampleNum);
    blues = l_blues;

    MatrixXf l_greens(m_maskArea, sampleNum);
    greens = l_greens;

}

std::vector<Vector3f> BrdfReplacement::sample(std::vector<Vector3f> inpainting, std::vector<Vector3f> mask, std::vector<Vector3f> directions, std::vector<Vector3f> normals, std::vector<Vector3f> sampledColors, int rows, int cols){
    int indexCounter = 0;
    float xC = float(cols)/2;
    float yC = float(rows)/2;

    std::vector<Vector3f> brdfReplacement;
    Vector3f color = m_diffuse;
    Vector3f specular = m_specular;
    float n = 5;

    int maskInd = 0;
    for(int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){

            if(mask[indexCounter][0] > 150){
                Vector3f lPrime = Vector3f(0,0,0);
                int sampleCount = 0;
                for(int sample = 0; sample < sampledColors.size(); sample++){
                    int objectInd = y * cols + x;
                    Vector3f sampleDir = directions[sample].normalized();
                    Vector3f objectNormal = normals[objectInd];
                    objectNormal = objectNormal.normalized();
                    if(sampleDir.dot(objectNormal) > 0){
                        sampleCount += 1;
                    }

                }
                for(int sample = 0; sample < sampledColors.size(); sample++){

                    int objectInd = y * cols + x;
                    Vector3f sampleDir = directions[sample].normalized();

                    Vector3f li = sampledColors[sample] / 255.0;

                    Vector3f V = Vector3f(x - xC, rows - y - yC, 100); // theres something goin on here
                    V = V.normalized();
                    Vector3f objectNormal = normals[objectInd];
                    objectNormal = objectNormal.normalized();

                    if(sampleDir.dot(objectNormal) > 0){
                        pixelToSampleIds[objectInd].push_back(sample);

                        float nDotL = fmin(1.0,sampleDir.dot(objectNormal));
                        Vector3f refl = (sampleDir) - 2 * (objectNormal.dot(sampleDir)) * objectNormal;
                        refl = -refl.normalized();
                        Vector3f coeff = color/(M_PI) + specular/(M_PI) * (n + 2) * pow(fmax(0, refl.dot(V)), n);
                        lPrime[0] += fmin(li[0] * coeff[0] * nDotL, 1.0);
                        lPrime[1] += fmin(li[1] * coeff[1] * nDotL, 1.0);
                        lPrime[2] += fmin(li[2] * coeff[2] * nDotL, 1.0);

                        if(m_solve){
                            float pdf = 1.0f/(M_PI * 2.0f);
                            reds(maskInd, sample) = (fmin(coeff[0] * nDotL, 1.0)/pdf)/sampleCount;
                            greens(maskInd, sample) = (fmin(coeff[1] * nDotL, 1.0)/pdf)/sampleCount;
                            blues(maskInd, sample) = (fmin(coeff[2] * nDotL, 1.0)/pdf)/sampleCount;
                        }
                    } else{
                        if(m_solve){
                            reds(maskInd, sample) = 0;
                            greens(maskInd, sample) = 0;
                            blues(maskInd, sample) = 0;
                        }
                    }
                }
                if(sampleCount > 0){
                    float pdf = 1.0f/(M_PI * 2.0f);
                    lPrime = 255 *  (lPrime/pdf) / sampleCount;
                }
                brdfReplacement.push_back(lPrime);
                maskInd += 1;
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
    m_solve = 0;
    return sample(inpainting, mask, directions, normals, sampledColors, rows, cols);
}

std::vector<Vector3f> BrdfReplacement::paintEnvMap(std::vector<Vector3f> inpainting, std::vector<Vector3f> mask, std::vector<Vector3f> normals,int rows, int cols){
    std::vector<Vector3f> directions;
    std::vector<Vector3f> sampledColors;

    for(int i = 0; i < rows; i ++){
        for(int j = 0; j < cols; j ++){
            if(mask[i * cols + j][0] > 150){
               m_maskArea += 1;
            }
        }
    }
    // calculate direction vectors
    importanceSampling(rows, cols, directions, sampledColors, inpainting);

    // store lighting coefficients
    m_solve = 1;
    sample(inpainting, mask, directions, normals, sampledColors, rows, cols);
    m_solve = 0;

    int maskInd = 0;
    // define desired lighting
    std::vector<int> changedPixels;
    for(int i = 0; i < rows; i ++){
        for(int j = 0; j < cols; j ++){
            if(mask[i * cols + j][0] > 150){
                if(i < float(rows) * 1.0f / 4){
                   changedPixels.push_back(maskInd);
                }
//                if(i > float(rows) * 1.0f / 4 && i < float(rows) * 3.0f / 4){
//                   changedPixels.push_back(maskInd);
//                }
//                if(i > float(rows) * 3.0f / 4){
//                   changedPixels.push_back(maskInd);
//                }
                maskInd += 1;
            }
        }
    }
    VectorXf desiredReds(changedPixels.size());
    VectorXf desiredGreens(changedPixels.size());
    VectorXf desiredBlues(changedPixels.size());
    int desired_num = 0;
    for(int i = 0; i < rows; i ++){
        for(int j = 0; j < cols; j ++){
            if(mask[i * cols + j][0] > 150){
                if(i < float(rows) * 1.0f / 4){
                    desiredReds(desired_num) = 255;
                    desiredGreens(desired_num) = 0;
                    desiredBlues(desired_num) = 0;
                    desired_num += 1;
                }
//                if(i > float(rows) * 1.0f / 4 && i < float(rows) * 3.0f / 4){
//                    desiredReds(desired_num) = 0;
//                    desiredGreens(desired_num) = 0;
//                    desiredBlues(desired_num) = 255;
//                    desired_num += 1;
//                }
//                if(i > float(rows) * 3.0f / 4){
//                    desiredReds(desired_num) = 0;
//                    desiredGreens(desired_num) = 255;
//                    desiredBlues(desired_num) = 0;
//                    desired_num += 1;
//                }
            }
        }
    }

    MatrixXf redToSolve(desired_num, sampleNum);
    MatrixXf greenToSolve(desired_num, sampleNum);
    MatrixXf blueToSolve(desired_num, sampleNum);
    for(int i = 0; i < changedPixels.size(); i++){
        int ind = changedPixels[i];
        for(int x = 0; x < sampleNum; x++){
            redToSolve(i, x) = reds(ind, x);
            greenToSolve(i, x) = greens(ind, x);
            blueToSolve(i, x) = blues(ind, x);
        }
    }

    VectorXf solveRed(sampleNum);
    VectorXf solveGreen(sampleNum);
    VectorXf solveBlue(sampleNum);

//    solveRed = (redToSolve.transpose() * redToSolve).ldlt().solve(redToSolve.transpose() * desiredReds);
//    solveGreen = (greenToSolve.transpose() * greenToSolve).ldlt().solve(greenToSolve.transpose() * desiredGreens);
//    solveBlue = (blueToSolve.transpose() * blueToSolve).ldlt().solve(blueToSolve.transpose() * desiredBlues);
    std::cout << "creating python environment" << std::endl;

    writeCoefficientsToFile("reds.txt", redToSolve, desired_num);
    writeCoefficientsToFile("blues.txt", redToSolve, desired_num);
    writeCoefficientsToFile("greens.txt", redToSolve, desired_num);

    writeDesiredToFile("desired_reds.txt", desiredReds, desired_num);
    writeDesiredToFile("desired_blues.txt", desiredBlues, desired_num);
    writeDesiredToFile("desired_greens.txt", desiredGreens, desired_num);

//    QProcess p;
//    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
//    env.insert("PYTHONPATH", "/Users/purvigoel/anaconda3/lib/python3.6/site-packages");
//    QStringList params;
//    params << "brdfsolver.py";
//    p.setStandardOutputFile("log.txt");
//    p.start("/Users/purvigoel/anaconda3/bin/python", params);
//    p.waitForFinished(-1);
//    std::cout << "here" << std::endl;

    getNewEnvmap("solved_reds.txt", solveRed);
    getNewEnvmap("solved_greens.txt", solveGreen);
    getNewEnvmap("solved_blues.txt", solveBlue);

    for(int i = 0 ; i < sampledColors.size(); i++){
        sampledColors[i] = Vector3f(fmax(fmin(solveRed(i), 255),0), fmax(fmin(solveGreen(i), 255),0), fmax(fmin(solveBlue(i), 255),0));
    }

    saveEnvmap(sampledColors);

    return sample(inpainting, mask, directions, normals, sampledColors, rows, cols);
}

void BrdfReplacement::getNewEnvmap(std::string filename, VectorXf &envmapChannel){
    std::ifstream infile(filename);
    std::string str;
    int linenum = 0;
    while (std::getline(infile, str))
    {
        envmapChannel(linenum) = std::stof(str);
        linenum += 1;
    }
}

void BrdfReplacement::saveEnvmap(std::vector<Vector3f> sampledColors){
    int envMapSize = sqrt(sampleNum);
    QImage imageOut(envMapSize, envMapSize, QImage::Format_RGB32);
    QRgb *imageBits = reinterpret_cast<QRgb *>(imageOut.bits());
    for(int i = 0; i < envMapSize; i++){
        for(int j = 0; j < envMapSize; j++){
            int index = i * envMapSize + j;
            Vector3f color = sampledColors[index];
            float colorR = fmax(fmin(color(0),255),0);
            float colorG = fmax(fmin(color(1), 255),0);
            float colorB = fmax(fmin(color(2), 255),0);
            QColor colorOut = QColor(int(colorR), int(colorG), int(colorB));
            imageBits[index] = colorOut.rgb();
        }
    }
    imageOut.save("images/envmap.png");

}

void BrdfReplacement::writeCoefficientsToFile(std::string filename, MatrixXf data, int dataNum){
    ofstream file;
    file.open(filename);
    for(int i = 0; i < dataNum; i++){
        for(int j = 0; j < sampleNum; j++){
              file << std::to_string(data(i,j));
              file << '\n';
        }
    }
    file.close();
}

void BrdfReplacement::writeDesiredToFile(std::string filename, VectorXf data, int dataNum){
    ofstream file;
    file.open(filename);

    for(int j = 0; j < dataNum; j++){
          file << std::to_string(data(j));
          file << '\n';
    }
    file.close();
}
