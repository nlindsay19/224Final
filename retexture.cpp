#include "retexture.h"
#include "imagereader.h"

Retexture::Retexture()
{
}

void Retexture::calculate(std::vector<Vector3f> T, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask)
{
    int width = mask.getImageWidth();
    std::cout << width << std::endl;
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
        Vector3f result_value = (1.f - m_f) * t_value + m_f * image[i];
        result.push_back(result_value);
    }

}

void Retexture::setF(float f) {
    m_f = f;
}

void Retexture::setS(float s) {
    m_s = s;
}
