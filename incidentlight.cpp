#include "incidentlight.h"
#include <vector>

incidentlight::incidentlight()
{
}

std::vector<Vector3f> incidentlight::inPaint(ImageReader mask, std::vector<Vector3f> image)
{
    int xMin = mask.getXMin();
    int xMax = mask.getXMax();

    std::vector<Vector3f> result (mask.getImageHeight()*mask.getImageWidth());

    for (int i = 0; i < mask.getImageHeight(); i++) {
        for (int j = 0; j < mask.getImageHeight(); j++) {

            if(QColor(mask.pixelAt(j,i)).red() < 150) {
                result.push_back(Vector3f(0, 0, 0));
                continue;
            }

            float w1 = (j - xMin) / (xMax - xMin);
            float w2 = (xMax - j) / (xMax - xMin);

            int x1 = 2*xMin - j;
            int x2 = 2*xMax - j;
            int index1 = i*mask.getImageWidth() + x1;
            int index2 = i*mask.getImageWidth() + x2;

            Vector3f L = w1*image[index1] + w2*image[index2];
            result.push_back(L);

        }
    }

    return result;
}
