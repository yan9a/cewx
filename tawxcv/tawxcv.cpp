//File: tawxcv.cpp
//Description: A simple example to use Tesseract, OpenCV, and wxWidgets together
//Author: Yan Naing Aye
//Date: 2020 March 28
//MIT License - Copyright (c) 2020 Yan Naing Aye

#include <wx/wx.h>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
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
	wxStaticBitmap *img;
	wxTextCtrl *txt;
	wxDECLARE_EVENT_TABLE();
	Mat immat;
public:
	MyFrame(const wxString& title);
	void OnQuit(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnEng(wxCommandEvent& event);
	void OnMya(wxCommandEvent& event);
	Mat GetImageFromFile();
};

enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_Open =wxID_OPEN,
	Minimal_Txt,
	Minimal_Eng,
	Minimal_Mya
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_Open, MyFrame::OnOpen)
	EVT_MENU(Minimal_Eng, MyFrame::OnEng)
	EVT_MENU(Minimal_Mya, MyFrame::OnMya)
wxEND_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1024, 600))
{
	Centre();

	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(Minimal_Open, "&Open\tAlt-O", "Open img");
	fileMenu->Append(Minimal_Eng, "&English\tAlt-E", "Get text in English");
	fileMenu->Append(Minimal_Mya, "&Myanmar\tAlt-M", "Get text in Myanmar");
	fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");
	wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
	SetMenuBar(menuBar);

	img = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("./myaim.jpg"), wxBITMAP_TYPE_JPEG), wxPoint(0, 0), wxSize(512,512));
	Connect(Minimal_Open, wxEVT_COMMAND_MENU_SELECTED,
      wxCommandEventHandler(MyFrame::OnOpen));
	//From opencv to wx
	immat = imread("./myaim.jpg",IMREAD_UNCHANGED);
	wxBitmap imwx1 = wx_from_mat(immat);
	img->SetBitmap(imwx1);

	txt = new wxTextCtrl(this,Minimal_Txt,wxT("Hello!"),wxPoint(512,0),wxSize(512,512),wxTE_MULTILINE | wxTE_WORDWRAP);
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
	MyFrame *frame = new MyFrame(wxT("Simple Tesseract, wxWidgets and OpenCV"));
	frame->Show(true);

	return true;
}

void MyFrame::OnOpen(wxCommandEvent &event)
{
    immat=GetImageFromFile();
    resize(immat,immat,Size(512,512),0,0,INTER_AREA);
    wxBitmap nbm=wx_from_mat(immat);
    img->SetBitmap(nbm);
}

Mat MyFrame::GetImageFromFile()
{
    wxString caption = "Choose a file";
    wxString wildcard = "JPG files (*.jpg)|*.jpg";
    wxString defaultDir = "./";
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename,wildcard,wxFD_OPEN);
    wxString path = "./engim.jpg";
    if (dialog.ShowModal() == wxID_OK)
    {
        path = dialog.GetPath();
    }
    Mat img = imread(path.ToStdString(),IMREAD_COLOR);
    return img;
}

void MyFrame::OnEng(wxCommandEvent& WXUNUSED(event))
{
    char *otxt;
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // initialize tesseract-ocr with English
    if(api->Init(NULL,"eng")){
        fprintf(stderr,"Could not initialize tesseract.\n");
        exit(1);
    }

    // set image
    api->SetImage(immat.data,immat.cols,immat.rows,3,immat.step);

    // Get OCR result
    otxt = api->GetUTF8Text();

	txt->Clear();
	txt->AppendText(wxString::Format(wxT("%s"), otxt));

    // destroy used obj and release memory
    api->End();
    delete api;
    delete [] otxt;
}

void MyFrame::OnMya(wxCommandEvent& WXUNUSED(event))
{
    char *otxt;
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // initialize tesseract-ocr with English
    if(api->Init(NULL,"mya")){
        fprintf(stderr,"Could not initialize tesseract.\n");
        exit(1);
    }

    // set image
    api->SetImage(immat.data,immat.cols,immat.rows,3,immat.step);

    // Get OCR result
    otxt = api->GetUTF8Text();

	txt->Clear();
	wxString txt2 = wxString::FromUTF8(otxt);
	txt->AppendText(txt2);

    // destroy used obj and release memory
    api->End();
    delete api;
    delete [] otxt;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}