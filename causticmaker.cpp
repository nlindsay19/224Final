#include "causticmaker.h"

#include <Eigen/Dense>
#include <vector>
#include <iostream>
using namespace Eigen;

CausticMaker::CausticMaker(std::vector<Vector3f> caustic,std::vector<Vector3f> originalImage, int rows, int cols)
{
    m_caustic = caustic;
    m_rows = rows;
    m_cols = cols;
    m_image = originalImage;

}

Matrix3f CausticMaker::basis(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4){
    Matrix3f source = Matrix3f::Zero();
    source(0,0) = x1;
    source(0,1) = x2;
    source(0,2) = x3;
    source(1,0) = y1;
    source(1,1) = y2;
    source(1,2) = y3;
    source(2,0) = 1.0;
    source(2,1) = 1.0;
    source(2,2) = 1.0;

    Vector3f b = Vector3f(x4, y4, 1.0f);

    Vector3f scales = source.inverse() * b;

    source(0,0) = x1 * scales[0];
    source(0,1) = x2 * scales[1];
    source(0,2) = x3 * scales[2];
    source(1,0) = y1 * scales[0];
    source(1,1) = y2 * scales[1];
    source(1,2) = y3 * scales[2];
    source(2,0) = 1.0 * scales[0];
    source(2,1) = 1.0 * scales[1];
    source(2,2) = 1.0 * scales[2];
    return source;

}

Vector3f CausticMaker::mapCoordinate(Matrix3f transform, float x, float y){
    Vector3f homogeneous = Vector3f(x, y, 1);
    homogeneous = transform * homogeneous;
    homogeneous[0] = fmin(fmax(homogeneous[0]/homogeneous[2], 0), m_cols - 1);
    homogeneous[1] = fmin(fmax(homogeneous[1]/homogeneous[2], 0), m_rows - 1);
    return homogeneous;
}

std::vector<Vector3f> CausticMaker::project(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4){
    Matrix3f A = basis(0,0,m_cols,0,m_cols,m_rows,0,m_rows);

    Matrix3f B = basis(x1,y1,x2,y2,x3,y3,x4,y4);
    Matrix3f bInv = B.inverse();

    Matrix3f mapper = A * bInv;

    std::vector<Vector3f> result;
    for(int i = 0; i < m_rows; i++){
        for(int j = 0; j < m_cols; j++){
            result.push_back(Vector3f(0,0,0));
        }
    }
    std::cout <<  fmin(x1, x4) << " " << fmax(x2, x3)<< std::endl;
    for(int i = 0; i < m_rows; i++){
        for(int j = 0; j < m_cols; j++){
            Vector3f invMap = mapCoordinate(mapper, j, i);
            int x = int(invMap[0]);
            int y = int(invMap[1]);
            if( i > fmin(y1, y2) && i < fmax(y3, y4)){
                if(j > fmin(x1, x4) && j < fmax(x2, x3)){
                    if( m_caustic[y * m_cols + x][0] > 3 ){
                        result[i * m_cols + j] = m_caustic[y * m_cols + x];
                    } else {
                        result[i * m_cols + j]  = m_image[i * m_cols + j];
                    }
                } else {
                    result[i * m_cols + j]  = m_image[i * m_cols + j];
                }
            } else {
                result[i * m_cols + j]  = m_image[i * m_cols + j];
            }

        }
    }
    return result;
}
