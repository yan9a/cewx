//File: minimalcv.cpp
//Description: A simple example to use OpenCV with wxWidgets
//Author: Yan Naing Aye
//Date: 2017 November 07
//MIT License - Copyright (c) 2017 Yan Naing Aye

#include <wx/wx.h>
#include <opencv2/opencv.hpp>
#include "util.h"
#include <string>
using namespace std;
using namespace cv;

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
	thiri = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("C:/opencv/thiri.png"), wxBITMAP_TYPE_PNG), wxPoint(256, 0), wxSize(512,512));

	//From opencv to wx
	Mat imcv1 = imread("C:/opencv/thiri.png",IMREAD_UNCHANGED);
	string str = "Channels:" + to_string(imcv1.channels());
	putText(imcv1, str, Point(100, 100), FONT_HERSHEY_PLAIN, 4.0, CV_RGB(128, 0, 128), 4.0);
	wxBitmap imwx1 = wx_from_mat(imcv1);
	thiri->SetBitmap(imwx1);
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

