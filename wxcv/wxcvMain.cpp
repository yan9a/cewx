/***************************************************************
 * Name:      wxcvMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    yan
 * Created:   2017-11-02
 * Copyright: yan (http://cool-emerald.blogspot.sg/)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__
#include "wxcvMain.h"

#include <opencv2/opencv.hpp>
using namespace cv;

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(wxcvFrame, wxFrame)
    EVT_CLOSE(wxcvFrame::OnClose)
    EVT_MENU(idMenuQuit, wxcvFrame::OnQuit)
    EVT_MENU(idMenuAbout, wxcvFrame::OnAbout)
END_EVENT_TABLE()

wxcvFrame::wxcvFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR
    Mat img = imread("grove.jpg", CV_LOAD_IMAGE_COLOR);
    namedWindow( "grove", CV_WINDOW_AUTOSIZE );
    imshow("grove", img);
}


wxcvFrame::~wxcvFrame()
{
}

void wxcvFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void wxcvFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void wxcvFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}
