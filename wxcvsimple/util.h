//https://stackoverflow.com/questions/2210937/opencv-with-other-gui-like-qt-or-wxwidgets-on-win32-vc
#include <wx/wx.h>
#include <opencv2/opencv.hpp>
using namespace cv;

void copy_and_swap_rb(char *s, char *d, int size) {
    // Copy image data source s to destination d, swapping R and B channels.
    // Assumes 8 bit depth, 3 interleaved channels, and width_step = width*3
    const int step = 3;
    char *end = s + size;
    while (s<end) {
        d[0] = s[2];
        d[1] = s[1];
        d[2] = s[0];
        d += step; s += step;
    }
}

void wx2cv(wxImage &wx, IplImage *ipl) {
    // Copy image data from wxWidgets image to Ipl (opencv) image
    // Assumes ipl image has seq "GBR", depth 8, and 3 channels, and
    // has the same size as the wxImage, and width_step = width*3.
    copy_and_swap_rb((char*)wx.GetData(), ipl->imageData, ipl->imageSize);
}

void cv2wx(IplImage *ipl, wxImage &wx ) {
    // Copy image data from Ipl (opencv) image to wxImage
    // Assumes ipl image has seq "GBR", depth 8, and 3 channels, and
    // has the same size as the wxImage, and width_step = width*3.
    copy_and_swap_rb( ipl->imageData, (char*)wx.GetData(),
                      wx.GetWidth()*wx.GetHeight()*3);
}

IplImage *cv_from_wx(wxImage &wx) {
    // Return a new IplImage copied from a wxImage.
    // Must be freed by user with cvReleaseImage().
    IplImage *ret = cvCreateImage(cvSize(wx.GetWidth(), wx.GetHeight()),
                                  IPL_DEPTH_8U, 3);
    wx2cv(wx, ret);
    return ret;
}

wxImage wx_from_cv( IplImage *cx) {
    // Return new wxImage copied from a compatible IplImage.
    // Assumes ipl image has seq "GBR", depth 8, and 3 channels
    // Fear not. The copy on return is cheap; does not deep-copy the data.
    wxImage wx(cx->width, cx->height, (unsigned char*) malloc(cx->imageSize), false);
    cv2wx(cx, wx);
    return wx;
}

wxImage wx_from_mat(Mat &img) {
    //https://stackoverflow.com/questions/15925084/conversion-from-iplimage-to-cvmat

    //For single channel
    //IplImage* iplimg = cvCloneImage(&(IplImage)mat); 

    //For 3 channel
    IplImage* iplimg = cvCreateImage(cvSize(img.cols, img.rows), 8, 3);
    IplImage ipltemp = img;
    cvCopy(&ipltemp, iplimg);

    return wx_from_cv(iplimg);
}

Mat mat_from_wx(wxImage &wx) {
    return cvarrToMat(cv_from_wx(wx));    
}
