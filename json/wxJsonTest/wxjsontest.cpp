/////////////////////////////////////////////////////////////////////////////
// Name:        wxjsontest.cpp
// Purpose:     Json using wxWidgets
// Author:      Yan Naing Aye
// Created:     25/03/2020
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

#include "wx/json_defs.h"
#include "wx/jsonreader.h"
#include "wx/jsonval.h"
#include "wx/jsonwriter.h"

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
    wxString ReadJson();
    wxTextCtrl *txt;
private:    
    wxDECLARE_EVENT_TABLE();
};

enum
{
    Minimal_Quit = wxID_EXIT
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
    MyFrame *frame = new MyFrame("wxJson");
    frame->Show(true);
    return true;
}
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);
    wxString jresult = ReadJson();
    txt = new wxTextCtrl(this,wxID_ANY, jresult, wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_RICH , wxDefaultValidator, wxTextCtrlNameStr);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

wxString MyFrame::ReadJson()
{
	// the JSON text, stored in a wxString object
	wxString document(("{\"Serial\" : {\"Baud\":9600, \"Parity\":\"Even\"}}"));
	wxString mes;

	// construct the JSON root object
	wxJSONValue  root;
	// construct a JSON parser
	wxJSONReader reader;

	// now read the JSON text and store it in the 'root' structure
	// check for errors before retreiving values...
	int numErrors = reader.Parse(document, &root);
    if (numErrors > 0) {
		mes = "ERROR: the JSON document is not well-formed";
	    //	const wxArrayString& errors = reader.GetErrors();
        return mes;
	}
	else {
		mes = "OK: Reading the JSON document... \n";
	}

	// get the values
	wxString v1 = root["Serial"]["Baud"].AsString();
	wxString v2 = root["Serial"]["Parity"].AsString();
	mes += "Serial: baudrate = "+ v1 + ", parity = " + v2;
    return mes;
}
