#include <QApplication>
#include "ExcelRW.h"
#include "TsFileRW.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //ExcelRW xlsxRW;
    //TsFileRW tsRw;
    ////xlsxRW.xlsxWrite("xlsxWriteTest", tsRw.TsRead("C:/Users/A/Desktop/multiLangTools/build/chz.ts"));
    //tsRw.tsFileDataSend(xlsxRW.xlsxRead("xlsxWriteTest.xlsx"));
    //tsRw.TsWrite("C:/Users/A/Desktop/multiLangTools/build/Test.ts");

    MainWindow mainwindow;
    mainwindow.show();

    return a.exec();
}
