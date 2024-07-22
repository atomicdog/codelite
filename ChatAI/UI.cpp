//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: UI.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "UI.hpp"

// Declare the bitmap loading function
extern void wxCF667InitBitmapResources();

namespace
{
// return the wxBORDER_SIMPLE that matches the current application theme
wxBorder get_border_simple_theme_aware_bit()
{
#if wxVERSION_NUMBER >= 3300 && defined(__WXMSW__)
    return wxSystemSettings::GetAppearance().IsDark() ? wxBORDER_SIMPLE : wxBORDER_STATIC;
#else
    return wxBORDER_DEFAULT;
#endif
} // DoGetBorderSimpleBit
bool bBitmapLoaded = false;
} // namespace

AssistanceAISettingsBaseDlg::AssistanceAISettingsBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title,
                                                         const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if (!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCF667InitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer1);

    wxBoxSizer* boxSizer5 = new wxBoxSizer(wxHORIZONTAL);

    boxSizer1->Add(boxSizer5, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBK_DEFAULT);
    m_notebook->SetName(wxT("m_notebook"));

    boxSizer5->Add(m_notebook, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_generalSettings =
        new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook, wxSize(-1, -1)), wxTAB_TRAVERSAL);
    m_notebook->AddPage(m_generalSettings, _("General"), true);

    wxFlexGridSizer* flexGridSizer11 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer11->SetFlexibleDirection(wxBOTH);
    flexGridSizer11->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer11->AddGrowableCol(1);
    m_generalSettings->SetSizer(flexGridSizer11);

    m_staticText12 = new wxStaticText(m_generalSettings, wxID_ANY, _("llama-cli:"), wxDefaultPosition,
                                      wxDLG_UNIT(m_generalSettings, wxSize(-1, -1)), 0);

    flexGridSizer11->Add(m_staticText12, 0, wxALL | wxALIGN_RIGHT | wxALIGN_BOTTOM, WXC_FROM_DIP(5));

    m_filePickerCLI = new wxFilePickerCtrl(m_generalSettings, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"),
                                           wxDefaultPosition, wxDLG_UNIT(m_generalSettings, wxSize(-1, -1)),
                                           wxFLP_DEFAULT_STYLE | wxFLP_SMALL);
    m_filePickerCLI->SetFocus();

    flexGridSizer11->Add(m_filePickerCLI, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText39 = new wxStaticText(m_generalSettings, wxID_ANY, _("Active model:"), wxDefaultPosition,
                                      wxDLG_UNIT(m_generalSettings, wxSize(-1, -1)), 0);

    flexGridSizer11->Add(m_staticText39, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    wxArrayString m_choiceModelsArr;
    m_choiceModels = new wxChoice(m_generalSettings, wxID_ANY, wxDefaultPosition,
                                  wxDLG_UNIT(m_generalSettings, wxSize(-1, -1)), m_choiceModelsArr, 0);

    flexGridSizer11->Add(m_choiceModels, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer23 = new wxBoxSizer(wxVERTICAL);

    boxSizer5->Add(boxSizer23, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_buttonNew = new wxButton(this, wxID_ANY, _("&New"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_buttonNew->SetToolTip(_("Add new model"));

    boxSizer23->Add(m_buttonNew, 0, wxALL, WXC_FROM_DIP(5));

    m_stdBtnSizer2 = new wxStdDialogButtonSizer();

    boxSizer1->Add(m_stdBtnSizer2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(10));

    m_button3 = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_button3->SetDefault();
    m_stdBtnSizer2->AddButton(m_button3);

    m_button4 = new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
    m_stdBtnSizer2->AddButton(m_button4);
    m_stdBtnSizer2->Realize();

#if wxVERSION_NUMBER >= 2900
    if (!wxPersistenceManager::Get().Find(m_notebook)) {
        wxPersistenceManager::Get().RegisterAndRestore(m_notebook);
    } else {
        wxPersistenceManager::Get().Restore(m_notebook);
    }
#endif

    SetName(wxT("AssistanceAISettingsBaseDlg"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if (GetSizer()) {
        GetSizer()->Fit(this);
    }
    if (GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
    if (!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
    // Connect events
    m_buttonNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAISettingsBaseDlg::OnNewModel, this);
    m_button3->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAISettingsBaseDlg::OnOK, this);
}

AssistanceAISettingsBaseDlg::~AssistanceAISettingsBaseDlg()
{
    m_buttonNew->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAISettingsBaseDlg::OnNewModel, this);
    m_button3->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAISettingsBaseDlg::OnOK, this);
}

ModelPageBase::ModelPageBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if (!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCF667InitBitmapResources();
        bBitmapLoaded = true;
    }

    wxFlexGridSizer* flexGridSizer16 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer16->SetFlexibleDirection(wxBOTH);
    flexGridSizer16->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer16->AddGrowableCol(1);
    this->SetSizer(flexGridSizer16);

    m_staticText17 =
        new wxStaticText(this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    flexGridSizer16->Add(m_staticText17, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_textCtrlModelName =
        new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
#if wxVERSION_NUMBER >= 3000
    m_textCtrlModelName->SetHint(wxT(""));
#endif

    flexGridSizer16->Add(m_textCtrlModelName, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_staticText19 =
        new wxStaticText(this, wxID_ANY, _("Model file:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

    flexGridSizer16->Add(m_staticText19, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

    m_filePickerModelFile =
        new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition,
                             wxDLG_UNIT(this, wxSize(-1, -1)), wxFLP_DEFAULT_STYLE | wxFLP_SMALL);

    flexGridSizer16->Add(m_filePickerModelFile, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    SetName(wxT("ModelPageBase"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if (GetSizer()) {
        GetSizer()->Fit(this);
    }
}

ModelPageBase::~ModelPageBase() {}

AssistanceAIChatWindowBase::AssistanceAIChatWindowBase(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                                                       const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if (!bBitmapLoaded) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCF667InitBitmapResources();
        bBitmapLoaded = true;
    }

    wxBoxSizer* boxSizer27 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer27);

    m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTB_FLAT);
    m_toolbar->SetToolBitmapSize(wxSize(16, 16));

    boxSizer27->Add(m_toolbar, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_splitter30 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                        wxSP_LIVE_UPDATE | wxSP_3D);
    m_splitter30->SetSashGravity(1);
    m_splitter30->SetMinimumPaneSize(150);

    boxSizer27->Add(m_splitter30, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_splitterPage32 = new wxPanel(m_splitter30, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter30, wxSize(-1, -1)),
                                   wxTAB_TRAVERSAL);

    wxBoxSizer* boxSizer35 = new wxBoxSizer(wxVERTICAL);
    m_splitterPage32->SetSizer(boxSizer35);

    m_stcOutput = new clThemedSTC(m_splitterPage32, wxID_ANY, wxDefaultPosition,
                                  wxDLG_UNIT(m_splitterPage32, wxSize(-1, -1)), wxBORDER_NONE);
    // Configure the fold margin
    m_stcOutput->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcOutput->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcOutput->SetMarginSensitive(4, true);
    m_stcOutput->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_stcOutput->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_stcOutput->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcOutput->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcOutput->SetMarginWidth(2, 0);
    m_stcOutput->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_stcOutput->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcOutput->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_stcOutput->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcOutput->SetMarginMask(3, 0);
    m_stcOutput->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcOutput->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcOutput->StyleClearAll();
    m_stcOutput->SetWrapMode(1);
    m_stcOutput->SetIndentationGuides(0);
    m_stcOutput->SetEOLMode(2);
    m_stcOutput->SetKeyWords(0, wxT(""));
    m_stcOutput->SetKeyWords(1, wxT(""));
    m_stcOutput->SetKeyWords(2, wxT(""));
    m_stcOutput->SetKeyWords(3, wxT(""));
    m_stcOutput->SetKeyWords(4, wxT(""));

    boxSizer35->Add(m_stcOutput, 1, wxEXPAND, WXC_FROM_DIP(5));

    m_splitterPage34 = new wxPanel(m_splitter30, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter30, wxSize(-1, -1)),
                                   wxTAB_TRAVERSAL);
    m_splitter30->SplitHorizontally(m_splitterPage32, m_splitterPage34, 0);

    wxBoxSizer* boxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    m_splitterPage34->SetSizer(boxSizer36);

    m_stcInput = new clThemedSTC(m_splitterPage34, wxID_ANY, wxDefaultPosition,
                                 wxDLG_UNIT(m_splitterPage34, wxSize(-1, -1)), wxBORDER_STATIC);
    m_stcInput->SetFocus();
    // Configure the fold margin
    m_stcInput->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
    m_stcInput->SetMarginMask(4, wxSTC_MASK_FOLDERS);
    m_stcInput->SetMarginSensitive(4, true);
    m_stcInput->SetMarginWidth(4, 0);

    // Configure the tracker margin
    m_stcInput->SetMarginWidth(1, 0);

    // Configure the symbol margin
    m_stcInput->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_stcInput->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_stcInput->SetMarginWidth(2, 0);
    m_stcInput->SetMarginSensitive(2, true);

    // Configure the line numbers margin
    m_stcInput->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stcInput->SetMarginWidth(0, 0);

    // Configure the line symbol margin
    m_stcInput->SetMarginType(3, wxSTC_MARGIN_FORE);
    m_stcInput->SetMarginMask(3, 0);
    m_stcInput->SetMarginWidth(3, 0);
    // Select the lexer
    m_stcInput->SetLexer(wxSTC_LEX_NULL);
    // Set default font / styles
    m_stcInput->StyleClearAll();
    m_stcInput->SetWrapMode(1);
    m_stcInput->SetIndentationGuides(0);
    m_stcInput->SetEOLMode(2);
    m_stcInput->SetKeyWords(0, wxT(""));
    m_stcInput->SetKeyWords(1, wxT(""));
    m_stcInput->SetKeyWords(2, wxT(""));
    m_stcInput->SetKeyWords(3, wxT(""));
    m_stcInput->SetKeyWords(4, wxT(""));

    boxSizer36->Add(m_stcInput, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    wxBoxSizer* boxSizer42 = new wxBoxSizer(wxVERTICAL);

    boxSizer36->Add(boxSizer42, 0, wxEXPAND, WXC_FROM_DIP(5));

    m_button37 = new wxButton(m_splitterPage34, wxID_ANY, _("Send"), wxDefaultPosition,
                              wxDLG_UNIT(m_splitterPage34, wxSize(-1, -1)), 0);
    m_button37->SetDefault();
    m_button37->SetToolTip(_("Send prompt\nShift+ENTER"));

    boxSizer42->Add(m_button37, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_buttonStop = new wxButton(m_splitterPage34, wxID_ANY, _("Stop"), wxDefaultPosition,
                                wxDLG_UNIT(m_splitterPage34, wxSize(-1, -1)), 0);

    boxSizer42->Add(m_buttonStop, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    SetName(wxT("AssistanceAIChatWindowBase"));
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if (GetSizer()) {
        GetSizer()->Fit(this);
    }
    // Connect events
    m_stcInput->Bind(wxEVT_UPDATE_UI, &AssistanceAIChatWindowBase::OnInputUI, this);
    m_button37->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAIChatWindowBase::OnSend, this);
    m_button37->Bind(wxEVT_UPDATE_UI, &AssistanceAIChatWindowBase::OnSendUI, this);
    m_buttonStop->Bind(wxEVT_UPDATE_UI, &AssistanceAIChatWindowBase::OnStopUI, this);
    m_buttonStop->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAIChatWindowBase::OnStop, this);
}

AssistanceAIChatWindowBase::~AssistanceAIChatWindowBase()
{
    m_stcInput->Unbind(wxEVT_UPDATE_UI, &AssistanceAIChatWindowBase::OnInputUI, this);
    m_button37->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAIChatWindowBase::OnSend, this);
    m_button37->Unbind(wxEVT_UPDATE_UI, &AssistanceAIChatWindowBase::OnSendUI, this);
    m_buttonStop->Unbind(wxEVT_UPDATE_UI, &AssistanceAIChatWindowBase::OnStopUI, this);
    m_buttonStop->Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &AssistanceAIChatWindowBase::OnStop, this);
}
