/***************************************************************
 * Name:      wxtestApp.h
 * Purpose:   Defines Application Class
 * Author:    yan
 * Created:   2017-11-02
 * Copyright: yan (http://cool-emerald.blogspot.sg/)
 * License:
 **************************************************************/

#ifndef WXTESTAPP_H
#define WXTESTAPP_H

#include <wx/app.h>

class wxtestApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // WXTESTAPP_H
