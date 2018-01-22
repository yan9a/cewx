// File: ce_wx_udp.cpp
// Description: A simpler version of wxWidgets UDP sample 
// Author: Yan Naing Aye
// Web: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References
// [1] Guillermo Rodriguez Garcia, Vadim Zeitlin, 
//     "Server for wxSocket demo", 
//     https://github.com/wxWidgets/wxWidgets/blob/master/samples/sockets/server.cpp, 2009.


#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/socket.h"

// this example is currently written to use only IP or only IPv6 sockets, it
// should be extended to allow using either in the future
#if wxUSE_IPV6
typedef wxIPV6address IPaddress;
#else
typedef wxIPV4address IPaddress;
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "./sample.xpm"
#endif

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
// override base class virtuals
// ----------------------------

// this one is called on application startup and is a good place for the app
// initialization (doing it here and not in the ctor allows to have an error
// return: if OnInit() returns false, the application terminates)
virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
// ctor(s)
MyFrame(const wxString& title);

// event handlers (these functions should _not_ be virtual)
void OnQuit(wxCommandEvent& event);
void OnAbout(wxCommandEvent& event);
void OnSend(wxCommandEvent& event);
void OnSocketEvent(wxSocketEvent& event);
private:

wxDatagramSocket *sock;
wxButton *btnSend;
wxTextCtrl *txtSend;
wxTextCtrl *txtRx;
// any class wishing to process wxWidgets events must use this macro
wxDECLARE_EVENT_TABLE();
};

// constants
const int ID_BTNSEND = 101;
const int ID_TXTSEND = 102;
const int ID_TXTRX = 103;

// IDs for the controls and the menu commands
enum
{
Button_Send = ID_BTNSEND,
Txt_Send = ID_TXTSEND,
Txt_Rx = ID_TXTRX,
SOCKET_ID,
// menu items
Minimal_Quit = wxID_EXIT,

// it is important for the id corresponding to the "About" command to have
// this standard value as otherwise it won't be handled properly under Mac
// (where it is special and put into the "Apple" menu)
Minimal_About = wxID_ABOUT
};


// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
EVT_MENU(Minimal_About, MyFrame::OnAbout)
EVT_SOCKET(SOCKET_ID, MyFrame::OnSocketEvent)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
// call the base class initialization method, currently it only parses a
// few common command-line options but it could be do more in the future
if ( !wxApp::OnInit() )
return false;

// create the main application window
MyFrame *frame = new MyFrame("wxWidgets UDP App");

// and show it (the frames, unlike simple controls, are not shown when
// created initially)
frame->Show(true);

// success: wxApp::OnRun() will be called which will enter the main message
// loop and the application will run. If we returned false here, the
// application would exit immediately.
return true;
}

// frame constructor
MyFrame::MyFrame(const wxString& title)
: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(390, 280), 
  wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
{
// set the frame icon
SetIcon(wxICON(sample));

#if wxUSE_MENUS
// create a menu bar
wxMenu *fileMenu = new wxMenu;

// the "About" item should be in the help menu
wxMenu *helpMenu = new wxMenu;
helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

// now append the freshly created menu to the menu bar...
wxMenuBar *menuBar = new wxMenuBar();
menuBar->Append(fileMenu, "&File");
menuBar->Append(helpMenu, "&Help");

// ... and attach this menu bar to the frame
SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
// create a status bar just for fun (by default with 1 pane only)
CreateStatusBar(2);
SetStatusText("UDP using wxWidgets");
#endif // wxUSE_STATUSBAR
btnSend = new wxButton(this, Button_Send, wxT("Send"), 
            wxPoint(5, 5), wxSize(100, 25));
txtSend = new wxTextCtrl(this, Txt_Send, wxT("Hello!"), 
            wxPoint(120, 5), wxSize(250, 25));
txtRx = new wxTextCtrl(this, Txt_Rx, wxT(""), wxPoint(5, 35), 
            wxSize(365, 125), wxTE_MULTILINE);

Connect(Button_Send, wxEVT_COMMAND_BUTTON_CLICKED, 
    wxCommandEventHandler(MyFrame::OnSend));

// Create the address - defaults to localhost:0 initially
IPaddress addr;
addr.AnyAddress();
addr.Service(3000);
txtRx->AppendText(wxString::Format(wxT("Creating UDP socket at %s:%u \n"), 
    addr.IPAddress(), addr.Service()));

// Create the socket
sock = new wxDatagramSocket(addr);

// We use IsOk() here to see if the server is really listening
if (!sock->IsOk()){
	txtRx->AppendText(wxString::Format(wxT("Could not listen at the specified port !\n")));
	return;
}

IPaddress addrReal;
if (!sock->GetLocal(addrReal)){
	txtRx->AppendText(wxString::Format(wxT("ERROR: couldn't get the address we bound to. \n")));
}
else{
	txtRx->AppendText(wxString::Format(wxT("Server listening at %s:%u \n"),
		addrReal.IPAddress(), addrReal.Service()));
}
// Setup the event handler
sock->SetEventHandler( *this, SOCKET_ID);
sock->SetNotify(wxSOCKET_INPUT_FLAG);
sock->Notify(true);
}


// event handlers
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
// true is to force the frame to close
Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
wxMessageBox(wxString::Format
(
"wxWidgets UDP sample\n"
"\n"
"Author: Yan Naing Aye \n"
"Web: http://cool-emerald.blogspot.com"
),
"About wxWidgets UDP sample",
wxOK | wxICON_INFORMATION,
this);
}

void MyFrame::OnSend(wxCommandEvent& WXUNUSED(event))
{
	wxString str = txtSend->GetValue();
	wxCharBuffer buffer = str.ToUTF8();
	size_t txn = str.length();

	IPaddress raddr;
	raddr.Hostname("localhost");
    //raddr.Hostname("192.168.2.71");
	raddr.Service(3001);
	if (sock->SendTo(raddr, buffer.data(), txn).LastCount() != txn)
	{
		txtRx->AppendText(wxT("Write error.\n"));
		return;
	}
	else {
		txtRx->AppendText("Tx: "+str+"\n");
	}
}

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
	IPaddress addr;
	addr.Service(3000);
	char buf[1024];
	size_t n;
	switch(event.GetSocketEvent())
	{
	case wxSOCKET_INPUT:
		//txtRx->AppendText("OnSocketEvent: wxSOCKET_INPUT\n");
		sock->Notify(false);		
		n = sock->RecvFrom(addr, buf, sizeof(buf)).LastCount();
		if (!n)	{
			txtRx->AppendText("ERROR: failed to receive data \n");
			return;
		}
		//txtRx->AppendText(wxString::Format(wxT("Received \"%s\" from %s:%u.\n"),
		//	wxString::From8BitData(buf, n),addr.IPAddress(), addr.Service()));
		txtRx->AppendText("Rx: "+wxString::From8BitData(buf, n) + "\n");
		sock->Notify(true);
		break;
	default: txtRx->AppendText("OnSocketEvent: Unexpected event !\n"); break;
	}
}
