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

void Retexture::calculate(std::vector<Vector3f> T, std::vector<Vector3f> background, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask)
{
    int width = mask.getImageWidth();
    std::cout << width << std::endl;
    float cmaxAverage = 0.0f;

    for (int i = 0; i < T.size(); i++) {
        int x = i % width;
        int y = i / width;

        if(QColor(mask.pixelAt(y,x)).red() < 150) {
            // Black part of the mask, don't do anything
            result.push_back(background[i]);
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

void Retexture::calculateMixedMaterial(std::vector<Vector3f> glass,std::vector<Vector3f> notGlass, std::vector<Vector3f> background, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask, ImageReader materialMask, ImageReader glassColors)
{
    int width = mask.getImageWidth();
    std::cout << width << std::endl;
    float cmaxAverage = 0.0f;
    std::vector<Vector3f> T = glass;
    for (int i = 0; i < T.size(); i++) {
        int x = i % width;
        int y = i / width;

        if(QColor(mask.pixelAt(y,x)).red() < 150) {
            // Black part of the mask, don't do anything

            // if image isn't 0, push back image.
                result.push_back(background[i]);

            continue;
        }

        if(QColor(materialMask.pixelAt(y,x)).red() < 50 && QColor(materialMask.pixelAt(y,x)).blue() < 50 && QColor(materialMask.pixelAt(y,x)).green() < 50){
           T = notGlass;
           m_s = 20.0f;
        } else {
            T = glass;
            m_s = 50.0f;
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

    T = glass;
    for (int i = 0; i < T.size(); i++) {
        int x = i % width;
        int y = i / width;

        if(QColor(mask.pixelAt(y,x)).red() < 150) {
            // Black part of the mask, don't do anything
            continue;
        }
        if(QColor(materialMask.pixelAt(y,x)).red() < 50 && QColor(materialMask.pixelAt(y,x)).blue() < 50 && QColor(materialMask.pixelAt(y,x)).green() < 50) {
            // Black part of the mask, don't do anything
            continue;
        }
        QColor stainedGlass = QColor(glassColors.pixelAt(y,x));

        Vector3f darkness = Vector3f(2.0f,2.0f,2.0f);
      //  Vector3f color = Vector3f(float(stainedGlass.red())/255.0f,float(stainedGlass.green())/255.0f,float(stainedGlass.blue())/255.0f);
        Vector3f color = Vector3f(1.0f,1.0f,1.0f);
        Vector3f resultValue = result[i];
        resultValue[0] = fmin(pow((resultValue[0] * color[0]/ cmaxAverage) , darkness[0]) * 255.0f,255.0f);
        resultValue[1] = fmin(pow((resultValue[1] * color[1]/ cmaxAverage) , darkness[1]) * 255.0f,255.0f);
        resultValue[2] = fmin(pow((resultValue[2] * color[2]/ cmaxAverage) , darkness[2]) * 255.0f, 255.0f);
        result[i] = resultValue;
    }

}

std::vector<Vector3f> Retexture::applyGaussianFilter(std::vector<Vector3f> inpainted, int width, int height, int frosty)
{

      std::vector<Vector3f> temp_image (width * height);
      std::fill(temp_image.begin(), temp_image.end(), Vector3f(0, 0, 0));
      double total_weight;
      double val;

      if (!frosty) {
          // Not doin it
          return inpainted;
      }

      // Create the kernel

      double sigma = 5.0;
      double s = sigma * 3;
      double kernel[(int) ((s * 2) + 1)];


      double coefficient = 1 / (sigma * sqrt(2 * M_PI));
      double exponent;
      double x;
      double gaussian;
      for (int i = 0; i < (2 * sigma * 3) + 1; ++i)
      {
        x = fabs(s - i);
        exponent = (-1 * x * x) / (2 * sigma * sigma);
        gaussian = coefficient * pow(M_E, exponent);
        kernel[i] = gaussian;
      }


      // Changing bounds so don't have to deal with edges now
      for (int i = 0; i < width; i++) {
        for (int j = 0;  j < height; j++) {

          val = 0;
          total_weight = 0;

          int index1D = width*j + i;

          for (int dx = -s; dx <= s; dx++) {
              if (dx+i >= 0 && dx+i < width) {
                  int origIndex = width * j + (i + dx);
                  temp_image[index1D] += (inpainted[origIndex] * kernel[(int) (dx+s)]);
                  total_weight += kernel[(int) (dx+s)];
              }
          }

          temp_image[index1D] = temp_image[index1D] / total_weight;
          //std::cout << temp_image[index1D] << std::endl;

        }
      }

      std::vector<Vector3f> temp_image2 (width * height);
      std::fill(temp_image2.begin(), temp_image2.end(), Vector3f(0, 0, 0));

      // Changing bounds so don't have to deal with edges now
      for (int i = 0; i < width; i++) {
        for (int j = 0;  j < height; j++) {

          val = 0;
          total_weight = 0;
          int index1D = width*j + i;

          for (int dy = -s;  dy <= s; dy++) {
              if (dy+j >= 0 && dy+j < height) {
                  int origIndex = width*(j+dy) + i;
                  temp_image2[index1D] += (temp_image[origIndex] * kernel[(int) (dy+s)]);
                  total_weight += kernel[(int) (dy+s)];
              }
          }

          temp_image2[index1D] = (temp_image2[index1D] / total_weight);//Vector3f(Vector3f(0.7, 0.4, 0.4).array() * (temp_image2[index1D] / total_weight).array());
        }
      }


    return temp_image2;
}


void Retexture::setF(float f) {
    m_f = f;
}

void Retexture::setS(float s) {
    m_s = s;
}
