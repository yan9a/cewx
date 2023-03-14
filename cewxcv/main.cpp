//File: cewxcv.cpp
//Description: A simple example to use OpenCV with wxWidgets
//Author: Yan Naing Aye
//Date: 2017 November 07
//MIT License - Copyright (c) 2017 Yan Naing Aye

#include <wx/wx.h>
#include <opencv2/opencv.hpp>
#include <string>
using namespace std;
using namespace cv;

//-----------------------------------------------------------------------------
//Description: Functions to convert wxImage and OpenCV Mat
wxImage wx_from_mat(Mat &img) {
    Mat im2;
    if(img.channels()==1){cvtColor(img,im2,COLOR_GRAY2RGB);}
	else if (img.channels() == 4) { cvtColor(img, im2, COLOR_BGRA2RGB);}
    else {cvtColor(img,im2,COLOR_BGR2RGB);}
	long imsize = im2.rows*im2.cols*im2.channels();
    wxImage wx(im2.cols, im2.rows,(unsigned char*)malloc(imsize), false);
	unsigned char* s=im2.data;
	unsigned char* d=wx.GetData();
	for (long i = 0; i < imsize; i++) { d[i] = s[i];}
    return wx;
}

Mat mat_from_wx(wxImage &wx) {
    Mat im2(Size(wx.GetWidth(),wx.GetHeight()),CV_8UC3,wx.GetData());
    cvtColor(im2,im2,COLOR_RGB2BGR);
    return im2;
}

//-----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
	wxStaticBitmap *thiri;
public:
	MyFrame(const wxString& title);

};
MyFrame::MyFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(512, 600))
{
	Centre();
	string fname = "./thiri.png";
	Mat imcv1 = imread(fname,IMREAD_UNCHANGED);	

	if (!imcv1.data) {
		// fail to read img
		imcv1 = Mat::zeros(512, 512, CV_8UC3);
		printf("Failed to read image %s\n",fname.c_str());
	}
	//From opencv to wx
	string str = "Channels:" + to_string(imcv1.channels());
	putText(imcv1, str, Point(100, 100), FONT_HERSHEY_PLAIN, 4.0, CV_RGB(128, 0, 128), 4.0);
	wxBitmap imwx1 = wx_from_mat(imcv1);

	// thiri = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("./thiri.png"), wxBITMAP_TYPE_PNG), wxPoint(256, 0), wxSize(512,512));
	thiri = new wxStaticBitmap(this, wxID_ANY, imwx1, wxPoint(256, 0), wxSize(512,512));
	// thiri->SetBitmap(imwx1);
}

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)
bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;
	wxInitAllImageHandlers();
	MyFrame *frame = new MyFrame(wxT("Simple wxWidgets and OpenCV"));
	frame->Show(true);

	return true;
}

