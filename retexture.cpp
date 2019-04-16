#include "retexture.h"
#include "imagereader.h"
#include <algorithm>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <random>
Retexture::Retexture()
{
}

void Retexture::calculate(std::vector<Vector3f> T, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask)
{
    int width = mask.getImageWidth();
    std::cout << width << std::endl;
    float cmaxAverage = 0.0f;

    for (int i = 0; i < T.size(); i++) {
        int x = i % width;
        int y = i / width;

        if(QColor(mask.pixelAt(y,x)).red() < 150) {
            // Black part of the mask, don't do anything
            result.push_back(T[i]);
            continue;
        }

        int t_x = fmod(float(x) + m_s*deltaX[i], width);
        int t_y = fmod(float(y) + m_s*deltaY[i], mask.getImageHeight());
        Vector3f t_value = T[t_y*width + t_x];
        Vector3f resultValue = (1.f - m_f) * t_value + m_f * image[i];

        float resultAverage = (resultValue[0] + resultValue[1] + resultValue[2])/3.0f;
        if(cmaxAverage < resultAverage){
            cmaxAverage = resultAverage;
        }
        result.push_back(resultValue);

    }
    std::cout << cmaxAverage << std::endl;

    for (int i = 0; i < T.size(); i++) {
        int x = i % width;
        int y = i / width;

        if(QColor(mask.pixelAt(y,x)).red() < 150) {
            // Black part of the mask, don't do anything

            continue;
        }
        Vector3f resultValue = result[i];
        resultValue[0] = fmin(pow((resultValue[0] / cmaxAverage) , 1.8) * 255.0f,255.0f);
        resultValue[1] = fmin(pow((resultValue[1] / cmaxAverage) , 1.8) * 255.0f,255.0f);
        resultValue[2] = fmin(pow((resultValue[2] / cmaxAverage) , 1.8) * 255.0f, 255.0f);
        result[i] = resultValue;
    }

}

void Retexture::setF(float f) {
    m_f = f;
}

void Retexture::setS(float s) {
    m_s = s;
}
