// File: th-worker.cpp
// Description: Using wxThread
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References (Original example)
// https://github.com/wxWidgets/wxWidgets/blob/master/samples/thread/thread.cpp

#include "wx/wx.h"
#include "wx/progdlg.h"
#include "./sample.xpm"

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
    void OnWorkerEvent(wxThreadEvent& event);
    // accessors for MyWorkerThread (called in its context!)
    bool Cancelled();
private:
    wxButton *btnStart;
    wxStaticText *lblText;
   
    // the progress dialog which we show while worker thread is running
    wxProgressDialog *m_dlgProgress;

    // was the worker thread cancelled by user?
    bool m_cancelled;
    wxCriticalSection m_csCancelled;        // protects m_cancelled

    wxDECLARE_EVENT_TABLE();
};
// --------------------------------------------------------
// a worker thread

class MyWorkerThread : public wxThread
{
public:
    MyWorkerThread(MyFrame *frame);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

public:
    MyFrame *m_frame;
    unsigned m_count;
};

// --------------------------------------------------------
// IDs for the controls and the menu commands
enum
{
    ID_MNU_QUIT = wxID_EXIT,
    ID_MNU_ABOUT = wxID_ABOUT,
    ID_BTN_START,
    ID_WORKER_THREAD,
    ID_LBL_TEXT
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_MNU_QUIT,  MyFrame::OnQuit)
    EVT_MENU(ID_MNU_ABOUT, MyFrame::OnAbout)
    EVT_THREAD(ID_WORKER_THREAD, MyFrame::OnWorkerEvent)
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
    lblText = new wxStaticText(this,ID_LBL_TEXT,
        wxT("Click the button to start the worker thread"),
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
    MyWorkerThread *thread = new MyWorkerThread(this);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
       // wxLogError(wxT("Can't create thread!"));
        return;
    }

    m_dlgProgress = new wxProgressDialog
        (
            wxT("Progress dialog"),
            wxT("Wait until the thread terminates or press [Cancel]"),
            100,
            this,
            wxPD_CAN_ABORT |
            wxPD_APP_MODAL |
            wxPD_ELAPSED_TIME |
            wxPD_ESTIMATED_TIME |
            wxPD_REMAINING_TIME
        );

    // thread is not running yet, no need for crit sect
    m_cancelled = false;

    thread->Run();
}

bool MyFrame::Cancelled()
{
    wxCriticalSectionLocker lock(m_csCancelled);

    return m_cancelled;
}

void MyFrame::OnWorkerEvent(wxThreadEvent& event)
{
    int n = event.GetInt();
    if ( n == -1 )
    {
        m_dlgProgress->Destroy();
        m_dlgProgress = (wxProgressDialog *)NULL;

        // the dialog is aborted because the event came from another thread, so
        // we may need to wake up the main event loop for the dialog to be
        // really closed
        wxWakeUpIdle();
    }
    else
    {
        if ( !m_dlgProgress->Update(n) )
        {
            wxCriticalSectionLocker lock(m_csCancelled);

            m_cancelled = true;
        }
    }
}

// --------------------------------------------------------
// MyWorkerThread

MyWorkerThread::MyWorkerThread(MyFrame *frame)
        : wxThread()
{
    m_frame = frame;
    m_count = 0;
}

void MyWorkerThread::OnExit()
{
}

wxThread::ExitCode MyWorkerThread::Entry()
{
    for ( m_count = 0; !m_frame->Cancelled() && (m_count < 100); m_count++ )
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;

        // create any type of command event here
        wxThreadEvent event( wxEVT_THREAD, ID_WORKER_THREAD );
        event.SetInt( m_count );

        // send in a thread-safe way
        wxQueueEvent( m_frame, event.Clone() );

        wxMilliSleep(200);
    }

    wxThreadEvent event( wxEVT_THREAD, ID_WORKER_THREAD );
    event.SetInt(-1); // that's all
    wxQueueEvent( m_frame, event.Clone() );

    return NULL;
}

// --------------------------------------------------------
