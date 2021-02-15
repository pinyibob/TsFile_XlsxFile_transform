#include "ExcelRW.h"
#include <qvariant.h>
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"

const std::string ExcelRW::CLASS_TAG = "CLASS_DIVIDE_TAG";
int ExcelRW::CURRENT_ROW = 1;

#define TS_TAG ExcelRW::CLASS_TAG
#define CUR_ROW ExcelRW::CURRENT_ROW

ExcelRW::ExcelRW()
{
}

void ExcelRW::xlsxWrite(const QString& targetPath, const TsFileData& data)
{
    CUR_ROW = 1;
    QXlsx::Document xlsx(targetPath);
    //write begin
    initExcelHead(xlsx);
    for (const auto& item : data.contexts) {
        //write class divide tag
        QXlsx::Format format1;
        format1.setFontColor(QColor(Qt::red));
        //xlsx.write(CUR_ROW, 1, const_cast<std::string&>(TS_TAG), format1);
        xlsx.write(CUR_ROW, 1, QString::fromStdString(TS_TAG), format1);
        xlsx.write(CUR_ROW, 2, item.className,format1);
        xlsx.setRowHidden(CUR_ROW, true);
        CUR_ROW++;//next line

        for (const auto& item_ : item.messages) {
            bool whetherHaveLocation = false;
            QList<QString>::const_iterator it1  = item_.cpp_ui_fileNames.cbegin();
            QList<QString>::const_iterator it2  = item_.lines.cbegin();
            for (; it1 != item_.cpp_ui_fileNames.cend() && it2 != item_.lines.cend(); ++it1,++it2) {
                xlsx.write(CUR_ROW, ExcelRW::location, *it1 + "_" + *it2);
                xlsx.write(CUR_ROW, ExcelRW::source, item_.source);
                xlsx.write(CUR_ROW, ExcelRW::translationType, item_.translationType);
                xlsx.write(CUR_ROW, ExcelRW::current_translation, item_.translation);
                ++CUR_ROW; 
                whetherHaveLocation = true;
            }       
            if (!whetherHaveLocation) {
                xlsx.write(CUR_ROW, ExcelRW::location, "");
                xlsx.write(CUR_ROW, ExcelRW::source, item_.source);
                xlsx.write(CUR_ROW, ExcelRW::translationType, item_.translationType);
                xlsx.write(CUR_ROW, ExcelRW::current_translation, item_.translation);
                ++CUR_ROW; 
            }
        }
    }//end write
    CUR_ROW = 1;
    xlsx.saveAs(targetPath);
}

TsFileData ExcelRW::xlsxRead(const QString& filePath)
{
    TsFileData data;

    CUR_ROW = 2;
    QXlsx::Document xlsx(filePath);

    auto xlsxRead = [&xlsx](int row, int line)->QString {
        return xlsx.read(row, line).toString();
    };
#define FIRST_COL xlsxRead(CUR_ROW,ExcelRW::location).toStdString()
    while (FIRST_COL == TS_TAG) {
        tsContext curContext;
        curContext.className = xlsxRead(CUR_ROW, ExcelRW::source);
        CUR_ROW++;
        while (FIRST_COL != TS_TAG && !xlsxRead(CUR_ROW,ExcelRW::source).isEmpty()) {
            tsMessage CurMsg;
            auto locationData = xlsxRead(CUR_ROW, ExcelRW::location).split("_");
            if (locationData.isEmpty()) {
                CurMsg.cpp_ui_fileNames.push_back(locationData[0]);
                CurMsg.lines.push_back(locationData[1]);//location end
            }

            CurMsg.source = xlsxRead(CUR_ROW, ExcelRW::source);
            CurMsg.translationType = xlsxRead(CUR_ROW, ExcelRW::translationType);
            CurMsg.translation = xlsxRead(CUR_ROW, ExcelRW::current_translation);
            //if there's new translation, use it instead
            QString NewTranslation = xlsxRead(CUR_ROW, ExcelRW::new_translation);
            if (!NewTranslation.isEmpty()) {
                CurMsg.translation = NewTranslation;
            }

            curContext.messages.push_back(std::move(CurMsg));
            ++CUR_ROW;
        }//msg read end
        data.contexts.push_back(std::move(curContext));
    }//xlsx read end;
#undef FIRST_COL

    return std::move(data);
}

void ExcelRW::initExcelHead(QXlsx::Document& writer)
{
    writer.write(CUR_ROW, ExcelRW::location, "Location");
    writer.write(CUR_ROW, ExcelRW::source, "Source");
    writer.write(CUR_ROW, ExcelRW::translationType, "TranslationType");
    writer.write(CUR_ROW, ExcelRW::current_translation, "CurrentTranslation");
    writer.write(CUR_ROW, ExcelRW::new_translation, "NewTranslation");

    CUR_ROW++;
}
