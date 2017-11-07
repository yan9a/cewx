
#include <wx/wx.h>
#include <opencv2/opencv.hpp>
using namespace cv;

class Simple : public wxFrame
{
public:
    Simple(const wxString& title);

};
Simple::Simple(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
{
  Centre();
  Mat img=imread("grove.jpg",CV_LOAD_IMAGE_COLOR);
  namedWindow("Img",CV_WINDOW_AUTOSIZE);
  imshow("Img",img);
}

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)
bool MyApp::OnInit()
{
    Simple *simple = new Simple(wxT("Simple"));
    simple->Show(true);

    return true;
}
