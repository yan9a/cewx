/***************************************************************
 * Name:      wxtestApp.cpp
 * Purpose:   Code for Application Class
 * Author:    yan
 * Created:   2017-11-02
 * Copyright: yan (http://cool-emerald.blogspot.sg/)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "wxtestApp.h"
#include "wxtestMain.h"

IMPLEMENT_APP(wxtestApp);

bool wxtestApp::OnInit()
{
    wxtestFrame* frame = new wxtestFrame(0L, _("wxWidgets Application Template"));

    frame->Show();

    return true;
}
