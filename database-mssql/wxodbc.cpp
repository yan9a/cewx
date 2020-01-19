// File: wxodbc.cpp
// Description: Using MS SQL Server with ODBC and wxWidgets
// WebSite: http://cool-emerald.blogspot.sg
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include "wx/wx.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "./sample.xpm"
#endif

#include "ceODBC.h"

// Define a new application type, each program should derive a class from wxApp
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

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSql(wxCommandEvent& event);
    void WriteList(string mes);
private:
    wxListBox *logList;
    wxButton *btnSql;
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();

};

// IDs for the controls and the menu commands
enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    LSTBOX_LOG,
    Button_Sql
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;
    // create the main application window
    MyFrame *frame = new MyFrame("ODBC wxWidgets App");
    frame->Show(true);
    return true;
}

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 450), 
          wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER)
{
    // set the frame icon
    SetIcon(wxICON(sample));
    
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
    CreateStatusBar(1);
    SetStatusText("ODBC with wxWidgets");

    btnSql = new wxButton(this, Button_Sql, wxT("Sql Exe"), 
      wxPoint(20,20), wxSize(100, 25));
    Connect(Button_Sql, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(MyFrame::OnSql));
    logList=new wxListBox(this,LSTBOX_LOG,wxPoint(20,70),wxSize(560, 250));
}

void MyFrame::WriteList(string mes)
{    
    int nl=logList->GetCount();
    if(nl>=12){
        logList->Delete(0);
    }    
    wxString wstr = mes;
    wxDateTime wdt;
    wdt.SetToCurrent();
    // wstr = wdt.Format(wxT("%Y-%m-%d %H:%M:%S"), wxDateTime::Local)+" : "+wstr; // add timestamp
    wxArrayString astr;
    astr.Add(wstr);
    nl=logList->GetCount();
    logList->InsertItems(astr,nl);
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
    "wxWidgets ODBC sample\n"
    "\n"
    "Author: Yan Naing Aye \n"
    "Web: http://cool-emerald.blogspot.com"
    ),
    "About wxWidgets ODBC sample",
    wxOK | wxICON_INFORMATION,
    this);
}

void MyFrame::OnSql(wxCommandEvent& WXUNUSED(event))
{
  string rs;
  vector< vector<string> > td;
  
  ceODBC db; // initialize
  db.Connect("mssqlserver","sa","password"); // connect
  
  td = db.SQL("SELECT * FROM myTable"); // exe sql 
  
  // display
  for(int i=0;i<td.size();i++){
    rs="";
    for(int j=0;j<td[i].size();j++){
      rs+=td[i][j]+" \t";
    }
    WriteList(rs);
  }
  
  db.Disconnect(); // disconnect
}
