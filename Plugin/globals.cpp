//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : globals.cpp
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
#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "clConsoleBase.h"
#include "clDataViewListCtrl.h"
#include "clFileSystemWorkspace.hpp"
#include "clGetTextFromUserDialog.h"
#include "cl_standard_paths.h"
#include "cpp_scanner.h"
#include "ctags_manager.h"
#include "debugger.h"
#include "debuggermanager.h"
#include "dirtraverser.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macromanager.h"
#include "macros.h"
#include "precompiled_header.h"
#include "procutils.h"
#include "project.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include "wx/app.h"
#include "wx/ffile.h"
#include "wx/listctrl.h"
#include "wx/tokenzr.h"
#include "wx/window.h"
#include "wxmd5.h"
#include <algorithm>
#include <set>
#include <wx/app.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/dataview.h>
#include <wx/dcmemory.h>
#include <wx/dcscreen.h>
#include <wx/dir.h>
#include <wx/display.h>
#include <wx/filename.h>
#include <wx/fontmap.h>
#include <wx/graphics.h>
#include <wx/icon.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/richmsgdlg.h>
#include <wx/settings.h>
#include <wx/sstream.h>
#include <wx/stc/stc.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/xrc/xmlres.h>
#include <wx/zipstrm.h>

#ifdef __WXMSW__
#include <UxTheme.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#ifdef __WXGTK20__
#include <gtk/gtk.h>
#endif

const wxEventType wxEVT_COMMAND_CL_INTERNAL_0_ARGS = ::wxNewEventType();
const wxEventType wxEVT_COMMAND_CL_INTERNAL_1_ARGS = ::wxNewEventType();

#ifdef __WXMSW__

typedef BOOL(WINAPI* ADMFA)(BOOL allow);                    // AllowDarkModeForApp
typedef BOOL(WINAPI* ADMFW)(HWND window, BOOL allow);       // AllowDarkModeForWindow
typedef void(WINAPI* FMT)();                                // FlushMenuThemes
typedef HRESULT(WINAPI* DSWA)(HWND, DWORD, LPCVOID, DWORD); // DwmSetWindowAttribute

BOOL CALLBACK DarkExplorerChildProc(HWND hwnd, LPARAM lparam)
{
    if(!IsWindow(hwnd))
        return TRUE;

    const BOOL is_darktheme = (BOOL)lparam;
    static const HMODULE huxtheme = GetModuleHandle(L"uxtheme.dll");

    if(huxtheme) {
        SetWindowTheme(hwnd, is_darktheme ? L"DarkMode_Explorer" : L"Explorer", NULL);
        static const ADMFW _AllowDarkModeForWindow = (ADMFW)GetProcAddress(huxtheme, MAKEINTRESOURCEA(133));
        if(_AllowDarkModeForWindow)
            _AllowDarkModeForWindow(hwnd, is_darktheme);
    }
    InvalidateRect(hwnd, nullptr, TRUE);
    return TRUE;
}

void MSWSetWindowDarkTheme(wxWindow* win)
{
    bool b = ColoursAndFontsManager::Get().IsDarkTheme();
    static const HMODULE huxtheme = GetModuleHandle(L"uxtheme.dll");
    if(huxtheme) {
        SetWindowTheme(win->GetHandle(), b ? L"DarkMode_Explorer" : L"Explorer", NULL);
        static const ADMFA _AllowDarkModeForApp = (ADMFA)GetProcAddress(huxtheme, MAKEINTRESOURCEA(135));
        static const ADMFW _AllowDarkModeForWindow = (ADMFW)GetProcAddress(huxtheme, MAKEINTRESOURCEA(133));
        if(_AllowDarkModeForApp && _AllowDarkModeForWindow) {
            _AllowDarkModeForApp(b);
            _AllowDarkModeForWindow(win->GetHandle(), b);
            EnumChildWindows(win->GetHandle(), &DarkExplorerChildProc, b);

            const FMT _FlushMenuThemes = (FMT)GetProcAddress(huxtheme, MAKEINTRESOURCEA(136));

            if(_FlushMenuThemes)
                _FlushMenuThemes();
            InvalidateRect(win->GetHandle(), nullptr, FALSE); // HACK
        }
    }
}

#else
void MSWSetWindowDarkTheme(wxWindow* win)
{
    wxUnusedVar(win);
}
#endif

// --------------------------------------------------------
// Internal handler to handle queuing requests...
// --------------------------------------------------------
class clInternalEventHandlerData : public wxClientData
{
    wxObject* m_this;
    clEventFunc_t m_funcPtr;
    wxClientData* m_arg;

public:
    clInternalEventHandlerData(wxObject* instance, clEventFunc_t func, wxClientData* arg)
        : m_this(instance)
        , m_funcPtr(func)
        , m_arg(arg)
    {
    }

    clInternalEventHandlerData(wxObject* instance, clEventFunc_t func)
        : m_this(instance)
        , m_funcPtr(func)
        , m_arg(NULL)
    {
    }

    virtual ~clInternalEventHandlerData() { wxDELETE(m_arg); }

    wxClientData* GetArg() const { return m_arg; }
    clEventFunc_t GetFuncPtr() const { return m_funcPtr; }
    wxObject* GetThis() { return m_this; }
};

class clInternalEventHandler : public wxEvtHandler
{
public:
    clInternalEventHandler()
    {
        EventNotifier::Get()->Connect(wxEVT_COMMAND_CL_INTERNAL_0_ARGS,
                                      wxCommandEventHandler(clInternalEventHandler::OnInternalEvent0), NULL, this);
        EventNotifier::Get()->Connect(wxEVT_COMMAND_CL_INTERNAL_1_ARGS,
                                      wxCommandEventHandler(clInternalEventHandler::OnInternalEvent1), NULL, this);
    }

    virtual ~clInternalEventHandler()
    {
        EventNotifier::Get()->Disconnect(wxEVT_COMMAND_CL_INTERNAL_0_ARGS,
                                         wxCommandEventHandler(clInternalEventHandler::OnInternalEvent0), NULL, this);
        EventNotifier::Get()->Disconnect(wxEVT_COMMAND_CL_INTERNAL_1_ARGS,
                                         wxCommandEventHandler(clInternalEventHandler::OnInternalEvent1), NULL, this);
    }

    /**
     * @brief Call 1 arguments function
     */
    void OnInternalEvent1(wxCommandEvent& e)
    {
        clInternalEventHandlerData* cd = reinterpret_cast<clInternalEventHandlerData*>(e.GetClientObject());
        if(cd) {
            wxObject* obj = cd->GetThis();
            wxClientData* arg = cd->GetArg();
            clEventFunc_t func = cd->GetFuncPtr();
            (obj->*func)(arg);

            delete cd;
            e.SetClientObject(NULL);
        }
    }

    /**
     * @brief Call 0 arguments function
     */
    void OnInternalEvent0(wxCommandEvent& e)
    {
        clInternalEventHandlerData* cd = reinterpret_cast<clInternalEventHandlerData*>(e.GetClientObject());
        if(cd) {
            wxObject* obj = cd->GetThis();
            clEventFunc_t func = cd->GetFuncPtr();
            (obj->*func)(NULL);

            delete cd;
            e.SetClientObject(NULL);
        }
    }
};

// construct a global handler here
clInternalEventHandler clEventHandlerHelper;

// --------------------------------------------------------
// Internal handler to handle queuing requests... end
// --------------------------------------------------------

static wxString DoExpandAllVariables(const wxString& expression, clCxxWorkspace* workspace, const wxString& projectName,
                                     const wxString& confToBuild, const wxString& fileName);

#ifdef __WXMAC__
#include <mach-o/dyld.h>

// On Mac we determine the base path using system call
//_NSGetExecutablePath(path, &path_len);
static wxString MacGetInstallPath()
{
    char path[257];
    uint32_t path_len = 256;
    _NSGetExecutablePath(path, &path_len);

    // path now contains
    // CodeLite.app/Contents/MacOS/
    wxFileName fname(wxString(path, wxConvUTF8));

    // remove he MacOS part of the exe path
    wxString file_name = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    wxString rest;
    file_name.EndsWith(wxT("MacOS/"), &rest);
    rest.Append(wxT("SharedSupport/"));

    return rest;
}
#endif

#if defined(__WXGTK__)
#include <dirent.h>
#include <unistd.h>
#endif

struct ProjListCompartor {
    bool operator()(const ProjectPtr p1, const ProjectPtr p2) const { return p1->GetName() > p2->GetName(); }
};

static bool IsBOMFile(const char* file_name)
{
    bool res(false);
    FILE* fp = fopen(file_name, "rb");
    if(fp) {
        struct stat buff;
        if(stat(file_name, &buff) == 0) {

            // Read the first 4 bytes (or less)
            size_t size = buff.st_size;
            if(size > 4)
                size = 4;

            char* buffer = new char[size];
            if(fread(buffer, sizeof(char), size, fp) == size) {
                BOM bom(buffer, size);
                res = (bom.Encoding() != wxFONTENCODING_SYSTEM);
            }
            delete[] buffer;
        }
        fclose(fp);
    }
    return res;
}

static bool ReadBOMFile(const char* file_name, wxString& content, BOM& bom)
{
    content.Empty();

    FILE* fp = fopen(file_name, "rb");
    if(fp) {
        struct stat buff;
        if(stat(file_name, &buff) == 0) {
            size_t size = buff.st_size;
            char* buffer = new char[size + 1];
            if(fread(buffer, sizeof(char), size, fp) == size) {
                buffer[size] = 0;

                wxFontEncoding encoding(wxFONTENCODING_SYSTEM);
                size_t bomSize(size);

                if(bomSize > 4)
                    bomSize = 4;
                bom.SetData(buffer, bomSize);
                encoding = bom.Encoding();

                if(encoding != wxFONTENCODING_SYSTEM) {
                    wxCSConv conv(encoding);
                    // Skip the BOM
                    char* ptr = buffer;
                    ptr += bom.Len();
                    content = wxString(ptr, conv);

                    if(content.IsEmpty()) {
                        content = wxString::From8BitData(ptr);
                    }
                }
            }
            delete[] buffer;
        }
        fclose(fp);
    } // From8BitData
    return content.IsEmpty() == false;
}

static bool ReadFile8BitData(const char* file_name, wxString& content)
{
    content.Empty();

    FILE* fp = fopen(file_name, "rb");
    if(fp) {
        struct stat buff;
        if(stat(file_name, &buff) == 0) {
            size_t size = buff.st_size;
            char* buffer = new char[size + 1];
            if(fread(buffer, sizeof(char), size, fp) == size) {
                buffer[size] = 0;
                content = wxString::From8BitData(buffer);
            }
            delete[] buffer;
        }
        fclose(fp);
    }
    return content.IsEmpty() == false;
}

bool SendCmdEvent(int eventId, void* clientData) { return EventNotifier::Get()->SendCommandEvent(eventId, clientData); }

bool SendCmdEvent(int eventId, void* clientData, const wxString& str)
{
    return EventNotifier::Get()->SendCommandEvent(eventId, clientData, str);
}

void PostCmdEvent(int eventId, void* clientData) { EventNotifier::Get()->PostCommandEvent(eventId, clientData); }

void SetColumnText(wxListCtrl* list, long indx, long column, const wxString& rText, int imgId)
{
    wxListItem list_item;
    list_item.SetId(indx);
    list_item.SetColumn(column);
    list_item.SetMask(wxLIST_MASK_TEXT);
    list_item.SetText(rText);
    list_item.SetImage(imgId);
    list->SetItem(list_item);
}

wxString GetColumnText(wxListCtrl* list, long index, long column)
{
    wxListItem list_item;
    list_item.SetId(index);
    list_item.SetColumn(column);
    list_item.SetMask(wxLIST_MASK_TEXT);
    list->GetItem(list_item);
    return list_item.GetText();
}

bool ReadFileWithConversion(const wxString& fileName, wxString& content, wxFontEncoding encoding, BOM* bom)
{
    wxLogNull noLog;
    content.Clear();
    wxFile file(fileName, wxFile::read);

    const wxCharBuffer name = _C(fileName);
    if(file.IsOpened()) {

        // If we got a BOM pointer, test to see whether the file is BOM file
        if(bom && IsBOMFile(name.data())) {
            return ReadBOMFile(name.data(), content, *bom);
        }

        if(encoding == wxFONTENCODING_DEFAULT) {
            encoding = EditorConfigST::Get()->GetOptions()->GetFileFontEncoding();
        }

        // first try the user defined encoding (except for UTF8: the UTF8 builtin appears to be faster)
        if(encoding != wxFONTENCODING_UTF8) {
            wxCSConv fontEncConv(encoding);
            if(fontEncConv.IsOk()) {
                file.ReadAll(&content, fontEncConv);
            }
        }

        if(content.IsEmpty()) {
            // now try the Utf8
            file.ReadAll(&content, wxConvUTF8);
            if(content.IsEmpty()) {
                // try local 8 bit data
                ReadFile8BitData(name.data(), content);
            }
        }
    }
    return !content.IsEmpty();
}

bool RemoveDirectory(const wxString& path)
{
    wxString cmd;
    if(wxGetOsVersion() & wxOS_WINDOWS) {
        // any of the windows variants
        cmd << wxT("rmdir /S /Q ") << wxT("\"") << path << wxT("\"");
    } else {
        cmd << wxT("\rm -fr ") << wxT("\"") << path << wxT("\"");
    }
    return wxShell(cmd);
}

bool IsValidCppIndetifier(const wxString& id)
{
    if(id.IsEmpty()) {
        return false;
    }
    // first char can be only _A-Za-z
    wxString first(id.Mid(0, 1));
    if(first.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")) != wxString::npos) {
        return false;
    }
    // make sure that rest of the id contains only a-zA-Z0-9_
    if(id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
        return false;
    }
    return true;
}

long AppendListCtrlRow(wxListCtrl* list)
{
    long item;
    list->GetItemCount() ? item = list->GetItemCount() : item = 0;

    wxListItem info;
    // Set the item display name
    info.SetColumn(0);
    info.SetId(item);
    item = list->InsertItem(info);
    return item;
}

bool IsValidCppFile(const wxString& id)
{
    if(id.IsEmpty()) {
        return false;
    }

    // make sure that rest of the id contains only a-zA-Z0-9_
    if(id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
        return false;
    }
    return true;
}

wxString ExpandVariables(const wxString& expression, ProjectPtr proj, IEditor* editor, const wxString& filename)
{
    wxString project_name(proj->GetName());
    wxString file = filename;
    if(file.IsEmpty() && editor) {
        file = editor->GetFileName().GetFullPath();
    }
    return ExpandAllVariables(expression, clCxxWorkspaceST::Get(), project_name, wxEmptyString, file);
}

// This functions accepts expression and expand all variables in it
wxString ExpandAllVariables(const wxString& expression, clCxxWorkspace* workspace, const wxString& projectName,
                            const wxString& selConf, const wxString& fileName)
{
    // add support for backticks commands
    wxString tmpExp;
    wxString noBackticksExpression;
    for(size_t i = 0; i < expression.Length(); i++) {
        if(expression.GetChar(i) == wxT('`')) {
            // found a backtick, loop over until we found the closing backtick
            wxString backtick;
            bool found(false);
            i++;
            for(; i < expression.Length(); i++) {
                if(expression.GetChar(i) == wxT('`')) {
                    found = true;
                    i++;
                    break;
                }
                backtick << expression.GetChar(i);
            }

            if(!found) {
                // dont replace anything
                clLogMessage(wxT("Syntax error in expression: ") + expression + wxT(": expecting '`'"));
                return expression;
            } else {
                // expand the backtick statement
                wxString expandedBacktick = DoExpandAllVariables(backtick, workspace, projectName, selConf, fileName);

                // execute the backtick
                wxArrayString output;
                ProcUtils::SafeExecuteCommand(expandedBacktick, output);

                // concatenate the array into sAssign To:pace delimited string
                backtick.Clear();
                for(size_t xx = 0; xx < output.GetCount(); xx++) {
                    backtick << output.Item(xx).Trim().Trim(false) << wxT(" ");
                }

                // and finally concatente the result of the backtick command back to the expression
                tmpExp << backtick;
            }
        } else {
            tmpExp << expression.GetChar(i);
        }
    }

    return DoExpandAllVariables(tmpExp, workspace, projectName, selConf, fileName);
}

wxString DoExpandAllVariables(const wxString& expression, clCxxWorkspace* workspace, const wxString& projectName,
                              const wxString& confToBuild, const wxString& fileName)
{
    wxString errMsg;
    wxString output(expression);

    size_t retries = 0;
    wxString dummyname, dummfullname;
    while((retries < 5) && MacroManager::Instance()->FindVariable(output, dummyname, dummyname)) {
        ++retries;
        DollarEscaper de(output);
        if(workspace) {
            output.Replace(wxT("$(WorkspaceName)"), workspace->GetName());
            ProjectPtr proj = workspace->FindProjectByName(projectName, errMsg);
            if(proj) {
                wxString project_name(proj->GetName());

                // make sure that the project name does not contain any spaces
                project_name.Replace(wxT(" "), wxT("_"));

                BuildConfigPtr bldConf = workspace->GetProjBuildConf(proj->GetName(), confToBuild);
                output.Replace(wxT("$(ProjectPath)"), proj->GetFileName().GetPath());
                output.Replace(wxT("$(WorkspacePath)"), workspace->GetWorkspaceFileName().GetPath());
                output.Replace(wxT("$(ProjectName)"), project_name);

                if(bldConf) {
                    output.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());
                    output.Replace("$(OutputFile)", bldConf->GetOutputFileName());

                    // the IntermediateDirectory variable is special, since it can contains
                    // other variables in it.
                    wxString id(bldConf->GetIntermediateDirectory());

                    // Substitute all macros from $(IntermediateDirectory)
                    id.Replace(wxT("$(ProjectPath)"), proj->GetFileName().GetPath());
                    id.Replace(wxT("$(WorkspacePath)"), workspace->GetWorkspaceFileName().GetPath());
                    id.Replace(wxT("$(ProjectName)"), project_name);
                    id.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());

                    output.Replace(wxT("$(IntermediateDirectory)"), id);
                    output.Replace(wxT("$(OutDir)"), id);

                    // Compiler-related variables

                    wxString cFlags = bldConf->GetCCompileOptions();
                    cFlags.Replace(wxT(";"), wxT(" "));
                    output.Replace(wxT("$(CC)"), bldConf->GetCompiler()->GetTool("CC"));
                    output.Replace(wxT("$(CFLAGS)"), cFlags);

                    wxString cxxFlags = bldConf->GetCompileOptions();
                    cxxFlags.Replace(wxT(";"), wxT(" "));
                    output.Replace(wxT("$(CXX)"), bldConf->GetCompiler()->GetTool("CXX"));
                    output.Replace(wxT("$(CXXFLAGS)"), cxxFlags);

                    wxString ldFlags = bldConf->GetLinkOptions();
                    ldFlags.Replace(wxT(";"), wxT(" "));
                    output.Replace(wxT("$(LDFLAGS)"), ldFlags);

                    wxString asFlags = bldConf->GetAssmeblerOptions();
                    asFlags.Replace(wxT(";"), wxT(" "));
                    output.Replace(wxT("$(AS)"), bldConf->GetCompiler()->GetTool("AS"));
                    output.Replace(wxT("$(ASFLAGS)"), asFlags);

                    wxString resFlags = bldConf->GetResCompileOptions();
                    resFlags.Replace(wxT(";"), wxT(" "));
                    output.Replace(wxT("$(RES)"), bldConf->GetCompiler()->GetTool("ResourceCompiler"));
                    output.Replace(wxT("$(RESFLAGS)"), resFlags);

                    output.Replace(wxT("$(AR)"), bldConf->GetCompiler()->GetTool("AR"));

                    output.Replace(wxT("$(MAKE)"), bldConf->GetCompiler()->GetTool("MAKE"));

                    output.Replace(wxT("$(IncludePath)"), bldConf->GetIncludePath());
                    output.Replace(wxT("$(LibraryPath)"), bldConf->GetLibPath());
                    output.Replace(wxT("$(ResourcePath)"), bldConf->GetResCmpIncludePath());
                    output.Replace(wxT("$(LinkLibraries)"), bldConf->GetLibraries());
                }

                if(output.Find(wxT("$(ProjectFiles)")) != wxNOT_FOUND)
                    output.Replace(wxT("$(ProjectFiles)"), proj->GetFilesAsString(false));

                if(output.Find(wxT("$(ProjectFilesAbs)")) != wxNOT_FOUND)
                    output.Replace(wxT("$(ProjectFilesAbs)"), proj->GetFilesAsString(true));
            }
        }

        if(fileName.IsEmpty() == false) {
            wxFileName fn(fileName);

            output.Replace(wxT("$(CurrentFileName)"), fn.GetName());

            wxString fpath(fn.GetPath());
            fpath.Replace(wxT("\\"), wxT("/"));
            output.Replace(wxT("$(CurrentFilePath)"), fpath);
            output.Replace(wxT("$(CurrentFileExt)"), fn.GetExt());

            wxString ffullpath(fn.GetFullPath());
            ffullpath.Replace(wxT("\\"), wxT("/"));
            output.Replace(wxT("$(CurrentFileFullPath)"), ffullpath);
            output.Replace(wxT("$(CurrentFileFullName)"), fn.GetFullName());
        }

        // exapnd common macros
        wxDateTime now = wxDateTime::Now();
        output.Replace(wxT("$(User)"), wxGetUserName());
        output.Replace(wxT("$(Date)"), now.FormatDate());

        if(workspace) {
            output.Replace(wxT("$(CodeLitePath)"), workspace->GetStartupDir());
        }

        // call the environment & workspace variables expand function
        output = EnvironmentConfig::Instance()->ExpandVariables(output, true);
    }
    return output;
}

bool WriteFileUTF8(const wxString& fileName, const wxString& content)
{
    wxFFile file(fileName, wxT("w+b"));
    if(!file.IsOpened()) {
        return false;
    }

    // first try the Utf8
    return file.Write(content, wxConvUTF8);
}

bool CompareFileWithString(const wxString& filePath, const wxString& str)
{
    wxString content;
    if(!ReadFileWithConversion(filePath, content)) {
        return false;
    }

    wxString diskMD5 = wxMD5::GetDigest(content);
    wxString mem_MD5 = wxMD5::GetDigest(str);
    return diskMD5 == mem_MD5;
}

bool CopyDir(const wxString& src, const wxString& target)
{
    wxString SLASH = wxFileName::GetPathSeparator();

    wxString from(src);
    wxString to(target);

    // append a slash if there is not one (for easier parsing)
    // because who knows what people will pass to the function.
    if(to.EndsWith(SLASH) == false) {
        to << SLASH;
    }

    // for both dirs
    if(from.EndsWith(SLASH) == false) {
        from << SLASH;
    }

    // first make sure that the source dir exists
    if(!wxDir::Exists(from)) {
        Mkdir(from);
        return false;
    }

    if(!wxDir::Exists(to)) {
        Mkdir(to);
    }

    wxDir dir(from);
    wxString filename;
    bool bla = dir.GetFirst(&filename);
    if(bla) {
        do {
            if(wxDirExists(from + filename)) {
                Mkdir(to + filename);
                CopyDir(from + filename, to + filename);
            } else {
                // change the umask for files only
                wxCopyFile(from + filename, to + filename);
            }
        } while(dir.GetNext(&filename));
    }
    return true;
}

void Mkdir(const wxString& path)
{
#ifdef __WXMSW__
    wxMkDir(path.GetData());
#else
    wxMkDir(path.ToAscii(), 0777);
#endif
}

bool WriteFileWithBackup(const wxString& file_name, const wxString& content, bool backup)
{
    if(backup) {
        wxString backup_name(file_name);
        backup_name << wxT(".bak");
        if(!wxCopyFile(file_name, backup_name, true)) {
            clLogMessage(wxString::Format(wxT("Failed to backup file %s, skipping it"), file_name.c_str()));
            return false;
        }
    }

    wxFFile file(file_name, wxT("wb"));
    if(file.IsOpened() == false) {
        // Nothing to be done
        wxString msg = wxString::Format(wxT("Failed to open file %s"), file_name.c_str());
        clLogMessage(msg);
        return false;
    }

    // write the new content
    wxCSConv fontEncConv(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
    file.Write(content, fontEncConv); // JK was without conversion
    file.Close();
    return true;
}

bool CopyToClipboard(const wxString& text)
{
    bool ret(true);

#if wxUSE_CLIPBOARD
    if(wxTheClipboard->Open()) {
        wxTheClipboard->UsePrimarySelection(false);
        if(!wxTheClipboard->SetData(new wxTextDataObject(text))) {
            ret = false;
        }
        wxTheClipboard->Close();
    } else {
        ret = false;
    }
#else // wxUSE_CLIPBOARD
    ret = false;
#endif
    return ret;
}

wxColour MakeColourLighter(wxColour color, float level) { return DrawingUtils::LightColour(color, level); }

bool IsFileReadOnly(const wxFileName& filename)
{
#ifdef __WXMSW__
    DWORD dwAttrs = GetFileAttributes(filename.GetFullPath().c_str());
    if(dwAttrs != INVALID_FILE_ATTRIBUTES && (dwAttrs & FILE_ATTRIBUTE_READONLY)) {
        return true;
    } else {
        return false;
    }
#else
    // try to open the file with 'write permission'
    return !filename.IsFileWritable();
#endif
}

void FillFromSmiColonString(wxArrayString& arr, const wxString& str)
{
    arr.clear();
    wxStringTokenizer tkz(str, wxT(";"));
    while(tkz.HasMoreTokens()) {

        wxString token = tkz.NextToken();
        token.Trim().Trim(false);
        if(token.IsEmpty()) {
            continue;
        }
        arr.Add(token.Trim());
    }
}

wxString ArrayToSmiColonString(const wxArrayString& array)
{
    wxString result;
    for(size_t i = 0; i < array.GetCount(); i++) {
        wxString tmp = NormalizePath(array.Item(i));
        tmp.Trim().Trim(false);
        if(tmp.IsEmpty() == false) {
            result += NormalizePath(array.Item(i));
            result += wxT(";");
        }
    }
    return result.BeforeLast(wxT(';'));
}

void StripSemiColons(wxString& str) { str.Replace(wxT(";"), wxT(" ")); }

wxString NormalizePath(const wxString& path)
{
    wxString normalized_path(path);
    normalized_path.Trim().Trim(false);
    normalized_path.Replace(wxT("\\"), wxT("/"));
    while(normalized_path.Replace("//", "/")) {}
    return normalized_path;
}

time_t GetFileModificationTime(const wxFileName& filename) { return GetFileModificationTime(filename.GetFullPath()); }

time_t GetFileModificationTime(const wxString& filename)
{
    struct stat buff;
    const wxCharBuffer cname = _C(filename);
    if(stat(cname.data(), &buff) < 0) {
        return 0;
    }
    return buff.st_mtime;
}

bool clIsMSYSEnvironment()
{
#ifdef __WXMSW__
    static bool isMSYS = false;
    static bool firstTime = true;

    if(firstTime) {
        firstTime = false;
        CL_DEBUG("Testing for MSYS environment...uname -a");
        wxString out = ProcUtils::SafeExecuteCommand("uname -a");
        CL_DEBUG("[%s]", out);
        if(out.IsEmpty()) {
            isMSYS = false;
        } else {
            out.MakeLower();
            isMSYS = out.Contains("mingw") || out.Contains("msys");
        }
    }
    return isMSYS;
#else
    return false;
#endif
}

bool clIsCygwinEnvironment()
{
#ifdef __WXMSW__
    static bool isCygwin = false;
    static bool firstTime = true;

    if(firstTime) {
        firstTime = false;
        CL_DEBUG("Testing for CYGWIN environment...uname -s");
        wxString out = ProcUtils::SafeExecuteCommand("uname -s");
        CL_DEBUG("[%s]", out);
        if(out.IsEmpty()) {
            isCygwin = false;
        } else {
            isCygwin = out.StartsWith("CYGWIN_NT");
        }
    }
    return isCygwin;
#else
    return false;
#endif
}

void WrapInShell(wxString& cmd)
{
    wxString command;
#ifdef __WXMSW__
    wxChar* shell = wxGetenv(wxT("COMSPEC"));
    if(!shell)
        shell = (wxChar*)wxT("CMD.EXE");
    command << shell << wxT(" /C ");
    if(cmd.StartsWith("\"") && !cmd.EndsWith("\"")) {
        command << "\"" << cmd << "\"";
    } else {
        command << cmd;
    }
    cmd = command;
#else
    command << wxT("/bin/sh -c '");
    // escape any single quoutes
    cmd.Replace("'", "\\'");
    command << cmd << wxT("'");
    cmd = command;
#endif
}

wxString clGetUserName()
{
    wxString squashedname, name = wxGetUserName();

    // The wx doc says that 'name' may now be e.g. "Mr. John Smith"
    // So try to make it more suitable to be an extension
    name.MakeLower();
    name.Replace(wxT(" "), wxT("_"));
    for(size_t i = 0; i < name.Len(); ++i) {
        wxChar ch = name.GetChar(i);
        if((ch < wxT('a') || ch > wxT('z')) && ch != wxT('_')) {
            // Non [a-z_] character: skip it
        } else {
            squashedname << ch;
        }
    }

    return (squashedname.IsEmpty() ? wxString(wxT("someone")) : squashedname);
}

static void DoReadProjectTemplatesFromFolder(const wxString& folder, std::list<ProjectPtr>& list,
                                             bool loadDefaults = true)
{
    // read all files under this directory
    wxArrayString files;
    if(wxFileName::DirExists(folder)) {
        DirTraverser dt("*.project");

        wxDir dir(folder);
        dir.Traverse(dt);

        files = dt.GetFiles();
        if(files.GetCount() > 0) {
            for(size_t i = 0; i < files.GetCount(); i++) {
                ProjectPtr proj(new Project());
                if(!proj->Load(files.Item(i))) {
                    // corrupted xml file?
                    clLogMessage(wxT("Failed to load template project: ") + files.Item(i) + wxT(" (corrupted XML?)"));
                    continue;
                }
                list.push_back(proj);

                // load template icon
                wxFileName fn(files.Item(i));
                fn.SetFullName("icon.png");
                if(fn.Exists()) {
                    wxBitmap bmp = wxBitmap(fn.GetFullPath(), wxBITMAP_TYPE_ANY);
                    if(bmp.IsOk() && bmp.GetWidth() == 16 && bmp.GetHeight() == 16) {
                        proj->SetIconPath(fn.GetFullPath());
                    }
                }
            }
        }
    }

    if(loadDefaults && files.IsEmpty()) {
        // if we ended up here, it means the installation got screwed up since
        // there should be at least 8 project templates !
        // create 3 default empty projects
        ProjectPtr exeProj(new Project());
        ProjectPtr libProj(new Project());
        ProjectPtr dllProj(new Project());
        libProj->Create(wxT("Static Library"), wxEmptyString, folder, PROJECT_TYPE_STATIC_LIBRARY);
        dllProj->Create(wxT("Dynamic Library"), wxEmptyString, folder, PROJECT_TYPE_DYNAMIC_LIBRARY);
        exeProj->Create(wxT("Executable"), wxEmptyString, folder, PROJECT_TYPE_EXECUTABLE);
        list.push_back(libProj);
        list.push_back(dllProj);
        list.push_back(exeProj);
    }
}

void GetProjectTemplateList(std::list<ProjectPtr>& list)
{
    DoReadProjectTemplatesFromFolder(clStandardPaths::Get().GetProjectTemplatesDir(), list);
    DoReadProjectTemplatesFromFolder(clStandardPaths::Get().GetUserProjectTemplatesDir(), list, false);
    list.sort(ProjListCompartor());
}

bool IsCppKeyword(const wxString& word)
{
    static std::set<wxString> words;

    if(words.empty()) {
        words.insert(wxT("auto"));
        words.insert(wxT("break"));
        words.insert(wxT("case"));
        words.insert(wxT("char"));
        words.insert(wxT("const"));
        words.insert(wxT("continue"));
        words.insert(wxT("default"));
        words.insert(wxT("define"));
        words.insert(wxT("defined"));
        words.insert(wxT("do"));
        words.insert(wxT("double"));
        words.insert(wxT("elif"));
        words.insert(wxT("else"));
        words.insert(wxT("endif"));
        words.insert(wxT("enum"));
        words.insert(wxT("error"));
        words.insert(wxT("extern"));
        words.insert(wxT("float"));
        words.insert(wxT("for"));
        words.insert(wxT("goto"));
        words.insert(wxT("if"));
        words.insert(wxT("ifdef"));
        words.insert(wxT("ifndef"));
        words.insert(wxT("include"));
        words.insert(wxT("int"));
        words.insert(wxT("line"));
        words.insert(wxT("long"));
        words.insert(wxT("bool"));
        words.insert(wxT("pragma"));
        words.insert(wxT("register"));
        words.insert(wxT("return"));
        words.insert(wxT("short"));
        words.insert(wxT("signed"));
        words.insert(wxT("sizeof"));
        words.insert(wxT("static"));
        words.insert(wxT("struct"));
        words.insert(wxT("switch"));
        words.insert(wxT("typedef"));
        words.insert(wxT("undef"));
        words.insert(wxT("union"));
        words.insert(wxT("unsigned"));
        words.insert(wxT("void"));
        words.insert(wxT("volatile"));
        words.insert(wxT("while"));
        words.insert(wxT("class"));
        words.insert(wxT("namespace"));
        words.insert(wxT("delete"));
        words.insert(wxT("friend"));
        words.insert(wxT("inline"));
        words.insert(wxT("new"));
        words.insert(wxT("operator"));
        words.insert(wxT("overload"));
        words.insert(wxT("protected"));
        words.insert(wxT("private"));
        words.insert(wxT("public"));
        words.insert(wxT("this"));
        words.insert(wxT("virtual"));
        words.insert(wxT("template"));
        words.insert(wxT("typename"));
        words.insert(wxT("dynamic_cast"));
        words.insert(wxT("static_cast"));
        words.insert(wxT("const_cast"));
        words.insert(wxT("reinterpret_cast"));
        words.insert(wxT("using"));
        words.insert(wxT("throw"));
        words.insert(wxT("catch"));
        words.insert(wxT("nullptr"));
    }

    return words.count(word) != 0;
}

void MSWSetNativeTheme(wxWindow* win, const wxString& theme)
{
#if defined(__WXMSW__) && defined(_WIN64)
    SetWindowTheme((HWND)win->GetHWND(), theme.c_str(), NULL);
#endif
}

void StringManager::AddStrings(size_t size, const wxString* strings, const wxString& current,
                               wxControlWithItems* control)
{
    m_size = size;
    m_unlocalisedStringArray = wxArrayString(size, strings);
    p_control = control;
    p_control->Clear();

    // Add each item to the control, localising as we go
    for(size_t n = 0; n < size; ++n) {
        p_control->Append(wxGetTranslation(strings[n]));
    }

    // Select in the control the currently used string
    SetStringSelection(current);
}

wxString StringManager::GetStringSelection() const
{
    wxString selection;
    // Find which localised string was selected
    int sel = p_control->GetSelection();
    if(sel != wxNOT_FOUND) {
        selection = m_unlocalisedStringArray.Item(sel);
    }

    return selection;
}

void StringManager::SetStringSelection(const wxString& str, size_t dfault /*= 0*/)
{
    if(str.IsEmpty() || m_size == 0) {
        return;
    }
    int sel = m_unlocalisedStringArray.Index(str);
    if(sel != wxNOT_FOUND) {
        p_control->SetSelection(sel);
    } else {
        if(dfault < m_size) {
            p_control->SetSelection(dfault);
        } else {
            p_control->SetSelection(0);
        }
    }
}

wxArrayString ReturnWithStringPrepended(const wxArrayString& oldarray, const wxString& str, const size_t maxsize)
{
    wxArrayString array(oldarray);
    if(!str.empty()) {
        // Remove any pre-existing identical entry
        // This avoids duplication, and allows us to prepend the current string
        // As a result, the array will be suitable for 'recently-used-strings' situations
        int index = array.Index(str);
        if(index != wxNOT_FOUND) {
            array.RemoveAt(index);
        }
        array.Insert(str, 0);
    }

    // Truncate the array if needed
    if(maxsize) {
        while(array.GetCount() > maxsize) {
            array.RemoveAt(array.GetCount() - 1);
        }
    }
    return array;
}

// Make absolute first, including abolishing any symlinks (Normalise only does MSW shortcuts)
// Then only 'make relative' if it's a subpath of reference_path (or reference_path itself)
bool MakeRelativeIfSensible(wxFileName& fn, const wxString& reference_path)
{
    if(reference_path.IsEmpty() || !fn.IsOk()) {
        return false;
    }

#if defined(__WXGTK__)
    // Normalize() doesn't account for symlinks in wxGTK
    wxStructStat statstruct;
    int error = wxLstat(fn.GetFullPath(), &statstruct);

    if(!error && S_ISLNK(statstruct.st_mode)) { // If it's a symlink
        char buf[4096];
        int len = readlink(fn.GetFullPath().mb_str(wxConvUTF8), buf, WXSIZEOF(buf) - sizeof(char));
        if(len != -1) {
            buf[len] = '\0'; // readlink() doesn't NULL-terminate the buffer
            fn.Assign(wxString(buf, wxConvUTF8, len));
        }
    }
#endif

    fn.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE | wxPATH_NORM_SHORTCUT);

    // Now see if fn is in or under 'reference_path'
    wxString fnPath = fn.GetPath();
    if((fnPath.Len() >= reference_path.Len()) && (fnPath.compare(0, reference_path.Len(), reference_path) == 0)) {
        fn.MakeRelativeTo(reference_path);
        return true;
    }

    return false;
}

wxString wxImplode(const wxArrayString& arr, const wxString& glue)
{
    wxString str, tmp;
    for(size_t i = 0; i < arr.GetCount(); i++) {
        str << arr.Item(i) << glue;
    }

    if(str.EndsWith(glue, &tmp)) {
        str = tmp;
    }
    return str;
}

wxString wxShellExec(const wxString& cmd, const wxString& projectName)
{
    wxString filename = wxFileName::CreateTempFileName(wxT("clTempFile"));
    wxString theCommand = wxString::Format(wxT("%s > \"%s\" 2>&1"), cmd.c_str(), filename.c_str());
    WrapInShell(theCommand);

    wxArrayString dummy;
    EnvSetter es(NULL, NULL, projectName, wxEmptyString);
    theCommand = EnvironmentConfig::Instance()->ExpandVariables(theCommand, false);
    ProcUtils::SafeExecuteCommand(theCommand, dummy);

    wxString content;
    wxFFile fp(filename, wxT("r"));
    if(fp.IsOpened()) {
        fp.ReadAll(&content);
    }
    fp.Close();
    clRemoveFile(filename);
    return content;
}

bool wxIsFileSymlink(const wxFileName& filename)
{
#ifdef __WXMSW__
    return false;
#else
    wxCharBuffer cb = filename.GetFullPath().mb_str(wxConvUTF8).data();
    struct stat stat_buff;
    // use lstat() otherwise, stat() will follow the actual file
    if(::lstat(cb.data(), &stat_buff) < 0)
        return false;
    return S_ISLNK(stat_buff.st_mode);
#endif
}

wxFileName wxReadLink(const wxFileName& filename)
{
#ifndef __WXMSW__
    if(wxIsFileSymlink(filename)) {
#if defined(__WXGTK__)
        // Use 'realpath' on Linux, otherwise this breaks on relative symlinks, and (untested) on symlinks-to-symlinks
        return wxFileName(CLRealPath(filename.GetFullPath()));

#else  // OSX
        wxFileName realFileName;
        char _tmp[512];
        memset(_tmp, 0, sizeof(_tmp));
        int len = readlink(filename.GetFullPath().mb_str(wxConvUTF8).data(), _tmp, sizeof(_tmp));
        if(len != -1) {
            realFileName = wxFileName(wxString(_tmp, wxConvUTF8, len));
            return realFileName;
        }
#endif // !OSX
    }
    return filename;

#else
    return filename;
#endif
}

wxString CLRealPath(const wxString& filepath) // This is readlink on steroids: it also makes-absolute, and dereferences
                                              // any symlinked dirs in the path
{
    return FileUtils::RealPath(filepath);
}

int wxStringToInt(const wxString& str, int defval, int minval, int maxval)
{
    long v;
    if(!str.ToLong(&v)) {
        return defval;
    }

    if(minval != -1 && v < minval)
        return defval;

    if(maxval != -1 && v > maxval)
        return defval;

    return v;
}

wxString wxIntToString(int val)
{
    wxString s;
    s << val;
    return s;
}

////////////////////////////////////////
// BOM
////////////////////////////////////////

BOM::BOM(const char* buffer, size_t len) { m_bom.AppendData(buffer, len); }

BOM::BOM() {}

BOM::~BOM() {}

wxFontEncoding BOM::Encoding()
{
    wxFontEncoding encoding = Encoding((const char*)m_bom.GetData());
    if(encoding != wxFONTENCODING_SYSTEM) {
        switch(encoding) {

        case wxFONTENCODING_UTF32BE:
        case wxFONTENCODING_UTF32LE:
            m_bom.SetDataLen(4);
            break;

        case wxFONTENCODING_UTF8:
            m_bom.SetDataLen(3);
            break;

        case wxFONTENCODING_UTF16BE:
        case wxFONTENCODING_UTF16LE:
        default:
            m_bom.SetDataLen(2);
            break;
        }
    }
    return encoding;
}

wxFontEncoding BOM::Encoding(const char* buff)
{
    // Support for BOM:
    //----------------------------------
    // 00 00 FE FF UTF-32, big-endian
    // FF FE 00 00 UTF-32, little-endian
    // FE FF       UTF-16, big-endian
    // FF FE       UTF-16, little-endian
    // EF BB BF    UTF-8
    //----------------------------------
    wxFontEncoding encoding = wxFONTENCODING_SYSTEM; /* -1 */

    static const char UTF32be[] = { 0x00, 0x00, (char)0xfe, (char)0xff };
    static const char UTF32le[] = { (char)0xff, (char)0xfe, 0x00, 0x00 };
    static const char UTF16be[] = { (char)0xfe, (char)0xff };
    static const char UTF16le[] = { (char)0xff, (char)0xfe };
    static const char UTF8[] = { (char)0xef, (char)0xbb, (char)0xbf };

    if(memcmp(buff, UTF32be, sizeof(UTF32be)) == 0) {
        encoding = wxFONTENCODING_UTF32BE;

    } else if(memcmp(buff, UTF32le, sizeof(UTF32le)) == 0) {
        encoding = wxFONTENCODING_UTF32LE;

    } else if(memcmp(buff, UTF16be, sizeof(UTF16be)) == 0) {
        encoding = wxFONTENCODING_UTF16BE;

    } else if(memcmp(buff, UTF16le, sizeof(UTF16le)) == 0) {
        encoding = wxFONTENCODING_UTF16LE;

    } else if(memcmp(buff, UTF8, sizeof(UTF8)) == 0) {
        encoding = wxFONTENCODING_UTF8;
    }
    return encoding;
}

void BOM::SetData(const char* buffer, size_t len)
{
    m_bom = wxMemoryBuffer();
    m_bom.SetDataLen(0);
    m_bom.AppendData(buffer, len);
}

int BOM::Len() const { return m_bom.GetDataLen(); }

void BOM::Clear()
{
    m_bom = wxMemoryBuffer();
    m_bom.SetDataLen(0);
}

/////////////////////////////////////////////////////////////////

clEventDisabler::clEventDisabler() { EventNotifier::Get()->DisableEvents(true); }

clEventDisabler::~clEventDisabler() { EventNotifier::Get()->DisableEvents(false); }

///////////////////////////////////////////////////////////////////////////////////////////////
// UTF8/16 conversions methods copied from wxScintilla
///////////////////////////////////////////////////////////////////////////////////////////////
enum { SURROGATE_LEAD_FIRST = 0xD800 };
enum { SURROGATE_TRAIL_FIRST = 0xDC00 };
enum { SURROGATE_TRAIL_LAST = 0xDFFF };

unsigned int clUTF8Length(const wchar_t* uptr, unsigned int tlen)
{
    unsigned int len = 0;
    for(unsigned int i = 0; i < tlen && uptr[i];) {
        unsigned int uch = uptr[i];
        if(uch < 0x80) {
            len++;
        } else if(uch < 0x800) {
            len += 2;
        } else if((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) {
            len += 4;
            i++;
        } else {
            len += 3;
        }
        i++;
    }
    return len;
}

// void UTF8FromUTF16(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len)
//{
//    int k = 0;
//    for (unsigned int i = 0; i < tlen && uptr[i];) {
//        unsigned int uch = uptr[i];
//        if (uch < 0x80) {
//            putf[k++] = static_cast<char>(uch);
//        } else if (uch < 0x800) {
//            putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
//            putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
//        } else if ((uch >= SURROGATE_LEAD_FIRST) &&
//                   (uch <= SURROGATE_TRAIL_LAST)) {
//            // Half a surrogate pair
//            i++;
//            unsigned int xch = 0x10000 + ((uch & 0x3ff) << 10) + (uptr[i] & 0x3ff);
//            putf[k++] = static_cast<char>(0xF0 | (xch >> 18));
//            putf[k++] = static_cast<char>(0x80 | ((xch >> 12) & 0x3f));
//            putf[k++] = static_cast<char>(0x80 | ((xch >> 6) & 0x3f));
//            putf[k++] = static_cast<char>(0x80 | (xch & 0x3f));
//        } else {
//            putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
//            putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
//            putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
//        }
//        i++;
//    }
//    putf[len] = '\0';
//}

// unsigned int UTF8CharLength(unsigned char ch)
//{
//    if (ch < 0x80) {
//        return 1;
//    } else if (ch < 0x80 + 0x40 + 0x20) {
//        return 2;
//    } else if (ch < 0x80 + 0x40 + 0x20 + 0x10) {
//        return 3;
//    } else {
//        return 4;
//    }
//}

// unsigned int UTF16Length(const char *s, unsigned int len)
//{
//    unsigned int ulen = 0;
//    unsigned int charLen;
//    for (unsigned int i=0; i<len;) {
//        unsigned char ch = static_cast<unsigned char>(s[i]);
//        if (ch < 0x80) {
//            charLen = 1;
//        } else if (ch < 0x80 + 0x40 + 0x20) {
//            charLen = 2;
//        } else if (ch < 0x80 + 0x40 + 0x20 + 0x10) {
//            charLen = 3;
//        } else {
//            charLen = 4;
//            ulen++;
//        }
//        i += charLen;
//        ulen++;
//    }
//    return ulen;
//}
//
// unsigned int UTF16FromUTF8(const char *s, unsigned int len, wchar_t *tbuf, unsigned int tlen)
//{
//    unsigned int ui=0;
//    const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
//    unsigned int i=0;
//    while ((i<len) && (ui<tlen)) {
//        unsigned char ch = us[i++];
//        if (ch < 0x80) {
//            tbuf[ui] = ch;
//        } else if (ch < 0x80 + 0x40 + 0x20) {
//            tbuf[ui] = static_cast<wchar_t>((ch & 0x1F) << 6);
//            ch = us[i++];
//            tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
//        } else if (ch < 0x80 + 0x40 + 0x20 + 0x10) {
//            tbuf[ui] = static_cast<wchar_t>((ch & 0xF) << 12);
//            ch = us[i++];
//            tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + ((ch & 0x7F) << 6));
//            ch = us[i++];
//            tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
//        } else {
//            // Outside the BMP so need two surrogates
//            int val = (ch & 0x7) << 18;
//            ch = us[i++];
//            val += (ch & 0x3F) << 12;
//            ch = us[i++];
//            val += (ch & 0x3F) << 6;
//            ch = us[i++];
//            val += (ch & 0x3F);
//            tbuf[ui] = static_cast<wchar_t>(((val - 0x10000) >> 10) + SURROGATE_LEAD_FIRST);
//            ui++;
//            tbuf[ui] = static_cast<wchar_t>((val & 0x3ff) + SURROGATE_TRAIL_FIRST);
//        }
//        ui++;
//    }
//    return ui;
//}

// [CHANGED] BEGIN
// void UTF8FromUCS2(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len)
//{
//    int k = 0;
//    for (unsigned int i = 0; i < tlen && uptr[i]; i++) {
//        unsigned int uch = uptr[i];
//        if (uch < 0x80) {
//            putf[k++] = static_cast<char>(uch);
//        } else if (uch < 0x800) {
//            putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
//            putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
//        } else {
//            putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
//            putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
//            putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
//        }
//    }
//    putf[len] = '\0';
//}

// unsigned int UCS2Length(const char *s, unsigned int len)
//{
//    unsigned int ulen = 0;
//    for (unsigned int i=0; i<len; i++) {
//        unsigned char ch = static_cast<unsigned char>(s[i]);
//        if ((ch < 0x80) || (ch > (0x80 + 0x40)))
//            ulen++;
//    }
//    return ulen;
//}

// unsigned int UCS2FromUTF8(const char *s, unsigned int len, wchar_t *tbuf, unsigned int tlen)
//{
//    unsigned int ui=0;
//    const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
//    unsigned int i=0;
//    while ((i<len) && (ui<tlen)) {
//        unsigned char ch = us[i++];
//        if (ch < 0x80) {
//            tbuf[ui] = ch;
//        } else if (ch < 0x80 + 0x40 + 0x20) {
//            tbuf[ui] = static_cast<wchar_t>((ch & 0x1F) << 6);
//            ch = us[i++];
//            tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
//        } else {
//            tbuf[ui] = static_cast<wchar_t>((ch & 0xF) << 12);
//            ch = us[i++];
//            tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + ((ch & 0x7F) << 6));
//            ch = us[i++];
//            tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
//        }
//        ui++;
//    }
//    return ui;
//}
// [CHANGED] END

wxString DbgPrependCharPtrCastIfNeeded(const wxString& expr, const wxString& exprType)
{
    static wxRegEx reConstArr(wxT("(const )?[ ]*(w)?char(_t)? *[\\[0-9\\]]*"));

    bool arrayAsCharPtr = false;
    DebuggerInformation info;
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr) {
        DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), info);
        arrayAsCharPtr = info.charArrAsPtr;
    }

    wxString newExpr;
    if(arrayAsCharPtr && reConstArr.Matches(exprType)) {
        // array
        newExpr << wxT("(char*)") << expr;

    } else {
        newExpr << expr;
    }
    return newExpr;
}

wxVariant MakeIconText(const wxString& text, const wxBitmap& bmp)
{
    wxIcon icn;
    icn.CopyFromBitmap(bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}

void PostCall(wxObject* instance, clEventFunc_t func, wxClientData* arg)
{
    clInternalEventHandlerData* cd = new clInternalEventHandlerData(instance, func, arg);
    wxCommandEvent evt(wxEVT_COMMAND_CL_INTERNAL_1_ARGS);
    evt.SetClientObject(cd);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void PostCall(wxObject* instance, clEventFunc_t func)
{
    clInternalEventHandlerData* cd = new clInternalEventHandlerData(instance, func);
    wxCommandEvent evt(wxEVT_COMMAND_CL_INTERNAL_0_ARGS);
    evt.SetClientObject(cd);
    EventNotifier::Get()->AddPendingEvent(evt);
}

wxArrayString SplitString(const wxString& inString, bool trim)
{
    wxArrayString lines;
    wxString curline;

    bool inContinuation = false;
    for(size_t i = 0; i < inString.length(); ++i) {
        wxChar ch = inString.GetChar(i);
        wxChar ch1 = (i + 1 < inString.length()) ? inString.GetChar(i + 1) : wxUniChar(0);
        wxChar ch2 = (i + 2 < inString.length()) ? inString.GetChar(i + 2) : wxUniChar(0);

        switch(ch) {
        case '\r':
            // do nothing
            curline << ch;
            break;
        case '\n':
            if(inContinuation) {
                curline << ch;

            } else {
                lines.Add(trim ? curline.Trim().Trim(false) : curline);
                curline.clear();
            }
            inContinuation = false;
            break;
        case '\\':
            curline << ch;
            if((ch1 == '\n') || (ch1 == '\r' && ch2 == '\n')) {
                inContinuation = true;
            }
            break;
        default:
            curline << ch;
            inContinuation = false;
            break;
        }
    }

    // any leftovers?
    if(curline.IsEmpty() == false) {
        lines.Add(trim ? curline.Trim().Trim(false) : curline);
        curline.clear();
    }
    return lines;
}

void LaunchTerminalForDebugger(const wxString& title, wxString& tty, wxString& realPts, long& pid)
{
    pid = wxNOT_FOUND;
    tty.Clear();
    realPts.Clear();

#if defined(__WXMSW__)
    // Windows
    wxUnusedVar(title);
#else
    // Non Windows machines
    clConsoleBase::Ptr_t console = clConsoleBase::GetTerminal();
    if(console->StartForDebugger()) {
        tty = console->GetTty();
        realPts = console->GetRealPts();
        pid = console->GetPid();
    }
#endif // !__WXMSW__
}

wxStandardID PromptForYesNoCancelDialogWithCheckbox(const wxString& message, const wxString& dlgId,
                                                    const wxString& yesLabel, const wxString& noLabel,
                                                    const wxString& cancelLabel, const wxString& checkboxLabel,
                                                    long style, bool checkboxInitialValue)
{
    int res = clConfig::Get().GetAnnoyingDlgAnswer(dlgId, wxNOT_FOUND);
    if(res == wxNOT_FOUND) {

        // User did not save his answer
        wxRichMessageDialog d(EventNotifier::Get()->TopFrame(), message, "CodeLite", style);
        d.ShowCheckBox(checkboxLabel);
        if(cancelLabel.empty()) {
            d.SetYesNoLabels(yesLabel, noLabel);
        } else {
            d.SetYesNoCancelLabels(yesLabel, noLabel, cancelLabel);
        }

        res = d.ShowModal();
        if(d.IsCheckBoxChecked() && (res != wxID_CANCEL)) {
            // store the user result
            clConfig::Get().SetAnnoyingDlgAnswer(dlgId, res);
        }
    }
    return static_cast<wxStandardID>(res);
}

wxStandardID PromptForYesNoDialogWithCheckbox(const wxString& message, const wxString& dlgId, const wxString& yesLabel,
                                              const wxString& noLabel, const wxString& checkboxLabel, long style,
                                              bool checkboxInitialValue)
{
    return PromptForYesNoCancelDialogWithCheckbox(message, dlgId, yesLabel, noLabel, "", checkboxLabel, style,
                                                  checkboxInitialValue);
}

static wxChar sPreviousChar(wxStyledTextCtrl* ctrl, int pos, int& foundPos, bool wantWhitespace)
{
    wxChar ch = 0;
    long curpos = ctrl->PositionBefore(pos);
    if(curpos == 0) {
        foundPos = curpos;
        return ch;
    }

    while(true) {
        ch = ctrl->GetCharAt(curpos);
        if(ch == wxT('\t') || ch == wxT(' ') || ch == wxT('\r') || ch == wxT('\v') || ch == wxT('\n')) {
            // if the caller is intrested in whitepsaces,
            // simply return it
            if(wantWhitespace) {
                foundPos = curpos;
                return ch;
            }

            long tmpPos = curpos;
            curpos = ctrl->PositionBefore(curpos);
            if(curpos == 0 && tmpPos == curpos)
                break;
        } else {
            foundPos = curpos;
            return ch;
        }
    }
    foundPos = -1;
    return ch;
}

wxString GetCppExpressionFromPos(long pos, wxStyledTextCtrl* ctrl, bool forCC)
{
    bool cont(true);
    int depth(0);

    int position(pos);
    int at(position);
    bool prevGt(false);
    while(cont && depth >= 0) {
        wxChar ch = sPreviousChar(ctrl, position, at, true);
        position = at;
        // Eof?
        if(ch == 0) {
            at = 0;
            break;
        }

        // Comment?
        int style = ctrl->GetStyleAt(position);
        if(style == wxSTC_C_COMMENT || style == wxSTC_C_COMMENTLINE || style == wxSTC_C_COMMENTDOC ||
           style == wxSTC_C_COMMENTLINEDOC || style == wxSTC_C_COMMENTDOCKEYWORD ||
           style == wxSTC_C_COMMENTDOCKEYWORDERROR || style == wxSTC_C_STRING || style == wxSTC_C_STRINGEOL ||
           style == wxSTC_C_CHARACTER) {
            continue;
        }

        switch(ch) {
        case wxT(';'):
            // dont include this token
            at = ctrl->PositionAfter(at);
            cont = false;
            break;
        case wxT('-'):
            if(prevGt) {
                prevGt = false;
                // if previous char was '>', we found an arrow so reduce the depth
                // which was increased
                depth--;
            } else {
                if(depth <= 0) {
                    // dont include this token
                    at = ctrl->PositionAfter(at);
                    cont = false;
                }
            }
            break;
        case wxT(' '):
        case wxT('\n'):
        case wxT('\v'):
        case wxT('\t'):
        case wxT('\r'):
            prevGt = false;
            if(depth <= 0) {
                cont = false;
                break;
            }
            break;
        case wxT('{'):
        case wxT('='):
            prevGt = false;
            cont = false;
            break;
        case wxT('('):
        case wxT('['):
            depth--;
            prevGt = false;
            if(depth < 0) {
                // dont include this token
                at = ctrl->PositionAfter(at);
                cont = false;
            }
            break;
        case wxT(','):
        case wxT('*'):
        case wxT('&'):
        case wxT('!'):
        case wxT('~'):
        case wxT('+'):
        case wxT('^'):
        case wxT('|'):
        case wxT('%'):
        case wxT('?'):
            prevGt = false;
            if(depth <= 0) {

                // dont include this token
                at = ctrl->PositionAfter(at);
                cont = false;
            }
            break;
        case wxT('>'):
            prevGt = true;
            depth++;
            break;
        case wxT('<'):
            prevGt = false;
            depth--;
            if(depth < 0) {

                // dont include this token
                at = ctrl->PositionAfter(at);
                cont = false;
            }
            break;
        case wxT(')'):
        case wxT(']'):
            prevGt = false;
            depth++;
            break;
        default:
            prevGt = false;
            break;
        }
    }

    if(at < 0)
        at = 0;
    wxString expr = ctrl->GetTextRange(at, pos);
    if(!forCC) {
        // If we do not require the expression for CodeCompletion
        // return the un-touched buffer
        return expr;
    }

    // remove comments from it
    CppScanner sc;
    sc.SetText(_C(expr));
    wxString expression;
    int type = 0;
    while((type = sc.yylex()) != 0) {
        wxString token = _U(sc.YYText());
        expression += token;
        expression += wxT(" ");
    }
    return expression;
}
wxString& WrapWithQuotes(wxString& str)
{
    if(str.Contains(" ")) {
        str.Prepend("\"").Append("\"");
    }
    return str;
}

wxString& EscapeSpaces(wxString& str)
{
    str.Replace(" ", "\\ ");
    return str;
}

bool SaveXmlToFile(wxXmlDocument* doc, const wxString& filename)
{
    CHECK_PTR_RET_FALSE(doc);

    wxString content;
    wxStringOutputStream sos(&content);
    if(doc->Save(sos)) {
        return ::WriteFileUTF8(filename, content);
    }
    return false;
}

wxString MakeCommandRunInBackground(const wxString& cmd)
{
    wxString alteredCommand;
#ifdef __WXMSW__
    alteredCommand << "start /b " << cmd;
#else
    // POSIX systems
    alteredCommand << cmd << "&";
#endif
    return alteredCommand;
}

void wxPGPropertyBooleanUseCheckbox(wxPropertyGrid* grid)
{
    grid->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);
}

void clRecalculateSTCHScrollBar(wxStyledTextCtrl* ctrl)
{
    // recalculate and set the length of horizontal scrollbar
    int maxPixel = 0;
    int startLine = ctrl->GetFirstVisibleLine();
    int endLine = startLine + ctrl->LinesOnScreen();
    if(endLine >= (ctrl->GetLineCount() - 1))
        endLine--;

    for(int i = startLine; i <= endLine; i++) {
        int visibleLine = (int)ctrl->DocLineFromVisible(i);      // get actual visible line, folding may offset lines
        int endPosition = ctrl->GetLineEndPosition(visibleLine); // get character position from begin
        int beginPosition = ctrl->PositionFromLine(visibleLine); // and end of line

        wxPoint beginPos = ctrl->PointFromPosition(beginPosition);
        wxPoint endPos = ctrl->PointFromPosition(endPosition);

        int curLen = endPos.x - beginPos.x;

        if(maxPixel < curLen) // If its the largest line yet
            maxPixel = curLen;
    }

    if(maxPixel == 0)
        maxPixel++; // make sure maxPixel is valid

    int currentLength = ctrl->GetScrollWidth(); // Get current scrollbar size
    if(currentLength != maxPixel) {
        // And if it is not the same, update it
        ctrl->SetScrollWidth(maxPixel);
    }
}
wxString clGetTextFromUser(const wxString& title, const wxString& message, const wxString& initialValue,
                           int charsToSelect, wxWindow* parent)
{
    clGetTextFromUserDialog dialog(parent == NULL ? EventNotifier::Get()->TopFrame() : parent, title, message,
                                   initialValue, charsToSelect);
    if(dialog.ShowModal() == wxID_OK) {
        return dialog.GetValue();
    }
    return "";
}

static IManager* s_pluginManager = NULL;
IManager* clGetManager()
{
    wxASSERT(s_pluginManager);
    return s_pluginManager;
}

void clSetManager(IManager* manager) { s_pluginManager = manager; }

void clStripTerminalColouring(const wxString& buffer, wxString& modbuffer)
{
    StringUtils::StripTerminalColouring(buffer, modbuffer);
}

bool clIsVaidProjectName(const wxString& name)
{
    return name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == wxString::npos;
}

double clGetContentScaleFactor()
{
    static bool once = false;
    static double res = 1.0;
    if(!once) {
        once = true;
#ifdef __WXGTK__
        GdkScreen* screen = gdk_screen_get_default();
        if(screen) {
            res = gdk_screen_get_resolution(screen) / 96.;
        }
#else
        res = (wxScreenDC().GetPPI().y / 96.);
#endif
    }
    return res;
}

int clGetScaledSize(int size)
{
    if(clGetContentScaleFactor() >= 1.5) {
        return size * 2;
    } else {
        return size;
    }
}

void clKill(int processID, wxSignal signo, bool kill_whole_group, bool as_superuser)
{
#ifdef __WXMSW__
    wxUnusedVar(as_superuser);
    ::wxKill(processID, signo, NULL, kill_whole_group ? wxKILL_CHILDREN : wxKILL_NOCHILDREN);
#else
    wxString sudoAskpass = ::wxGetenv("SUDO_ASKPASS");
    const char* sudo_path;

    sudo_path = "/usr/bin/sudo";
    if(!wxFileName::Exists(sudo_path)) {
        sudo_path = "/usr/local/bin/sudo";
    }
    if(as_superuser && wxFileName::Exists(sudo_path) && wxFileName::Exists(sudoAskpass)) {
        wxString cmd;
        cmd << sudo_path << " --askpass kill -" << (int)signo << " ";
        if(kill_whole_group) {
            cmd << "-";
        }
        cmd << processID;
        int rc = system(cmd.mb_str(wxConvUTF8).data());
        wxUnusedVar(rc);
    } else {
        ::wxKill(processID, signo, NULL, kill_whole_group ? wxKILL_CHILDREN : wxKILL_NOCHILDREN);
    }
#endif
}

void clSetEditorFontEncoding(const wxString& encoding)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    options->SetFileFontEncoding(encoding);
    TagsManagerST::Get()->SetEncoding(options->GetFileFontEncoding());
    EditorConfigST::Get()->SetOptions(options);
}

bool clFindExecutable(const wxString& name, wxFileName& exepath, const wxArrayString& hint)
{
    return FileUtils::FindExe(name, exepath, hint);
}

int clFindMenuItemPosition(wxMenu* menu, int menuItemId)
{
    if(!menu)
        return wxNOT_FOUND;

    const wxMenuItemList& list = menu->GetMenuItems();
    wxMenuItemList::const_iterator iter = list.begin();
    for(int pos = 0; iter != list.end(); ++iter, ++pos) {
        if((*iter)->GetId() == menuItemId) {
            return pos;
        }
    }
    return wxNOT_FOUND;
}

bool clNextWord(const wxString& str, size_t& offset, wxString& word) { return FileUtils::NextWord(str, offset, word); }

wxString clJoinLinesWithEOL(const wxArrayString& lines, int eol)
{
    wxString glue = "\n";
    switch(eol) {
    case wxSTC_EOL_CRLF:
        glue = "\r\n";
        break;
    case wxSTC_EOL_CR:
        glue = "\r";
        break;
    default:
        glue = "\n";
        break;
    }
    wxString result;
    for(size_t i = 0; i < lines.size(); ++i) {
        if(!result.IsEmpty()) {
            result << glue;
        }
        result << lines.Item(i);
    }
    return result;
}

wxSize clGetDisplaySize()
{
    // Calculate the display size only once. If the user changes the display size, he will need to restart CodeLite
    static wxSize displaySize;
    if(displaySize.GetHeight() == 0) {

        int displayWidth = ::wxGetDisplaySize().GetWidth();
        int displayHeight = ::wxGetDisplaySize().GetHeight();
        for(size_t i = 0; i < wxDisplay::GetCount(); ++i) {
            wxDisplay display(i);
            displayWidth = wxMax(display.GetClientArea().GetWidth(), displayWidth);
            displayHeight = wxMax(display.GetClientArea().GetHeight(), displayHeight);
        }
        displaySize = wxSize(displayWidth, displayHeight);
    }
    return displaySize;
}

void clFitColumnWidth(wxDataViewCtrl* ctrl)
{
#ifndef __WXOSX__
    for(size_t i = 0; i < ctrl->GetColumnCount(); ++i) {
        ctrl->GetColumn(i)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    }
#endif
}

wxVariant MakeBitmapIndexText(const wxString& text, int imgIndex)
{
    clDataViewTextBitmap tb(text, imgIndex);
    wxVariant vr;
    vr << tb;
    return vr;
}

wxVariant MakeCheckboxVariant(const wxString& label, bool checked, int imgIndex)
{
    clDataViewCheckbox cb(checked, imgIndex, label);
    wxVariant vr;
    vr << cb;
    return vr;
}

void clSetTLWindowBestSizeAndPosition(wxWindow* win)
{
    if(!win || !win->GetParent()) {
        return;
    }
    wxTopLevelWindow* tlw = dynamic_cast<wxTopLevelWindow*>(win);
    wxTopLevelWindow* parentTlw = dynamic_cast<wxTopLevelWindow*>(win->GetParent());

    if(!tlw || !parentTlw) {
        return;
    }

    wxRect frameSize = parentTlw->GetSize();
    frameSize.Deflate(100);
    tlw->SetMinSize(frameSize.GetSize());
    tlw->SetSize(frameSize.GetSize());
    tlw->GetSizer()->Fit(win);
    tlw->CentreOnParent();
}

static void DoSetDialogSize(wxDialog* win, double factor)
{
#if 0
    wxUnusedVar(win);
    wxUnusedVar(factor);
#else
    if(!win) {
        return;
    }
    if(factor <= 0.0) {
        factor = 1.0;
    }

    wxWindow* parent = win->GetParent();
    if(!parent) {
        parent = wxTheApp->GetTopWindow();
    }
    if(parent) {
        wxSize parentSize = parent->GetSize();

        double dlgWidth = (double)parentSize.GetWidth() * factor;
        double dlgHeight = (double)parentSize.GetHeight() * factor;
        parentSize.SetWidth(dlgWidth);
        parentSize.SetHeight(dlgHeight);
        win->SetMinSize(parentSize);
        win->SetSize(parentSize);
        // win->GetSizer()->Fit(win);
        win->GetSizer()->Layout();
        win->CentreOnParent();
    }
#endif
}

void clSetDialogBestSizeAndPosition(wxDialog* win) { DoSetDialogSize(win, 0.66); }

void clSetSmallDialogBestSizeAndPosition(wxDialog* win) { DoSetDialogSize(win, 0.5); }

void clSetDialogSizeAndPosition(wxDialog* win, double ratio) { DoSetDialogSize(win, ratio); }

bool clIsCxxWorkspaceOpened() { return clCxxWorkspaceST::Get()->IsOpen() || clFileSystemWorkspace::Get().IsOpen(); }

int clGetSize(int size, const wxWindow* win)
{
    if(!win) {
        return size;
    }
#ifdef __WXGTK__
    wxString dpiscale = "1.0";
    if(wxGetEnv("GDK_DPI_SCALE", &dpiscale)) {
        double scale = 1.0;
        if(dpiscale.ToDouble(&scale)) {
            double scaledSize = scale * size;
            return scaledSize;
        }
    }
#endif
#if wxCHECK_VERSION(3, 1, 0)
    return win->FromDIP(size);
#else
    return size;
#endif
}

bool clIsWaylandSession()
{
#ifdef __WXGTK__
    // Try to detect if this is a Wayland session; we have some Wayland-workaround code
    wxString sesstype("XDG_SESSION_TYPE"), session_type;
    wxGetEnv(sesstype, &session_type);
    return session_type.Lower().Contains("wayland");
#else
    return false;
#endif
}
