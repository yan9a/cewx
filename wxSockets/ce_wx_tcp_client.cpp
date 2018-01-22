// File: ce_wx_tcp_client.cpp
// Description: A simple wxWidgets TCP client sample 
// Author: Yan Naing Aye
// Web: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye
// References
// [1] Guillermo Rodriguez Garcia, "Client for wxSocket demo,"
//   https://github.com/wxWidgets/wxWidgets/blob/master/samples/sockets/client.cpp, 1999.
// [2] Julian Smart and Kevin Hock, "Cross-Platform GUI Programming with wxWidgets,"
//    Pearson Education, Inc. 2006. ISBN: 0-13-147381-6.

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/socket.h"

#if wxUSE_IPV6
typedef wxIPV6address IPaddress;
#else
typedef wxIPV4address IPaddress;
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "./sample.xpm"
#endif

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
	// ctor(s)
	MyFrame(const wxString& title);
	~MyFrame();
	// event handlers (these functions should _not_ be virtual)
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	// event handlers for Socket menu
	void OnOpenConnection(wxCommandEvent& event);
	void OnCloseConnection(wxCommandEvent& event);
	void OnSend(wxCommandEvent& event);
	void OnSocketEvent(wxSocketEvent& event);

	// convenience functions
	void UpdateStatusBar();

private:

	wxSocketClient *sock;
	wxButton *btnSend;
	wxTextCtrl *txtSend;
	wxTextCtrl *txtRx;
	wxMenu *fileMenu;
	wxMenu *helpMenu;
	// any class wishing to process wxWidgets events must use this macro
	wxDECLARE_EVENT_TABLE();
};

// IDs for the controls and the menu commands
enum
{
	ID_BTNSEND=101,
	ID_TXTSEND,
	ID_TXTRX,
	SOCKET_ID,
	CLIENT_OPEN=wxID_OPEN,
	CLIENT_CLOSE=wxID_CLOSE,
	// menu items
	Minimal_Quit = wxID_EXIT,
	Minimal_About = wxID_ABOUT
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
EVT_MENU(Minimal_About, MyFrame::OnAbout)
EVT_SOCKET(SOCKET_ID, MyFrame::OnSocketEvent)
EVT_MENU(CLIENT_OPEN, MyFrame::OnOpenConnection)
EVT_MENU(CLIENT_CLOSE, MyFrame::OnCloseConnection)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// 'Main program'
bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	// create the main application window
	MyFrame *frame = new MyFrame("wxWidgets TCP Client");

	frame->Show(true);
	return true;
}

// frame constructor
MyFrame::MyFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(390, 280)
        ,wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
{
	// set the frame icon
	SetIcon(wxICON(sample));

#if wxUSE_MENUS
	// create a menu bar
	fileMenu = new wxMenu;

	// the "About" item should be in the help menu
	helpMenu = new wxMenu;
	helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

	fileMenu->Append(CLIENT_OPEN, "&Open session\tAlt-O","Connect to server");
	fileMenu->Append(CLIENT_CLOSE,"&Close session\tAlt-C","Close connection");
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
	SetStatusText("TCP client using wxWidgets");
#endif // wxUSE_STATUSBAR
	btnSend = new wxButton(this, ID_BTNSEND, wxT("Send"), 
        wxPoint(5, 5), wxSize(100, 25));
	txtSend = new wxTextCtrl(this,ID_TXTSEND, wxT("Hello!"), 
        wxPoint(120, 5), wxSize(250, 25));
	txtRx = new wxTextCtrl(this,ID_TXTRX, wxT(""), 
        wxPoint(5, 35), wxSize(365, 125), wxTE_MULTILINE);

	Connect(ID_BTNSEND, wxEVT_COMMAND_BUTTON_CLICKED, 
        wxCommandEventHandler(MyFrame::OnSend));

	// Create the socket
	sock = new wxSocketClient();

	// Setup the event handler and subscribe to most events
	sock->SetEventHandler( *this, SOCKET_ID);
	sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
		wxSOCKET_INPUT_FLAG |
		wxSOCKET_LOST_FLAG);
	sock->Notify(true);
}

MyFrame::~MyFrame()
{
	// No delayed deletion here, as the frame is dying anyway
	delete sock;
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
		"wxWidgets TCP client sample\n"
		"\n"
		"Author: Yan Naing Aye \n"
		"Web: http://cool-emerald.blogspot.com"
	),
		"About wxWidgets TCP client sample",
		wxOK | wxICON_INFORMATION,
		this);
}

void MyFrame::OnSend(wxCommandEvent& WXUNUSED(event))
{
	wxString str = txtSend->GetValue();
	wxCharBuffer buffer = str.ToUTF8();
	size_t txn = str.length();

	unsigned char len;
	len = txn;
	sock->Write(&len, 1);//send the length of the message first
	if (sock->Write(buffer.data(), txn).LastCount() != txn)
	{
		txtRx->AppendText(wxT("Write error.\n"));
		return;
	}
	else {
		txtRx->AppendText("Tx: " + str + "\n");
	}
}

void MyFrame::OnOpenConnection(wxCommandEvent& WXUNUSED(event))
{
	// Create the address - defaults to localhost:0 initially
	IPaddress addr;
	//addr.AnyAddress();
	addr.Hostname("localhost");
	addr.Service(3000);
	txtRx->AppendText(wxString::Format(wxT("Trying to connect to %s:%u \n"), 
        addr.IPAddress(), addr.Service()));

	fileMenu->Enable(CLIENT_OPEN, false);
	fileMenu->Enable(CLIENT_CLOSE, false);
	// we connect asynchronously and will get a wxSOCKET_CONNECTION event when
	// the connection is really established
	//
	// if you want to make sure that connection is established right here you
	// could call WaitOnConnect(timeout) instead

	sock->Connect(addr, false);

	//update status
	UpdateStatusBar();
}

void MyFrame::OnCloseConnection(wxCommandEvent& WXUNUSED(event))
{
	sock->Close();
	
	//update status
	UpdateStatusBar();
}

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
	txtRx->AppendText(wxT("OnSocketEvent: "));
	wxSocketBase *sockBase = event.GetSocket();

	// First, print a message
	switch (event.GetSocketEvent())
	{
	case wxSOCKET_INPUT: txtRx->AppendText(wxT("wxSOCKET_INPUT\n")); break;
	case wxSOCKET_LOST: txtRx->AppendText(wxT("wxSOCKET_LOST\n")); break;
	case wxSOCKET_CONNECTION: txtRx->AppendText(wxT("wxSOCKET_CONNECTION\n")); break;
	default: txtRx->AppendText(wxT("Unexpected event !\n")); break;
	}

	// Now we process the event
	switch (event.GetSocketEvent())
	{
	case wxSOCKET_INPUT:
	{
		// We disable input events, so that the test doesn't trigger
		// wxSocketEvent again.
		sockBase->SetNotify(wxSOCKET_LOST_FLAG);

		// Receive data from socket and send it back. We will first
		// get a byte with the buffer size, so we can specify the
		// exact size and use the wxSOCKET_WAITALL flag. Also, we
		// disabled input events so we won't have unwanted reentrance.
		// This way we can avoid the infamous wxSOCKET_BLOCK flag.

		sockBase->SetFlags(wxSOCKET_WAITALL);

		// Read the size @ first byte
		unsigned char len;
		sockBase->Read(&len, 1);
		char buf[256];
		// Read the message
		wxUint32 lenRd = sockBase->Read(buf, len).LastCount();
		if (!lenRd) {
			txtRx->AppendText(wxT("Failed to read message.\n"));
			return;
		}
		else {
			txtRx->AppendText(wxString::Format(wxT("Read %d bytes.\n"), lenRd));
		}

		txtRx->AppendText(wxString::Format(wxT("Rx: %s \n"), 
            wxString::FromUTF8(buf, len)));	
		// Enable input events again.
		sockBase->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
		break;
	}
	default:;
	}

	//update status
	UpdateStatusBar();
}

void MyFrame::UpdateStatusBar()
{
	fileMenu->Enable(CLIENT_OPEN, !sock->IsConnected());
	fileMenu->Enable(CLIENT_CLOSE, sock->IsConnected());
	if (sock->IsConnected()) {
		//SetStatusText(wxString::Format(wxT("%s:%u"), 
        //    addr.IPAddress(), addr.Service()), 1);
		SetStatusText(wxString::Format(wxT("Connected")), 1);
	}
	else {
		SetStatusText(wxString::Format(wxT("Not connected")), 1);
	}
}
