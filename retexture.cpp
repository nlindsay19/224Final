#include "retexture.h"
#include "imagereader.h"

Retexture::Retexture()
{
}

void Retexture::calculate(std::vector<Vector3f> T, std::vector<Vector3f> image, std::vector<float> deltaX, std::vector<float> deltaY, std::vector<Vector3f> &result, ImageReader mask)
{
    int width = mask.getImageWidth();

    for (int i = 0; i < T.size(); i++) {
        int x = i % width;
        int y = i / width;

        if(QColor(mask.pixelAt(x,y)).red() < 150) {
            // Black part of the mask, don't do anything
            result.push_back(Vector3f(0, 0, 0));
            continue;
        }

        int t_x = x + m_s*deltaX[i];
        int t_y = y + m_s*deltaY[i];
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
