/***************************************************************
 * Name:      wxcvMain.h
 * Purpose:   Defines Application Frame
 * Author:    yan
 * Created:   2017-11-02
 * Copyright: yan (http://cool-emerald.blogspot.sg/)
 * License:
 **************************************************************/

#ifndef WXCVMAIN_H
#define WXCVMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wxcvApp.h"

class wxcvFrame: public wxFrame
{
    public:
        wxcvFrame(wxFrame *frame, const wxString& title);
        ~wxcvFrame();
    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout
        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};


#endif // WXCVMAIN_H
