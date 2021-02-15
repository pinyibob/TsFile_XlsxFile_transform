#pragma once
#include "TsFileRW.h"
#include "xlsxdocument.h"
#include <string>
class ExcelRW
{
public:
    ExcelRW();
    void xlsxWrite(const QString& targetPath, const TsFileData& data);
    TsFileData xlsxRead(const QString& filePath);

private:
    //static member for xlsxReading and Writing
    static int CURRENT_ROW;
    static const string CLASS_TAG;

    static const int location = 1;
    static const int source = 2;
    static const int translationType = 3;
    static const int current_translation = 4;
    //write in the new translate and replace the origin one, even if it's empty;
    static const int new_translation = 5;

private:
    void initExcelHead(QXlsx::Document& writer);

};
