#include "stdafx.h"

#include "ErrorDialog.h"

ErrorDialog::ErrorDialog(std::string title, std::string message)
{
    MessageBox(NULL, CA2CT(message.c_str()), CA2CT(title.c_str()), MB_ICONERROR);
}

ErrorDialog::ErrorDialog(std::string message)
{
    ErrorDialog("Error", message);
}
