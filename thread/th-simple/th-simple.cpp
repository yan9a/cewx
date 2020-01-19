// File: th-simple.cpp
// Description: Using wxThread
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References (Original example)
// https://github.com/wxWidgets/wxWidgets/blob/master/samples/thread/thread.cpp

#include "wx/wx.h"
#include "./sample.xpm"
#include <string>
#include <stdio.h>
using namespace std;

WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);
// --------------------------------------------------------
// a simple thread

class MyThread : public wxThread
{
public:
    MyThread();
    virtual ~MyThread();

    // thread execution starts here
    virtual void *Entry();

public:
    int m_count;
};

// --------------------------------------------------------
class MyApp : public wxApp
{
public:
    MyApp();
    virtual ~MyApp(){};
    virtual bool OnInit();
    wxCriticalSection m_critsect;
    wxArrayThread m_threads;

    // semaphore used to wait for the threads to exit, see MyFrame::OnQuit()
    wxSemaphore m_semAllDone;

    // indicates that we're shutting down and all threads should exit
    bool m_shuttingDown;
};

// --------------------------------------------------------
class MyFrame : public wxFrame, private wxLog
{
public:
    MyFrame(const wxString& title);
    ~MyFrame();
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnStart(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnResume(wxCommandEvent& event);
    void WriteList(string mes);
protected:
    virtual void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info);
private:
    wxListBox *logList;
    wxButton *btnStart;
    wxButton *btnStop;
    wxButton *btnPause;
    wxButton *btnResume;
    // old log target, we replace it with one using m_txtctrl during this
    // frame life time
    wxLog *m_oldLogger;

    // helper function - creates a new thread (but doesn't run it)
    MyThread *CreateThread();

    wxDECLARE_EVENT_TABLE();
};
// --------------------------------------------------------
// IDs for the controls and the menu commands
enum
{
    ID_MNU_QUIT = wxID_EXIT,
    ID_MNU_ABOUT = wxID_ABOUT,
    ID_LST_LOG,
    ID_BTN_START,
    ID_BTN_STOP,
    ID_BTN_PAUSE,
    ID_BTN_RESUME
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_MNU_QUIT,  MyFrame::OnQuit)
    EVT_MENU(ID_MNU_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
    m_shuttingDown = false;
}

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
    m_oldLogger = wxLog::GetActiveTarget();
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
    btnStop = new wxButton(this, ID_BTN_STOP, wxT("Stop"), 
      wxPoint(140,20), wxSize(100, 25));
    btnPause = new wxButton(this, ID_BTN_PAUSE, wxT("Pause"), 
      wxPoint(260,20), wxSize(100, 25));
    btnResume = new wxButton(this, ID_BTN_RESUME, wxT("Resume"), 
      wxPoint(380,20), wxSize(100, 25));
    logList=new wxListBox(this,ID_LST_LOG,wxPoint(20,70),wxSize(560, 250));

    Connect(ID_BTN_START, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(MyFrame::OnStart));
    Connect(ID_BTN_STOP, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(MyFrame::OnStop));
    Connect(ID_BTN_PAUSE, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(MyFrame::OnPause));
    Connect(ID_BTN_RESUME, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(MyFrame::OnResume));
    wxLog::SetActiveTarget(this);
}

MyFrame::~MyFrame()
{
    wxLog::SetActiveTarget(m_oldLogger);


    // NB: although the OS will terminate all the threads anyhow when the main
    //     one exits, it's good practice to do it ourselves -- even if it's not
    //     completely trivial in this example

    // tell all the threads to terminate: note that they can't terminate while
    // we're deleting them because they will block in their OnExit() -- this is
    // important as otherwise we might access invalid array elements

    {
        wxCriticalSectionLocker locker(wxGetApp().m_critsect);

        // check if we have any threads running first
        const wxArrayThread& threads = wxGetApp().m_threads;
        size_t count = threads.GetCount();

        if ( !count )
            return;

        // set the flag indicating that all threads should exit
        wxGetApp().m_shuttingDown = true;
    }

    // now wait for them to really terminate
    wxGetApp().m_semAllDone.Wait();
}

void MyFrame::WriteList(string mes)
{    
    int nl=logList->GetCount();
    if(nl>=20){ logList->Delete(0); }    
    wxString wstr = mes;
    wxDateTime wdt;
    wdt.SetToCurrent();
    wstr = wdt.Format(wxT("%Y-%m-%d %H:%M:%S"), wxDateTime::Local)+
        " : "+wstr; // add timestamp
    wxArrayString astr;
    astr.Add(wstr);
    nl=logList->GetCount();
    logList->InsertItems(astr,nl);
}

void MyFrame::DoLogRecord(wxLogLevel level, 
    const wxString& msg, const wxLogRecordInfo& info)
{
    /*     
    if ( level <= wxLOG_Warning || level == wxLOG_Trace )
    {
        m_oldLogger->LogRecord(level, msg, info);
        return;
    } 
    */
    this->WriteList(msg.ToStdString());
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
    MyThread *thread = CreateThread(); // call the following function
    if ( thread->Run() != wxTHREAD_NO_ERROR )
    {
        wxLogStatus(wxT("Can't start thread!"));
    }
}

MyThread *MyFrame::CreateThread()
{
    MyThread *thread = new MyThread;

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogStatus(wxT("Can't create thread!"));
    }

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
    wxGetApp().m_threads.Add(thread);

    return thread;
}

void MyFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    wxThread* toDelete = NULL;
    {
        wxCriticalSectionLocker enter(wxGetApp().m_critsect);

        // stop the last thread
        if ( wxGetApp().m_threads.IsEmpty() )
        {
            wxLogStatus(wxT("No thread to stop!"));
        }
        else
        {
            toDelete = wxGetApp().m_threads.Last();
        }
    }

    if ( toDelete )
    {
        // This can still crash if the thread gets to delete itself
        // in the mean time.
        toDelete->Delete();
        wxLogStatus(wxT("Last thread stopped."), 1);
    }
}

void MyFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{       
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    // pause last running thread
    int n = wxGetApp().m_threads.Count() - 1;
    while ( n >= 0 && !wxGetApp().m_threads[n]->IsRunning()) n--;

    if ( n < 0 ) {
        wxLogStatus(wxT("No thread to pause!"));
    }
    else {
        wxGetApp().m_threads[n]->Pause();
        wxLogStatus(wxT("Thread paused."), 1);
    }
}

void MyFrame::OnResume(wxCommandEvent& WXUNUSED(event))
{       
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    // resume first suspended thread
    size_t n = 0, count = wxGetApp().m_threads.Count();
    while ( n < count && !wxGetApp().m_threads[n]->IsPaused() )
        n++;

    if ( n == count ) {
        wxLogStatus(wxT("No thread to resume!"));
    }
    else {
        wxGetApp().m_threads[n]->Resume();
        wxLogStatus(wxT("Thread resumed."), 1);
    }
}

// --------------------------------------------------------
// MyThread
MyThread::MyThread()
        : wxThread()
{
    m_count = 0;
}

MyThread::~MyThread()
{
    wxCriticalSectionLocker locker(wxGetApp().m_critsect);

    wxArrayThread& threads = wxGetApp().m_threads;
    threads.Remove(this);

    if ( threads.IsEmpty() )
    {
        // signal the main thread that there are no more threads left if it is
        // waiting for us
        if ( wxGetApp().m_shuttingDown )
        {
            wxGetApp().m_shuttingDown = false;

            wxGetApp().m_semAllDone.Post();
        }
    }
}

wxThread::ExitCode MyThread::Entry()
{
    wxLogMessage("Thread started (priority = %u).", GetPriority());

    for ( m_count = 0; m_count < 10; m_count++ )
    {
        // check if the application is shutting down: in this case all threads
        // should stop a.s.a.p.
        {
            wxCriticalSectionLocker locker(wxGetApp().m_critsect);
            if ( wxGetApp().m_shuttingDown )
                return NULL;
        }

        // check if just this thread was asked to exit
        if ( TestDestroy() )
            break;

        wxLogMessage("Thread progress: %u", m_count);

        // wxSleep() can't be called from non-GUI thread!
        wxThread::Sleep(1000);
    }

    wxLogMessage("Thread finished.");

    return NULL;
}
// --------------------------------------------------------
