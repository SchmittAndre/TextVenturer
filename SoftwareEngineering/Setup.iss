; -- Languages.iss --
; Demonstrates a multilingual installation.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=TextVenturer
AppId=TextVenturer
AppVerName={cm:MyAppVerName,1.0}
DefaultDirName={pf}\{cm:MyAppName}
DefaultGroupName={cm:MyAppName}
UninstallDisplayIcon={app}data\icon\Icon.ico
VersionInfoDescription=TextVenturer Setup
VersionInfoProductName=TextVenturer
OutputDir=C:\Users\Andre\Documents\GitHub\TextVenturer\SoftwareEngineering
ArchitecturesInstallIn64BitMode=x64
ChangesAssociations = yes

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: de; MessagesFile: "compiler:Languages\German.isl"

[Messages]
en.BeveledLabel=English
de.BeveledLabel=Deutsch

[CustomMessages]
en.MyDescription=My description
en.MyAppName=TextVenturer
en.MyAppVerName=TextVenturer %1
de.MyDescription=Meine Beschreibung
de.MyAppName=TextVenturer
de.MyAppVerName=TextVenturer %1

[Files]
Source: "x64\Release\TextVenturer.exe"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "x64\Release\glew32.dll"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "Win32\Release\TextVenturer.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: solidbreak
Source: "Win32\Release\TextVenturer.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: solidbreak
Source: "TextVenturer\data\adventure\Tutorial.txvs"; DestDir: "{app}\data\adventure"
Source: "TextVenturer\data\adventure\EscapeRoom.txvs"; DestDir: "{app}\data\adventure"
Source: "TextVenturer\data\adventure\the quest for the bow.txvs"; DestDir: "{app}\data\adventure"
Source: "TextVenturer\data\AsciiArt\logo.txva"; DestDir: "{app}\data\AsciiArt"
Source: "TextVenturer\data\font\font.pdn"; DestDir: "{app}\data\font"
Source: "TextVenturer\data\font\font.png"; DestDir: "{app}\data\font"
Source: "TextVenturer\data\icon\Icon.ico"; DestDir: "{app}\data\icon"
Source: "TextVenturer\data\shader\test.fs"; DestDir: "{app}\data\shader"
Source: "TextVenturer\data\shader\test.vs"; DestDir: "{app}\data\shader"


[Dirs]
Name: "{app}\data"
Name: "{app}\data\compiled"

[Icons]
Name: "{group}\{cm:MyAppName}"; Filename: "{app}\TextVenturer.exe"
Name: "{group}\{cm:UninstallProgram,{cm:MyAppName}}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\{cm:MyAppName}"; Filename: "{app}\{cm:MyAppName}"; Tasks: desktopicon

[Tasks]
; The following task doesn't do anything and is only meant to show [CustomMessages] usage
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: associate; Description: "&Associate files (beta)"; GroupDescription: "Other tasks:"; Flags: unchecked


[Registry]
; Start "Software\My Company\My Program" keys under HKEY_CURRENT_USER
; and HKEY_LOCAL_MACHINE. The flags tell it to always delete the
; "My Program" keys upon uninstall, and delete the "My Company" keys
; if there is nothing left in them.
Root: HKCR; Subkey: ".txvs"; Flags: uninsdeletekey; Tasks: associate
Root: HKCR; Subkey: ".txvs"; ValueData: "{cm:MyAppName}"; Flags: uninsdeletevalue; ValueType: string; ValueName: ""; Tasks: associate
Root: HKCR; Subkey: "{cm:MyAppName}"; ValueData: "Program {cm:MyAppName}";  Flags: uninsdeletekey;   ValueType: string;  ValueName: ""; Tasks: associate
Root: HKCR; Subkey: "{cm:MyAppName}\DefaultIcon"; ValueData: "{app}\{cm:MyAppName},0";               ValueType: string;  ValueName: ""; Tasks: associate
Root: HKCR; Subkey: "{cm:MyAppName}\shell\open\command"; ValueData: """{app}\{cm:MyAppName}"" ""%1""";  ValueType: string;  ValueName: ""; Tasks: associate
