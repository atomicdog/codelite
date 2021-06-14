; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=CodeLite
AppVerName=CodeLite
AppPublisher=Eran Ifrah
AppVersion=15.0.5
AppPublisherURL=http://codelite.org
AppSupportURL=http://codelite.org
AppUpdatesURL=http://codelite.org
DefaultDirName={pf32}\CodeLite
DefaultGroupName=CodeLite
LicenseFile=license.txt
OutputDir=output
OutputBaseFilename=codelite-i686-15.0.5
ChangesEnvironment=yes
FlatComponentsList=yes
SetupIconFile=box_software.ico
Compression=lzma/ultra
SolidCompression=true
InternalCompressLevel=ultra
PrivilegesRequired=none
UninstallDisplayIcon={app}\codelite.exe,0

;;==================================
;; 32 bit setup
;;==================================
#define CODELITE_ROOT "C:\src\codelite"
#define WXWIN "C:\src\wxWidgets32"
#define MINGW_DIR "C:\compilers\mingw64-i686\mingw32\bin"
#define CLANG_DIR "C:\LLVM\bin"
#define BUILD_BIN_DIR "C:\src\codelite\build-Win_x86_Release\bin"


[Languages]
Name: "eng"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#BUILD_BIN_DIR}\codelite.exe"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#BUILD_BIN_DIR}\codelite-echo.exe"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#BUILD_BIN_DIR}\codelite-make.exe"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#BUILD_BIN_DIR}\codelite-terminal.exe"; DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#BUILD_BIN_DIR}\wx-config.exe"; DestDir: "{app}";
Source: "{#BUILD_BIN_DIR}\codelite_indexer.exe"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#BUILD_BIN_DIR}\codelite_launcher.exe"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#BUILD_BIN_DIR}\codelite_cppcheck.exe"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#BUILD_BIN_DIR}\codelite-cc.exe"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#BUILD_BIN_DIR}\codelite-exec.exe"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#BUILD_BIN_DIR}\makedir.exe"; DestDir: "{app}"; Flags: ignoreversion ; 


;; ---- wxWidgets DLLs
Source: "{#WXWIN}\lib\gcc_dll\wxbase313u_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxbase313u_net_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxbase313u_xml_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_adv_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_aui_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_core_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_html_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_propgrid_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_richtext_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_stc_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_xrc_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#WXWIN}\lib\gcc_dll\wxmsw313u_ribbon_gcc_cl.dll"; DestDir: "{app}"; Flags: ignoreversion;

;; ---- MinGW64 Dlls
Source: "{#MINGW_DIR}\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#MINGW_DIR}\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#MINGW_DIR}\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion;

;; ---- Binaries needed by CodeLite
Source: "{#CODELITE_ROOT}\InnoSetup\license.txt"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\LICENSE"; DestDir: "{app}"; Flags: ignoreversion; 

;; ---------- Configuration files can be copied from the 64 bit version
Source: "{#CODELITE_ROOT}\Runtime\codelite-remote"; DestDir: "{app}";
Source: "{#CODELITE_ROOT}\Runtime\config\codelite.xml.default"; DestDir: "{app}\config";
Source: "{#CODELITE_ROOT}\Runtime\config\plugins.xml.default"; DestDir: "{app}\config";
Source: "{#CODELITE_ROOT}\Runtime\config\accelerators.conf.default"; DestDir: "{app}\config";
Source: "{#CODELITE_ROOT}\Runtime\config\debuggers.xml.default"; DestDir: "{app}\config";
Source: "{#CODELITE_ROOT}\Runtime\config\build_settings.xml.default.win"; DestDir: "{app}\config"; DestName: "build_settings.xml.default"; 
Source: "{#CODELITE_ROOT}\Runtime\rc\*"; DestDir: "{app}\rc"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\astyle.sample"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\php.sample"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\config\codelite.layout.default"; DestDir: "{app}\config"; DestName: codelite.layout; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\templates\*"; DestDir: "{app}\templates"; Flags: recursesubdirs ; 
Source: "{#CODELITE_ROOT}\SpellChecker\dics\*"; DestDir: "{app}\dics"; Flags: recursesubdirs ; 
Source: "{#CODELITE_ROOT}\Runtime\plugins\resources\*"; DestDir: "{app}\plugins\resources\"; Flags: ignoreversion ; 

; Copy clangd for Windows
Source: "{#CLANG_DIR}\clangd.exe"; DestDir: "{app}/lsp"; Flags: ignoreversion;
Source: "{#CLANG_DIR}\clang-format.exe"; DestDir: "{app}/"; Flags: ignoreversion;

; Override with Windows specific files
Source: "{#CODELITE_ROOT}\Runtime\templates\projects\dynamic-library\dynamic-library.project.windows"; DestName: dynamic-library.project; DestDir: "{app}\templates\projects\dynamic-library"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\templates\projects\dynamic-library-wx-enabled\dynamic-library-wx-enabled.project.windows"; DestName: dynamic-library-wx-enabled.project; DestDir: "{app}\templates\projects\dynamic-library-wx-enabled"; Flags: ignoreversion ; 

; XML settings
Source: "{#CODELITE_ROOT}\Runtime\lexers\*.json"; DestDir: "{app}\lexers\"; Flags: ignoreversion ; 


; Copy the plugins (by name)
Source: "{#CODELITE_ROOT}\Runtime32\debuggers\DebuggerGDB.dll"; DestDir: "{app}\debuggers"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\abbreviation.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\CodeFormatter.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\ContinuousBuild.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Copyright.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\CppChecker.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Cscope.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\DatabaseExplorer.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\ExternalTools.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\git.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Gizmos.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Outline.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\snipwiz.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\QMakePlugin.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Subversion2.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\UnitTestPP.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\wxFormBuilder.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\ZoomNavigator.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\SFTP.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\CMakePlugin.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\CodeLiteDiff.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\LLDBDebugger.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\wxcrafter.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\WordCompletion.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\SpellCheck.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\PHP.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\WebTools.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\HelpPlugin.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\AutoSave.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Tail.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\EditorConfigPlugin.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\codelite_vim.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\PHPLint.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\PHPRefactoring.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\SmartCompletion.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Docker.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\LanguageServer.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\EOSWiki.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Rust.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 
Source: "{#CODELITE_ROOT}\Runtime32\plugins\Remote.dll"; DestDir: "{app}\plugins"; Flags: ignoreversion 

Source: "{#CODELITE_ROOT}\Runtime32\libwxsqlite3u.dll"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\libcodeliteu.dll"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\libplugin_sdku.dll"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\libdatabaselayersqliteu.dll"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\libwxshapeframeworku.dll"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\sdk\libssh\lib32\libssh.dll"; DestDir: "{app}\"; DestName: "libssh.dll"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\sdk\libssh\lib32\libzlib.dll"; DestDir: "{app}\"; DestName: "libzlib.dll"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\images\*"; DestDir: "{app}\images"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\images\*"; DestDir: "{app}\images"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\*.zip"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime32\*.zip"; DestDir: "{app}"; Flags: ignoreversion ; 
Source: "{#CODELITE_ROOT}\Runtime\locale\*"; DestDir: "{app}\locale"; Flags: recursesubdirs ; 
Source: "{#CODELITE_ROOT}\Runtime\gdb_printers\*"; DestDir: "{app}\gdb_printers"; Flags: recursesubdirs ; 
Source: "{#CODELITE_ROOT}\Runtime32\wxgui.zip";  DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#CODELITE_ROOT}\Runtime\PHP.zip";  DestDir: "{app}"; Flags: ignoreversion; 
Source: "{#CODELITE_ROOT}\Plugin\jsctags.zip";  DestDir: "{app}"; Flags: ignoreversion;
Source: "{#CODELITE_ROOT}\PHPRefactoring\phprefactor.phar";  DestDir: "{app}"; Flags: ignoreversion;

[Icons]
Name: "{group}\CodeLite "; Filename: "{app}\codelite.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram, CodeLite}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\CodeLite "; Filename: "{app}\codelite.exe"; WorkingDir: "{app}" ;Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\CodeLite"; WorkingDir: "{app}"; Filename: "{app}\codelite.exe"; Tasks: quicklaunchicon

[INI]

[Registry]
Root: HKLM; Subkey: "Software\codelite\settings"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: noerror
Root: HKLM; Subkey: "Software\codelite\settings"; ValueType: string; ValueName: "PluginsDir";  ValueData: "{app}\plugins"; Flags: noerror

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Run]
Filename: "{app}\codelite.exe"; Description: "Launch CodeLite"; Flags: postinstall nowait skipifsilent

[Code]
var
    // Globals
    sMinGWFolderName: String;
    bMinGWBackedUp: Boolean;
    
// Uninstall
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  case CurUninstallStep of
    usUninstall:
      begin
        RegDeleteKeyIncludingSubkeys(HKCR, '*\shell\Open With CodeLite');
        // Prompt the user to delete all his settings, default to "No"
        if MsgBox('Would you like to keep your user settings?', mbConfirmation, MB_YESNO or MB_DEFBUTTON1) = IDNO 
        then begin
            DelTree(ExpandConstant('{userappdata}') + '\codelite', True, True, True);
        end;
      end;
    usPostUninstall:
      begin
      end;
  end;
end;

//--------------------
// Uninstall
//--------------------

function GetUninstallString(): String;
var
  sUnInstPath: String;
  sUnInstallString: String;
begin
  sUnInstPath := 'Software\Microsoft\Windows\CurrentVersion\Uninstall\CodeLite_is1';
  sUnInstallString := '';
  if not RegQueryStringValue(HKLM, sUnInstPath, 'UninstallString', sUnInstallString) then
    RegQueryStringValue(HKCU, sUnInstPath, 'UninstallString', sUnInstallString);
  Result := sUnInstallString;
end;

function UnInstallOldVersion(): Integer;
var
    sUnInstallString: String;
    sUnInstallStringOld: String;
    iResultCode: Integer;
    
begin
    // Return Values:
    // 1 - uninstall string is empty
    // 2 - error executing the UnInstallString
    // 3 - successfully executed the UnInstallString

    // default return value
    Result := 0;

    sUnInstallString    := GetUninstallString();
    if sUnInstallString <> '' then begin
        sUnInstallString := RemoveQuotes(sUnInstallString);
    if Exec(sUnInstallString, '/SILENT /NORESTART /SUPPRESSMSGBOXES ','', SW_HIDE, ewWaitUntilTerminated, iResultCode) then
        Result := 3
    else
        Result := 2;
    end else
        Result := 1;
end;

function GetCodeLiteInstallPath(): String;
var
    sRegKey: String;
    sCodeLitePath: String;
begin
    sRegKey := 'Software\codelite\settings';
    sCodeLitePath := '';
    if not RegQueryStringValue(HKLM, sRegKey, 'InstallPath', sCodeLitePath) then
        RegQueryStringValue(HKCU, sRegKey, 'InstallPath', sCodeLitePath);
    Result := sCodeLitePath;
end;

function UnInstallVersion6(): Integer;
var
    sCodeLitePath: String;
    
begin
    if GetCodeLiteInstallPath() <> '' then
    BEGIN
        // Uninstall codelite 6.X
        sCodeLitePath := GetCodeLiteInstallPath()
        
        // Delete the following folders:
        DelTree(sCodeLitePath + '\plugins', True, True, True )
        DelTree(sCodeLitePath + '\lexers', True, True, True)
        DelTree(sCodeLitePath + '\debuggers', True, True, True)
        DelTree(sCodeLitePath + '\templates', True, True, True)
        DelTree(sCodeLitePath + '\gdb_printers', True, True, True)
        DelTree(sCodeLitePath + '\config', True, True, True)
        DelTree(sCodeLitePath + '\dics', True, True, True)
        DelTree(sCodeLitePath + '\locale', True, True, True)

        // Clear the content of CodeLite installation folder (*.exe, *.zip, *.dll)
        DelTree(sCodeLitePath + '\*.exe', False, True, False)
        DelTree(sCodeLitePath + '\*.zip', False, True, False)
        DelTree(sCodeLitePath + '\*.dll', False, True, False)
        DelTree(sCodeLitePath + '\*.RPT', False, True, False)
        DelTree(sCodeLitePath + '\*.ini', False, True, False)
        DelTree(sCodeLitePath + '\*.html', False, True, False)
        DelTree(sCodeLitePath + '\*.dat', False, True, False)
        
        if MsgBox('Would you like to keep your local settings?', mbConfirmation, MB_YESNO or MB_DEFBUTTON1) = IDNO 
        then 
        BEGIN
            DelTree(ExpandConstant('{userappdata}') + '\codelite', True, True, True)
        END
    END
end;

function IsUpgrade(): Boolean;
begin
  Result := (GetUninstallString() <> '');
end;

function GetMinGWInstallLocation(): String;
var
    sMinGWPath: String;
    sMinGWPathLocation: String;
begin
    sMinGWPath := 'Software\codelite\settings';
    sMinGWPathLocation := '';
    if not RegQueryStringValue(HKLM, sMinGWPath, 'MinGW', sMinGWPathLocation) then
        RegQueryStringValue(HKCU, sMinGWPath, 'MinGW', sMinGWPathLocation);
    Result := sMinGWPathLocation;
end;

//-------------------------------------------------------
// Backup MinGW installation before uninstalling CodeLite
//-------------------------------------------------------
//procedure BackupMinGW();
//var
//    sMinGWLocation: String;
//    sTempFolder: String;
//    sMinGWBackupPath: String;
//    sMinGWDrive: String;
//    
//begin
//    bMinGWBackedUp := False;
//    sMinGWLocation := GetMinGWInstallLocation();
//    if sMinGWLocation <> '' then
//    begin
//        sMinGWFolderName := ExtractFileName(sMinGWLocation);
//        sMinGWDrive := ExtractFileDrive(sMinGWLocation);
//        sMinGWBackupPath := sMinGWDrive + '\' + sMinGWFolderName + '.backup'; // Rename the folder so it won't get deleted
//        bMinGWBackedUp := RenameFile(sMinGWLocation, sMinGWBackupPath);
//    end;
//end;

//----------------------------------------------------
// Restore MinGW from a previously back up
//----------------------------------------------------
//procedure RestoreMinGW();
//var
//    sMinGWLocation: String;
//    sTempFolder: String;
//    sMinGWBackupPath: String;
//    sMinGWNewLocation: String;
//begin
//    sMinGWLocation := GetMinGWInstallLocation();
//    sMinGWNewLocation := sMinGWLocation; // By default restore to the old location
//    if (sMinGWLocation <> '') AND (bMinGWBackedUp = True) then
//    BEGIN
//        sMinGWBackupPath := GetEnv('TEMP') + '\' + sMinGWFolderName;
//        if not CreateDir(sMinGWNewLocation) then
//        BEGIN
//            // Could not create the restore location, prompt the user
//            sMinGWNewLocation := ''
//            if BrowseForFolder('Could not restore MinGW to its previous location' + #13#10 + 'Please select a new location to restore MinGW (Setup will restore it into a separate folder):', sMinGWNewLocation, False) = True then
//            BEGIN
//                sMinGWNewLocation := sMinGWNewLocation + sMinGWFolderName;
//            END
//        END else BEGIN
//            // We can safely restore the folder
//            RemoveDir(sMinGWNewLocation)
//        END
//    END
//
//	if sMinGWNewLocation <> '' then
//    BEGIN
//        RenameFile(sMinGWBackupPath, sMinGWNewLocation)
//    END
//end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
begin
    case CurStep of
    ssInstall:
        begin
          if (IsUpgrade()) then
            begin
                // Uninstall CodeLite
                UnInstallVersion6()
            end;
        end;
    end;
end;
