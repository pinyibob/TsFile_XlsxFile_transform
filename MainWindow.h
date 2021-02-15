#pragma once
#include <qwidget.h>

class QPushButton;
class QLabel;
class TsFileRW;
class ExcelRW;
class QLineEdit;
class MainWindow : public QWidget
{
public:
    MainWindow(QWidget* parent = nullptr);
    void initConnect();
private:
    TsFileRW* m_pTsRW = nullptr;
    ExcelRW* m_pExcelRW = nullptr;

    QLabel* m_pLabel;
    QPushButton* m_pTsToExcelBtn = nullptr;
    QPushButton* m_pExcelToTsBtn = nullptr;

    QLineEdit* m_pLconvertPath = nullptr;
    QPushButton* m_pToolPathBtn = nullptr;
};

