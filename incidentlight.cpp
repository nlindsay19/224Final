#include "incidentlight.h"
#include <vector>

incidentlight::incidentlight()
{
}

std::vector<Vector3f> incidentlight::inPaint(ImageReader mask, std::vector<Vector3f> image)
{
    int xMin = mask.getXMin();
    int xMax = mask.getXMax();
    std::cout << xMin << " " << xMax << std::endl;
    std::vector<Vector3f> result;

    for (int i = 0; i < mask.getImageHeight(); i++) {
        for (int j = 0; j < mask.getImageWidth(); j++) {

            if(QColor(mask.pixelAt(i,j)).red() < 150) {
                Eigen::Vector3f backgroundColor = image[mask.indexAt(i,j)];
                result.push_back(backgroundColor);
                continue;
            }

            float w1 = (float(j) - float(xMin)) / (float(xMax) - float(xMin));
            float w2 = (float(xMax) - float(j)) / (float(xMax) - float(xMin));

            int x1 = 2*xMin - j;
            if(x1 < 0){
                x1 += xMin;
            }
            int x2 = 2*xMax - j;
            if(x2 < 0){
                x2 += (mask.getImageWidth() - xMax);
            }
            int index1 = i*mask.getImageWidth() + x1;
            int index2 = i*mask.getImageWidth() + x2;

            Vector3f L = w1*image[index1] + w2*image[index2];
            result.push_back(L);

        }
    }

    return result;
}
