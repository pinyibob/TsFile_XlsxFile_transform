#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdialog.h>
#include <ExcelRW.h>
#include <TsFileRW.h>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>


MainWindow::MainWindow(QWidget* parent)
    :QWidget(parent)
{
    resize(500,500);

    m_pExcelRW = new ExcelRW();
    m_pTsRW = new TsFileRW();

    m_pLabel = new QLabel(this);
    m_pTsToExcelBtn = new QPushButton(this);
    m_pExcelToTsBtn = new QPushButton(this);

    QHBoxLayout* hToolLay = new QHBoxLayout;
    m_pLconvertPath = new QLineEdit(this);
    m_pToolPathBtn = new QPushButton(this);
    m_pToolPathBtn->setText("LocateQtTool");
    hToolLay->addWidget(m_pToolPathBtn);
    hToolLay->addWidget(m_pLconvertPath);

    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->addWidget(m_pTsToExcelBtn);
    hLay->addWidget(m_pExcelToTsBtn);
    QVBoxLayout* fiLay = new QVBoxLayout(this);
    fiLay->addLayout(hToolLay);
    fiLay->addLayout(hLay);
    fiLay->addWidget(m_pLabel);

    m_pTsToExcelBtn->setText("TsToExcel");
    m_pExcelToTsBtn->setText("ExcelToTs");
    m_pLabel->setText("with this exe, u can transform .ts to .xlsx or reverse; And u can \
        put in new words in 'newTranslation volumn' in generated xlsxFiles to replace the old one and \
        create new;if u don't wanna location info, u can locate the lconvert.exe and create ts file again");
    m_pLabel->setWordWrap(true);
    m_pLabel->adjustSize();

    initConnect();
}

void MainWindow::initConnect()
{
    connect(m_pTsToExcelBtn, &QPushButton::clicked, this, [this]() {

            QString fileName = QFileDialog::getOpenFileName(nullptr, "find a ts file");
            QFileInfo file(fileName);
            
            if (fileName.endsWith(".ts")) {
                m_pExcelRW->xlsxWrite(fileName.replace(".ts",".xlsx"),m_pTsRW->TsRead(fileName));
            }

        });
    connect(m_pExcelToTsBtn, &QPushButton::clicked, this, [this]() {
        
            QString fileName = QFileDialog::getOpenFileName(nullptr, "find a excel file");
            if (fileName.endsWith(".xlsx")) {
                m_pTsRW->tsFileDataSend(m_pExcelRW->xlsxRead(fileName));
                m_pTsRW->TsWrite(fileName.replace(".xlsx",".ts"));
            }
        });
    connect(m_pToolPathBtn, &QPushButton::clicked, this,[this]() {
        auto path = QFileDialog::getOpenFileName(nullptr, "locate qt_lconvert.exe path to remove location info");
        m_pLconvertPath->setText(path);
        extern QString Qt_lconvert_path;
        Qt_lconvert_path = path;
        });

}
