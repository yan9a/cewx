#include "wx/wx.h"
#include <stdio.h>
using namespace std;

// --------------------------------------------------------
// a simple thread

class MyThread : public wxThread
{
public:
    MyThread();
    virtual ~MyThread();

    // thread execution starts here
    virtual void *Entry();

private:
    int m_count;
};
// --------------------------------------------------------
// MyThread
MyThread::MyThread(): wxThread()
{
    m_count = 0;
}

MyThread::~MyThread()
{
}

wxThread::ExitCode MyThread::Entry()
{
    printf("Thread started (Priority = %d).\n",GetPriority());
    for ( m_count = 0; m_count < 10; m_count++ )
    {
        // check if just this thread was asked to exit
        if ( TestDestroy() ) break;
        printf("Thread progress: %d \n", m_count);
        // wxSleep() can't be called from non-GUI thread!
        wxThread::Sleep(1000);
    }
    printf("Thread finished.\n");
    return NULL;
}
// --------------------------------------------------------
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

};
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250, 150))
{
    Centre();

    MyThread *th = new MyThread;
    if ( th->Create() != wxTHREAD_NO_ERROR )
    {
         printf("Can't create thread! \n");
    }
    else {
        printf("New thread created.\n");
        if ( th->Run() != wxTHREAD_NO_ERROR )
        {
            printf("Can't start thread! \n");
        }
        else printf("New thread started.\n");
    }
}

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(wxT("Simple thread"));
    frame->Show(true);

    return true;
}
