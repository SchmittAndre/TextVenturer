#include "stdafx.h"

ErrorDialog::ErrorDialog(string title, string message)
{
    MessageBox(NULL, CA2CT(message.c_str()), CA2CT(title.c_str()), MB_ICONERROR);
}

ErrorDialog::ErrorDialog(string message)
{
    ErrorDialog("Error", message);
}
