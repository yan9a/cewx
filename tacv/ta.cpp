#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
using namespace cv;
int main()
{
    char *otxt;
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // initialize tesseract-ocr with English
    if(api->Init(NULL,"eng")){
        fprintf(stderr,"Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with OpenCV
    Mat img = imread("./engim.jpg",1);
    if(!img.data){
        fprintf(stderr,"Could not read input image.\n");
        exit(1);
    }

    // set image
    api->SetImage(img.data,img.cols,img.rows,3,img.step);

    // Get OCR result
    otxt = api->GetUTF8Text();
    printf("OCR output: \n%s\n",otxt);

    // destroy used obj and release memory
    api->End();
    delete api;
    delete [] otxt;

    return 0;
}
