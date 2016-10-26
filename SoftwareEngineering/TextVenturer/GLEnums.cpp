#include "stdafx.h"

DWORD getDataSize(GLDataType dataType)
{
    switch (dataType) {
    case dtByte:
    case dtUByte:
        return 1;
    case dtShort:
    case dtUShort:
        return 2;
    case dtInt:
    case dtUInt:
    case dtFloat:
        return 4;
    case dtDouble:
        return 8;
    default:
        return 0;
    }
}