//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : configurationtoolbase.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef __configurationtoolbase__
#define __configurationtoolbase__

#include "codelite_exports.h"

#include <wx/xml/xml.h>

class SerializedObject;

class WXDLLIMPEXP_SDK ConfigurationToolBase
{
protected:
    wxXmlDocument m_doc;
    wxString m_fileName;

public:
    ConfigurationToolBase();
    virtual ~ConfigurationToolBase();

    bool Load(const wxString& filename);
    bool WriteObject(const wxString& name, SerializedObject* obj);
    bool ReadObject(const wxString& name, SerializedObject* obj);

    virtual wxString GetRootName() = 0;
};
#endif // __configurationtoolbase__
