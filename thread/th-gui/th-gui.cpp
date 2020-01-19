// File: th-gui.cpp
// Description: Using wxThread
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References (Original example)
// https://github.com/wxWidgets/wxWidgets/blob/master/samples/thread/thread.cpp

#include "wx/wx.h"
#include "./sample.xpm"

// --------------------------------------------------------
// a thread which executes GUI calls using wxMutexGuiEnter/Leave

#define GUITHREAD_BMP_SIZE          300
#define GUITHREAD_NUM_UPDATES       50
class MyImageDialog;

class MyGUIThread : public wxThread
{
public:
    MyGUIThread(MyImageDialog *dlg) : wxThread(wxTHREAD_JOINABLE) {
        m_dlg = dlg;
    }
    virtual ExitCode Entry();

private:
    MyImageDialog *m_dlg;
};

// --------------------------------------------------------
// an helper dialog used by MyFrame::OnStartGUIThread

class MyImageDialog: public wxDialog
{
public:
    // ctor
    MyImageDialog(wxFrame *frame);
    ~MyImageDialog();

    // stuff used by MyGUIThread:
    wxBitmap m_bmp;    // the bitmap drawn by MyGUIThread
    wxCriticalSection m_csBmp;        // protects m_bmp

private:
    void OnGUIThreadEvent(wxThreadEvent& event);
    void OnPaint(wxPaintEvent&);

    MyGUIThread m_thread;
    int m_nCurrentProgress;

    wxDECLARE_EVENT_TABLE();
};

// --------------------------------------------------------
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    
};
// --------------------------------------------------------
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    ~MyFrame();
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnStart(wxCommandEvent& event);
private:
    wxButton *btnStart;
    wxStaticText *lblText;

    wxDECLARE_EVENT_TABLE();
};

// --------------------------------------------------------
// IDs for the controls and the menu commands
enum
{
    ID_MNU_QUIT = wxID_EXIT,
    ID_MNU_ABOUT = wxID_ABOUT,
    ID_BTN_START,
    ID_LBL_TEXT,
    GUITHREAD_EVENT 
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_MNU_QUIT,  MyFrame::OnQuit)
    EVT_MENU(ID_MNU_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
    MyFrame *frame = new MyFrame("wxWidgets App");
    frame->Show(true);
    return true;
}
// --------------------------------------------------------
// MyFrame

MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 450), 
          wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
{
    SetIcon(wxICON(sample));
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_MNU_ABOUT, "&About\tF1", "Show about dialog");
    fileMenu->Append(ID_MNU_QUIT, "E&xit\tAlt-X", "Quit this program");
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar(1);
    SetStatusText("wxWidgets App");

    btnStart = new wxButton(this, ID_BTN_START, wxT("Start"), 
      wxPoint(20,20), wxSize(100, 25));
    Connect(ID_BTN_START, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(MyFrame::OnStart));
    lblText = new wxStaticText(this,ID_LBL_TEXT,wxT("Click the button to start the gui thread"),
      wxPoint(20,50));
}

MyFrame::~MyFrame()
{

}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
   wxMessageBox(wxString::Format
    (
    "wxWidgets thread\n"
    "\n"
    "Author: Yan Naing Aye \n"
    "Web: http://cool-emerald.blogspot.com"
    ),
    "About wxWidgets thread",
    wxOK | wxICON_INFORMATION,
    this);
}

void MyFrame::OnStart(wxCommandEvent& WXUNUSED(event))
{       
    MyImageDialog dlg(this);
    dlg.ShowModal();
}

// --------------------------------------------------------
// MyImageDialog

wxBEGIN_EVENT_TABLE(MyImageDialog, wxDialog)
    EVT_THREAD(GUITHREAD_EVENT, MyImageDialog::OnGUIThreadEvent)
    EVT_PAINT(MyImageDialog::OnPaint)
wxEND_EVENT_TABLE()

MyImageDialog::MyImageDialog(wxFrame *parent)
    : wxDialog(parent, wxID_ANY, "Image created by a secondary thread",
               wxDefaultPosition, wxSize(GUITHREAD_BMP_SIZE,GUITHREAD_BMP_SIZE)*1.5, wxDEFAULT_DIALOG_STYLE),
      m_thread(this)
{
    m_nCurrentProgress = 0;

    CentreOnScreen();

    // NOTE: no need to lock m_csBmp until the thread isn't started:

    // create the bitmap
    if (!m_bmp.Create(GUITHREAD_BMP_SIZE,GUITHREAD_BMP_SIZE) || !m_bmp.IsOk())
    {
        wxLogError("Couldn't create the bitmap!");
        return;
    }

    // clean it
    wxMemoryDC dc(m_bmp);
    dc.SetBackground( *wxBLACK_BRUSH);
    dc.Clear();

    // draw the bitmap from a secondary thread
    if ( m_thread.Create() != wxTHREAD_NO_ERROR ||
         m_thread.Run() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create/run thread!"));
        return;
    }
}

MyImageDialog::~MyImageDialog()
{
    // in case our thread is still running and for some reason we are destroyed,
    // do wait for the thread to complete as it assumes that its MyImageDialog
    // pointer is always valid
    m_thread.Delete();
}

void MyImageDialog::OnGUIThreadEvent(wxThreadEvent& event)
{
    m_nCurrentProgress = int(((float)event.GetInt()*100)/GUITHREAD_NUM_UPDATES);

    Refresh();
}

void MyImageDialog::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);

    const wxSize& sz = dc.GetSize();

    {
        // paint the bitmap
        wxCriticalSectionLocker locker(m_csBmp);
        dc.DrawBitmap(m_bmp, (sz.GetWidth()-GUITHREAD_BMP_SIZE)/2,
                             (sz.GetHeight()-GUITHREAD_BMP_SIZE)/2);
    }

    // paint a sort of progress bar with a 10px border:
    dc.SetBrush( *wxRED_BRUSH);
    dc.DrawRectangle(10,10, m_nCurrentProgress*(sz.GetWidth()-20)/100,30);
    dc.SetTextForeground( *wxBLUE);
    dc.DrawText(wxString::Format("%d%%", m_nCurrentProgress),
                (sz.GetWidth()-dc.GetCharWidth()*2)/2,
                25-dc.GetCharHeight()/2);
}
// --------------------------------------------------------
// MyGUIThread

wxThread::ExitCode MyGUIThread::Entry()
{

    for (int i=0; i<GUITHREAD_NUM_UPDATES && !TestDestroy(); i++)
    {
        // inform the GUI toolkit that we're going to use GUI functions
        // from a secondary thread:
        wxMutexGuiEnter();

        {
            wxCriticalSectionLocker lock(m_dlg->m_csBmp);

            // draw some more stuff on the bitmap
            wxMemoryDC dc(m_dlg->m_bmp);
            dc.SetBrush((i%2)==0 ? *wxBLUE_BRUSH : *wxGREEN_BRUSH);
            dc.DrawRectangle(rand()%GUITHREAD_BMP_SIZE, rand()%GUITHREAD_BMP_SIZE, 30, 30);

            // simulate long drawing time:
            wxMilliSleep(200);
        }

        // if we don't release the GUI mutex the MyImageDialog won't be able to refresh
        wxMutexGuiLeave();

        // notify the dialog that another piece of our masterpiece is complete:
        wxThreadEvent event( wxEVT_THREAD, GUITHREAD_EVENT );
        event.SetInt(i+1);
        wxQueueEvent( m_dlg, event.Clone() );

        // give the main thread the time to refresh before we lock the GUI mutex again
        // FIXME: find a better way to do this!
        wxMilliSleep(100);
    }


    return (ExitCode)0;
}
// --------------------------------------------------------