#include "TsFileRW.h"

#include <iostream>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>
#include <QDomDocument>
#include <QCoreApplication>
#include <QDir>
#include <qprocess.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <QList>
#include <QVector>
//set qt Tools path to trans tsfile to standard and you can use -locations none cmd to clean the location information to avoid
//confict on git;
QString Qt_lconvert_path = "D:/qt5.90/5.9/msvc2017_64/bin/lconvert.exe";
#define QT_LCONVERT_PATH Qt_lconvert_path;
//#define TS_LINES_INFO_VISIBLE true;

TsFileRW::TsFileRW(QString tsFilePath)
{
    m_srcFilePath = tsFilePath;
}

const TsFileData& TsFileRW::TsRead(QString srcFile)
{
    m_srcFilePath = srcFile;
    //code for test
    //auto dirPath = QCoreApplication::applicationDirPath();
    //QDir dir(dirPath);
    //dir.cd("../Translation");
    //auto tsFileInfo = dir.entryInfoList(QStringList("*.ts"), QDir::Filter::Files, QDir::NoSort);
    //auto fileName = tsFileInfo[0].absoluteFilePath();

    QFile tsFile(srcFile);
    if (!tsFile.open(QFile::ReadOnly | QFile::Text))
        return m_TsData;

    QXmlStreamReader reader(&tsFile);
    while (!reader.atEnd())
    {
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement:
        {
            if (reader.name() == "TS") {
                m_TsData.tsVersion = reader.attributes().value("version").toString();

                while (reader.readNextStartElement()) {
                    m_TsData.contexts.push_back(contextRead(reader));
                }
            }
            break;
        }
        case QXmlStreamReader::StartDocument:
            m_TsData.xmlVersion = reader.documentVersion().toString();
            m_TsData.encoding = reader.documentEncoding().toString();
            break;
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::EndElement:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
        default:
            break;
        }
    }
    if (reader.error() > 0) {
        qDebug() << "read error occured! QAQ\n location:\trow:" + reader.lineNumber() + QString("line:") + reader.columnNumber();
        qDebug() << "error type:" << reader.error();
    }
    //QMessageBox::information(nullptr, "read success","one file parser finished",QMessageBox::StandardButton::Ok);
    qDebug() << "one file parser finished";

    return m_TsData;
}

tsContext TsFileRW::contextRead(QXmlStreamReader& reader) const
{
    tsContext curTsContent;
    while (reader.readNextStartElement()) {
        if (reader.name().toString() == "name")
        {
            curTsContent.className = reader.readElementText();
        }
        else if (reader.name() == "message") {
            curTsContent.messages.push_back(msgRead(reader));
        }
        else {
            reader.skipCurrentElement();
        }
    }
    return std::move(curTsContent);
}

tsMessage TsFileRW::msgRead(QXmlStreamReader& reader) const
{
    tsMessage curMsg;

    while (reader.readNextStartElement()) {
        if (reader.name() == "location") {
            curMsg.cpp_ui_fileNames.push_back(reader.attributes().value("filename").toString());
            curMsg.lines.push_back(reader.attributes().value("line").toString());
            reader.skipCurrentElement();//to the endElement
        }
        if (reader.name() == "source") {
            curMsg.source = reader.readElementText();//read all and to the endElement
        }
        if (reader.name() == "translation") {
            curMsg.translationType = reader.attributes().value("type").toString();
            curMsg.translation = reader.readElementText();
        }
    }
    return std::move(curMsg);
}

void TsFileRW::TsWrite(QString targetFile_AbsolutePath)
{
    //code for test
    //targetFile_AbsolutePath = "C:/Users/A/Documents/TranslationTools/writeTsTest.ts";

    QFile tsFile(targetFile_AbsolutePath);

    QDomDocument doc;
    //xml info
    doc.appendChild(doc.createProcessingInstruction("xml", QString("version=\"%1\" encoding=\"%2\"")
        .arg(m_TsData.xmlVersion).arg(m_TsData.encoding)));

    //note
    auto FileComment = doc.createComment("DOCTYPE TS");
    doc.appendChild(FileComment);

    //ts version
    auto versionAttri = doc.createElement("TS");
    versionAttri.setAttribute("version", m_TsData.xmlVersion);
    doc.appendChild(versionAttri);

    for (const auto& item : m_TsData.contexts) {
    //contexts write
        auto contextNode = doc.createElement("context");
        versionAttri.appendChild(contextNode);
        contextWrite(contextNode, item, doc);
    }

    if (!tsFile.open(QIODevice::WriteOnly)) {
        return;
    }
    QTextStream output(&tsFile);
    doc.save(output, QDomNode::EncodingFromTextStream);

    QString QtToolPath = QT_LCONVERT_PATH;
    QFile convertTool(QtToolPath);
    if (!convertTool.exists())
        return;

    QString cmd = QDir::toNativeSeparators(QtToolPath) + " " + "-locations none" + " -i " 
        + QDir::toNativeSeparators(targetFile_AbsolutePath) + " -o "
        + QDir::toNativeSeparators(targetFile_AbsolutePath);
   
    QProcess::execute(cmd);
}

void TsFileRW::tsFileDataSend(const TsFileData& data)
{
    m_TsData = data;
}

void TsFileRW::contextWrite(QDomElement& parent, const tsContext& curContext, QDomDocument& doc)
{
    auto nameE = doc.createElement("name");
    auto nameT = doc.createTextNode(curContext.className);
    nameE.appendChild(nameT);
    parent.appendChild(nameE);
    for (const auto& item_ : curContext.messages) {
        msgAppend(parent, item_, doc);
    }
}

void TsFileRW::msgAppend(QDomElement& parent, const tsMessage& curMsg, QDomDocument& doc)
{
    auto msgE = doc.createElement("message");
    parent.appendChild(msgE);

    for (int i = 0; i < curMsg.cpp_ui_fileNames.size(); ++i) {
        QVariantMap FileInfoMap;
        FileInfoMap.insert("filename",curMsg.cpp_ui_fileNames[i]);
        FileInfoMap.insert("line", curMsg.lines[i]);
        AppendElementWithAttribute(msgE, "location", FileInfoMap, doc);
    }

    auto sourceE = doc.createElement("source");
    sourceE.appendChild(doc.createTextNode(curMsg.source));
    msgE.appendChild(sourceE);

    QVariantMap transMap;
    auto transE = doc.createElement("translation");
    transE.setAttribute("type", curMsg.translationType);
    transE.appendChild(doc.createTextNode(curMsg.translation));//translation element end
    msgE.appendChild(transE);
}

void TsFileRW::AppendElementWithAttribute(QDomElement& parent, const QString& elementName, const QVariantMap& ElementAttriMap, QDomDocument& doc)
{
    auto newElement = doc.createElement(elementName);
    for (const auto& item : ElementAttriMap.keys()) {
        //in case of fault input
        if (item.isEmpty() || ElementAttriMap[item].toString().isEmpty())
            return;

        newElement.setAttribute(item, ElementAttriMap[item].toString());
    }
    parent.appendChild(newElement);
}
