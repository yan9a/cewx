
// File: wxpp.cpp
// Description: using parallel port with wxWidgets on Linux
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include "wx/wx.h"
#include "./sample.xpm"
#include "cePPort.h"
using namespace ce;
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnChkOut(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
private:
	wxTimer m_timer;
    wxCheckBox *chkOut;
	wxCheckBox *chkIn;
    cePPort *p0;
    wxDECLARE_EVENT_TABLE();
};

const int ID_CHKOUT = 101;
const int ID_CHKIN = 102;
const int ID_TIMER = 103;
enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
    MyFrame *frame = new MyFrame("Parallel Port Control");
    frame->Show(true);
    return true;
}
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200), wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
       ,m_timer(this, ID_TIMER)
{
    SetIcon(wxICON(sample));
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar(2);
    SetStatusText("Parallel Port Control");
    chkOut = new wxCheckBox(this, ID_CHKOUT, wxT("OUTPUT-D0"), wxPoint(10, 10), wxDefaultSize);
	chkIn = new wxCheckBox(this, ID_CHKIN, wxT("INPUT-S7"), wxPoint(10, 40), wxDefaultSize);
    Connect(ID_CHKOUT,wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MyFrame::OnChkOut));
    Connect(ID_TIMER,wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnTimer));
	m_timer.Start(100);
	chkIn->Disable(); 
    p0=new cePPort();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Parallel Port Control\n"
                    "Author: Yan Naing Aye\n"
                    "Website: cool-emerald.blogspot.com"
                 ),
                 "About Parallel Port Control",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	bool r=p0->RdS(7);
    if(r!=chkIn->IsChecked()){
        chkIn->SetValue(r);
    }	
}

void MyFrame::OnChkOut(wxCommandEvent& WXUNUSED(event))
{
	if (chkOut->IsChecked()) {
		p0->WrD(true,0);
	}
    else {
        p0->WrD(false,0);
    }
}