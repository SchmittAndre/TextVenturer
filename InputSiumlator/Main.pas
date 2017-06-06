unit Main;

interface

uses
  Classes, SysUtils, LazFileUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls, Windows, RegExpr;

type

  { TFullPath }

  TFullPath = class
    Path: String;
    constructor Create(APath: String);
  end;

  { TfrmMain }

  TfrmMain = class(TForm)
    btnAddScripts: TButton;
    btnStartScript: TButton;
    edtAppName: TEdit;
    gbFindApp: TGroupBox;
    gbSelectScript: TGroupBox;
    lbErrorMessage: TLabel;
    lbApps: TListBox;
    lbScripts: TListBox;

    procedure btnAddScriptsClick(Sender: TObject);
    procedure btnStartScriptClick(Sender: TObject);
    procedure edtAppNameChange(Sender: TObject);
    procedure edtAppNameKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure lbAppsDblClick(Sender: TObject);
    procedure lbScriptsDblClick(Sender: TObject);
    procedure lbScriptsMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);

  private

    FFindString: TRegExpr;

    procedure AddWindow(AName: String; AWindow: HWND);
    procedure RefreshApps;
    procedure ShowWnd(const Wnd: HWND);

    class function WindowEnumFunc(AWindow: HWND; ALParam: LPARAM): BOOL; static; stdcall;

    procedure RunScript(AFileName: String; AWnd: HWND);

  end;

var
  frmMain: TfrmMain;

implementation

{$R *.lfm}

{ TFullPath }

constructor TFullPath.Create(APath: String);
begin
  Path := APath;
end;

{ TfrmMain }

procedure TfrmMain.RefreshApps;
begin
  lbApps.Items.BeginUpdate;
  lbApps.Clear;
  lbApps.ScrollWidth := 0;
  EnumWindows(WindowEnumFunc, 0);
  lbApps.Items.EndUpdate;
end;

procedure TfrmMain.ShowWnd(const Wnd: HWND);
var
  WindowLong: LONG;
begin
  WindowLong := GetWindowLong(Wnd, GWL_STYLE);
  if not (WindowLong and WS_VISIBLE <> 0) then
    Exit;
  if (WindowLong and WS_MINIMIZE) <> 0 then
    ShowWindow(Wnd, SW_RESTORE)
  else
    SetForegroundWindow(Wnd);
end;

procedure TfrmMain.edtAppNameChange(Sender: TObject);
var
  E: ERegExpr;
begin
  if Trim(edtAppName.Text) = '' then
    FFindString.Expression := '.*'
  else
    FFindString.Expression := UpperCase(edtAppName.Text);

  try
    FFindString.Compile;
    RefreshApps;

    if lbApps.Count = 0 then
    begin
      lbErrorMessage.Visible := True;
      lbErrorMessage.Caption := 'No match!';
      edtAPpName.Font.Color := clRed;
    end
    else
    begin
      lbErrorMessage.Visible := False;
      edtAppName.Font.Color := clDefault;
    end;
  except
    on E : ERegExpr do
    begin
      lbErrorMessage.Visible := True;
      lbErrorMessage.Caption := Copy(E.Message, 17, Length(E.Message));
      edtAppName.Font.Color := clRed;
      FFindString.Expression := '.*';
      RefreshApps;
    end;
  end;
end;

procedure TfrmMain.btnStartScriptClick(Sender: TObject);
begin
  if lbScripts.ItemIndex = -1 then
  begin
    MessageBox(Handle, 'Please select a Script!', nil, MB_ICONERROR);
    Exit;
  end;
  if lbApps.ItemIndex = -1 then
  begin
    MessageBox(Handle, 'Please select an Application!', nil, MB_ICONERROR);
    Exit;
  end;
  RunScript(
    TFullPath(lbScripts.Items.Objects[lbScripts.ItemIndex]).Path,
    HWND(lbApps.Items.Objects[lbApps.ItemIndex])
  );
end;

procedure TfrmMain.btnAddScriptsClick(Sender: TObject);
var
  F: String;
begin
  with TOpenDialog.Create(Self) do
  begin
    Filter := 'Input Simulation|*.ips';
    Options := Options + [ofAllowMultiSelect, ofFileMustExist];
    if Execute then
      for F in Files do
        lbScripts.AddItem(ExtractFileNameOnly(F), TFullPath.Create(F));
  end;
end;

procedure TfrmMain.edtAppNameKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
begin
  if Key = VK_RETURN then
    RefreshApps;
end;

procedure TfrmMain.FormCreate(Sender: TObject);
begin
  FFindString := TRegExpr.Create('.*');
  RefreshApps;
  Update;
end;

procedure TfrmMain.FormDestroy(Sender: TObject);
var
  I: Integer;
begin
  for I := 0 to lbScripts.Count - 1 do
    lbScripts.Items.Objects[I].Free;
  FFindString.Free;
end;

procedure TfrmMain.lbAppsDblClick(Sender: TObject);
var
  Wnd: HWND;
begin
  if lbApps.ItemIndex = -1 then
    Exit;
  Wnd := HWND(lbApps.Items.Objects[lbApps.ItemIndex]);
  ShowWnd(Wnd);
end;

procedure TfrmMain.lbScriptsDblClick(Sender: TObject);
begin
  if lbScripts.ItemIndex <> -1 then
    btnStartScriptClick(btnStartScript);
end;

procedure TfrmMain.lbScriptsMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  I: Integer;
begin
  if Button = mbRight then
  begin
    I := lbScripts.GetIndexAtY(Y);
    if I <> -1 then
    begin
      lbScripts.Items.Objects[I].Free;
      lbScripts.Items.Delete(I);
    end;
  end;
end;

procedure TfrmMain.AddWindow(AName: String; AWindow: HWND);
begin
  lbApps.AddItem(AName, TObject(AWindow));
  lbApps.ScrollWidth := Max(lbApps.ScrollWidth, lbApps.Canvas.TextWidth(AName));
end;

class function TfrmMain.WindowEnumFunc(AWindow: HWND; ALParam: LPARAM): BOOL; stdcall;
var
  L, WindowLong: Integer;
  PCharName: PChar;
  Name: String;
begin
  L := GetWindowTextLength(AWindow) + 1;
  PCharName := GetMem(L);
  GetWindowText(AWindow, PCharName, L);

  WindowLong := GetWindowLong(AWindow, GWL_STYLE);
  if (WindowLong and WS_VISIBLE <> 0) then
  begin
    Name := PCharName;
    if (Name <> '') and (frmMain.FFindString.Exec(UpperCase(Name))) then
      frmMain.AddWindow(Name, AWindow);
  end;
  FreeMem(PCharName);
  Result := True; // true = continue enumeration, false = break enumeration
end;

procedure TfrmMain.RunScript(AFileName: String; AWnd: HWND);

  function StartsWith(AString, AStart: String; out ARest: String): Boolean;
  begin
    AStart := AStart;
    ARest := Trim(Copy(AString, Length(AStart) + 2, Length(AString)));
    Result := Copy(AString, 1, Length(AStart)) = AStart;
  end;

var
  Script: TStringList;
  Line, Data: String;
  I: Integer;
  C: Char;
  Num: Single;
  FrmtSettings: TFormatSettings;
begin
  if not FileExists(AFileName) then
  begin
    MessageBox(Handle, 'File not found!', nil, MB_ICONWARNING);
    Exit;
  end;

  Script := TStringList.Create;
  Script.LoadFromFile(AFileName);

  FrmtSettings.DecimalSeparator := '.';

  ShowWnd(AWnd);

  for I := 0 to Script.Count - 1 do
  begin
    Line := Script[I];
    if StartsWith(Line, 'type', Data) then
    begin
      for C in Data do
        PostMessage(AWnd, WM_CHAR, WPARAM(C), 0);
      PostMessage(AWnd, WM_KEYDOWN, WPARAM(VK_RETURN), 0)
    end
    else if StartsWith(Line, 'sleep', Data) then
    begin
      if TryStrToFloat(Data, Num, FrmtSettings) then
      begin
        Sleep(Round(Num * 1000));
      end
      else
      begin
        MessageBox(Handle, PChar(Format('Invalid sleep time %s', [Data])), nil, MB_ICONERROR);
      end;
    end
    else if StartsWith(Line, 'return', Data) then
    begin
      PostMessage(AWnd, WM_KEYDOWN, WPARAM(VK_RETURN), 0);
    end
    else if not StartsWith(Line, '//', Data) then
    begin
      MessageBox(Handle, PChar(Format('Unknown command in line %d', [I + 1])), nil, MB_ICONERROR);
      Break;
    end;
  end;

  Script.Free;

end;

end.

