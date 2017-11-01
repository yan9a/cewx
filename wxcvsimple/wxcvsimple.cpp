//File: wxcvsimple.cpp
//Description: A simple example to use OpenCV with wxWidgets
//Author: Yan Naing Aye
//Date: 2017 November 01
//MIT License - Copyright (c) 2017 Yan Naing Aye

#include <wx/wx.h>
#include <opencv2/opencv.hpp>
#include "util.h"
using namespace cv;

class MyFrame : public wxFrame
{
    wxStaticBitmap *lena;
    wxStaticBitmap *thiri;
public:
    MyFrame(const wxString& title);

};
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(512, 256))
{
    Centre();
    lena = new wxStaticBitmap(this,wxID_ANY,wxBitmap(wxT("lena.jpg"),wxBITMAP_TYPE_JPEG),wxPoint(0,0),wxSize(256, 256));
    thiri = new wxStaticBitmap(this,wxID_ANY,wxBitmap(wxT("lena.jpg"),wxBITMAP_TYPE_JPEG),wxPoint(256,0),wxSize(256, 256));

    //From opencv to wx
    Mat imcv1=imread("thiri.jpg",CV_LOAD_IMAGE_COLOR);
    wxBitmap imwx1=wx_from_mat(imcv1);
    thiri->SetBitmap(imwx1);

    //From wx to opencv
    wxImage imwx2;
    imwx2.LoadFile(wxT("lena.jpg"), wxBITMAP_TYPE_JPEG);
    Mat imcv2=mat_from_wx(imwx2);
    namedWindow("Img",CV_WINDOW_AUTOSIZE);
    imshow("Img",imcv2);
}

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
    wxInitAllImageHandlers();
    MyFrame *frame = new MyFrame(wxT("Simple wxWidgets + OpenCV"));
    frame->Show(true);

    return true;
}
