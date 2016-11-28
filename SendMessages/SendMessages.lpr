program SendMessages;

{$APPTYPE CONSOLE}

uses
  Windows, RegEx;

function FoundWindow(AWND: HWND; param: LPARAM): BOOL; stdcall;
var
  Text: String;
  Len: LongInt;
  Offset, Position: integer;
begin
  Len := GetWindowTextLength(AWND) + 1;
  if Len > 1 then
  begin
    SetLength(Text, Len);
    GetWindowText(AWND, @Text[1], Len);
    Offset := 0;
    with TRegexEngine.Create('eingabe') do
    begin
      IgnoreCase := True;
      if MatchString(Text, Position, Offset) then
      begin
        (PHANDLE(param))^ := AWND;
        WriteLn('Found: ' + Text);
      end;
      Free;
    end;
  end;
  Result := True;
end;

var
  Window: HWND;
  C: Char;
  Text: String;
  PID, Process: DWORD;

begin

  Window := 0;

  EnumWindows(FoundWindow, LPARAM(@Window));

  if Window = 0 then
    WriteLn('Couldn''t find window!');

  ReadLn(Text);

  ShowWindow(Window, SW_RESTORE);

  //PostMessage(Window, WM_CLOSE, 0, 0);
  for C in Text do
  begin
    SendMessage(Window, WM_CHAR, WPARAM(C), 0);
  end;

  SendMessage(Window, WM_KEYDOWN, VK_RETURN, 0);


  WriteLn('Sent "' + Text + '"');

  Readln;

end.

