/***************************************************************
 * Name:      wxtestMain.h
 * Purpose:   Defines Application Frame
 * Author:    yan
 * Created:   2017-11-02
 * Copyright: yan (http://cool-emerald.blogspot.sg/)
 * License:
 **************************************************************/

#ifndef WXTESTMAIN_H
#define WXTESTMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wxtestApp.h"

class wxtestFrame: public wxFrame
{
    public:
        wxtestFrame(wxFrame *frame, const wxString& title);
        ~wxtestFrame();
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


#endif // WXTESTMAIN_H
