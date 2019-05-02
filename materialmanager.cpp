#include "materialmanager.h"
#include "shapeestimator.h"
#include "incidentlight.h"
#include "brdfreplacement.h"
#include "retexture.h"
#include "histogram.h"
#include "causticmaker.h"
MaterialManager::MaterialManager()
{

}

bool MaterialManager::transformMaterial(){
    if(materialParams.makeMaterial == BRDF){
        return changeBrdf();
    }
    if(materialParams.makeMaterial == RETEXTURE){
        return retexture();
    }
    if(materialParams.makeMaterial == GLASS){
        return makeGlass();
    }
    if(materialParams.makeMaterial == CAUSTIC){
        return makeCaustic();
    }
}

bool MaterialManager::areBasicParamsValid(){
    if(materialParams.mainImageFile == ""){
        std::cout << "Please insert a main image file" << std::endl;
        return false;
    }
    if(materialParams.maskFile == ""){
        std::cout << "Please insert a mask image file" << std::endl;
        return false;
    }
    return true;
}

bool MaterialManager::areBrdfParamsValid(){
    if(materialParams.diffuse[0] < 0 || materialParams.diffuse[1] < 0 || materialParams.diffuse[2] < 0){
        std::cout << "Invalid diffuse values" << std::endl;
        return false;
    }
    if(materialParams.specular[0] < 0 || materialParams.specular[1] < 0 || materialParams.specular[2] < 0){
        std::cout << "Invalid specular values" << std::endl;
        return false;
    }
    return true;
}

bool MaterialManager::areShapeEstimationParamsValid(){
    if(materialParams.bilateralSmoothing < 0){
        std::cout << "invalid bilateral smoothing value" << std::endl;
        return false;
    }
    if(materialParams.curvature < 0){
        std::cout << "invalid curvature values" << std::endl;
        return false;
    }

    return true;
}

bool MaterialManager::areRetextureParamsValid(){
    if(materialParams.textureFile == ""){
        std::cout << "invalid texture file" << std::endl;
        return false;
    }
    if(materialParams.s < 0){
        std::cout << "roughness value invalid" << std::endl;
        return false;
    }
    return true;
}

bool MaterialManager::areGlassParametersValid(){
    if(materialParams.frosty < 0){
        std::cout << "frosted glass value is invalid" << std::endl;
        return false;
    }
    if(materialParams.darkness < 0){
        std::cout << " glass darkness value is invalid" << std::endl;
        return false;
    }
    if(materialParams.glassColor[0] < 0 || materialParams.glassColor[1] < 0 || materialParams.glassColor[2] < 0){
        std::cout << " glass color value is invalid" << std::endl;
        return false;
    }
    return true;
}

bool MaterialManager::changeBrdf(){
    if(!areBasicParamsValid()){
        return false;
    }
    ImageReader im(materialParams.mainImageFile);
    ImageReader mask(materialParams.maskFile);

    if(materialParams.backgroundFile == ""){
        // Use input image as background rather than replacing background.
        materialParams.backgroundFile = materialParams.mainImageFile;
    }
    ImageReader background(materialParams.backgroundFile);

    int cols = im.getImageWidth();
    int rows = im.getImageHeight();

    std::vector<float> depth;
    std::vector<Eigen::Vector3f> normals;
    std::vector<float> gradientX;
    std::vector<float> gradientY;

    ShapeEstimator se;

    if(!areShapeEstimationParamsValid()){
        return false;
    }

    se.m_bilateralSmoothing = materialParams.bilateralSmoothing;
    se.m_curvature = materialParams.curvature;

    se.estimateShape(im,mask, depth, normals, gradientX, gradientY);

    incidentlight incidentObj;
    std::vector<Vector3f> inpainting = incidentObj.inPaint(mask, background.toVector());
    vectorToFile(inpainting, "images/inpaint.png", rows, cols);

    BrdfReplacement br;

    if(!areBrdfParamsValid()){
        return false;
    }
    br.m_diffuse = materialParams.diffuse;
    br.m_specular = materialParams.specular;

    std::vector<Vector3f> replaced = br.replaceBrdf(inpainting, mask.toVector(), normals, rows, cols);
    vectorToFile(replaced, "images/replaceBrdf.png", rows, cols);
    return true;
}

void MaterialManager::vectorToFile(std::vector<Vector3f> data, QString filenameOut, int rows, int cols){
    QImage imageOut(cols, rows, QImage::Format_RGB32);
    QRgb *imageBits = reinterpret_cast<QRgb *>(imageOut.bits());
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int index = i * cols + j;
            Vector3f color = data[index];
            float colorR = fmin(color(0),255);
            float colorG = fmin(color(1), 255);
            float colorB = fmin(color(2), 255);
            QColor colorOut = QColor(int(colorR), int(colorG), int(colorB));
            imageBits[index] = colorOut.rgb();
        }
    }
    imageOut.save(filenameOut);
}

bool MaterialManager::retexture(){
    if(!areBasicParamsValid()){
        return false;
    }
    ImageReader im(materialParams.mainImageFile);
    ImageReader mask(materialParams.maskFile);

    if(materialParams.backgroundFile == ""){
        // Use input image as background rather than replacing background.
        materialParams.backgroundFile = materialParams.mainImageFile;
    }
    ImageReader background(materialParams.backgroundFile);

    if(!areRetextureParamsValid()){
        return false;
    }
    ImageReader texture(materialParams.textureFile);

    int cols = im.getImageWidth();
    int rows = im.getImageHeight();

    std::vector<float> depth;
    std::vector<Eigen::Vector3f> normals;
    std::vector<float> gradientX;
    std::vector<float> gradientY;

    ShapeEstimator se;

    if(!areShapeEstimationParamsValid()){
        return false;
    }

    se.m_bilateralSmoothing = materialParams.bilateralSmoothing;
    se.m_curvature = materialParams.curvature;

    se.estimateShape(im,mask, depth, normals, gradientX, gradientY);

    incidentlight incidentObj;
    std::vector<Vector3f> inpainting = incidentObj.inPaint(mask, background.toVector());
    vectorToFile(inpainting, "images/inpaint.png", rows, cols);

    Retexture retextureObj;
    retextureObj.m_s = materialParams.s;

    std::vector<Vector3f> retexturing;
    retextureObj.calculateTexture(texture.toVector(), inpainting, im.toVector(), gradientX, gradientY, retexturing, mask );
    vectorToFile(retexturing, "images/replaceTexture.png", rows, cols);
    return true;
}

bool MaterialManager::makeGlass(){
    if(!areBasicParamsValid()){
        return false;
    }
    ImageReader im(materialParams.mainImageFile);
    ImageReader mask(materialParams.maskFile);

    if(materialParams.backgroundFile == ""){
        // Use input image as background rather than replacing background.
        materialParams.backgroundFile = materialParams.mainImageFile;
    }
    ImageReader background(materialParams.backgroundFile);

    int cols = im.getImageWidth();
    int rows = im.getImageHeight();

    std::vector<float> depth;
    std::vector<Eigen::Vector3f> normals;
    std::vector<float> gradientX;
    std::vector<float> gradientY;

    ShapeEstimator se;

    if(!areShapeEstimationParamsValid()){
        return false;
    }

    se.m_bilateralSmoothing = materialParams.bilateralSmoothing;
    se.m_curvature = materialParams.curvature;

    se.estimateShape(im,mask, depth, normals, gradientX, gradientY);
    incidentlight incidentObj;
    std::vector<Vector3f> inpainting = incidentObj.inPaint(mask, background.toVector());
    vectorToFile(inpainting, "images/inpaint.png", rows, cols);

    Retexture retextureObj;

    if(!areGlassParametersValid()){
        return false;
    }

    retextureObj.m_frosty = materialParams.frosty;
    retextureObj.m_s = materialParams.s;
    retextureObj.m_glassColor = materialParams.glassColor;
    retextureObj.m_darkness = materialParams.darkness;

    std::vector<Vector3f> blurred = retextureObj.applyGaussianFilter(inpainting, cols, rows, retextureObj.m_frosty);
    vectorToFile(blurred, "images/inpaintBlur.png", rows, cols);

    std::vector<Vector3f> retexturing;
    retextureObj.calculate(blurred, inpainting, im.toVector(), gradientX, gradientY, retexturing, mask);

    Histogram hist(se.getLuminances());
    std::vector<int> highlights = hist.findHighlights();
    std::vector<Vector3f> originalImage = im.toVector();

    for (int i = 0; i < highlights.size(); i++) {
        int index = highlights[i];
        Vector3f originalVal = originalImage[index];
        int gray = (int(originalVal[0]) + int(originalVal[1]) + int(originalVal[2]))/3;
        retexturing[index] = Vector3f(gray, gray, gray);
    }
    vectorToFile(retexturing, "images/glass.png", rows, cols);
    return true;
}

bool MaterialManager::makeCaustic(){
    // caustic
    if(!areBasicParamsValid()){
        return false;
    }
    ImageReader im(materialParams.mainImageFile);
    ImageReader mask(materialParams.maskFile);

    if(materialParams.backgroundFile == ""){
        // Use input image as background rather than replacing background.
        materialParams.backgroundFile = materialParams.mainImageFile;
    }
    ImageReader background(materialParams.backgroundFile);

    int cols = im.getImageWidth();
    int rows = im.getImageHeight();

    std::vector<float> depth;
    std::vector<Eigen::Vector3f> normals;
    std::vector<float> gradientX;
    std::vector<float> gradientY;

    ShapeEstimator se;

    if(!areShapeEstimationParamsValid()){
        return false;
    }

    se.m_bilateralSmoothing = materialParams.bilateralSmoothing;
    se.m_curvature = materialParams.curvature;

    se.estimateShape(im,mask, depth, normals, gradientX, gradientY);
    incidentlight incidentObj;
    std::vector<Vector3f> inpainting = incidentObj.inPaint(mask, background.toVector());
    vectorToFile(inpainting, "images/inpaint.png", rows, cols);

    Retexture retextureObj;

    if(!areGlassParametersValid()){
        return false;
    }

    retextureObj.m_frosty = materialParams.frosty;
    retextureObj.m_s = materialParams.s;

    std::vector<Vector3f> blurred = retextureObj.applyGaussianFilter(inpainting, cols, rows, retextureObj.m_frosty);
    vectorToFile(blurred, "images/inpaintBlur.png", rows, cols);

    std::vector<Vector3f> retexturing;
    retextureObj.calculate(blurred, inpainting, im.toVector(), gradientX, gradientY, retexturing, mask);

    Histogram hist(se.getLuminances());
    std::vector<int> highlights = hist.findHighlights();
    std::vector<Vector3f> originalImage = im.toVector();

    for (int i = 0; i < highlights.size(); i++) {
        int index = highlights[i];
        Vector3f originalVal = originalImage[index];
        int gray = (int(originalVal[0]) + int(originalVal[1]) + int(originalVal[2]))/3;
        retexturing[index] = Vector3f(gray, gray, gray);
    }
    vectorToFile(retexturing, "images/glass.png", rows, cols);

    ImageReader c("images/c_project.png");

    CausticMaker cm(c.toVector(), retexturing, rows, cols);
    std::vector<Vector3f> caustic = cm.project(0,0,100,0,255,255,0,100);

    vectorToFile(caustic, "images/glassWithCaustic.png", rows, cols);
    return true;
}
