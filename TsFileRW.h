#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QVector>

using namespace std;

struct tsContext;
struct tsMessage;

struct TsFileData {
    QString tsVersion = "2.1";
    QString encoding = "utf-8";
    QString xmlVersion = "1.0";
    QList<tsContext>contexts;
};

struct tsContext {
    QString className;
    QList<tsMessage> messages;
};

struct tsMessage {
    QList<QString> cpp_ui_fileNames;
    QList<QString> lines;
    QString source;
    QString translation;
    QString translationType;
};

class QXmlStreamReader;
class QXmlStreamWriter;
class QDomDocument;
class QDomElement;
class TsFileRW : public QObject
{
    Q_OBJECT
public:
    TsFileRW(QString tsFilePath = "");

    const TsFileData& TsRead(QString srcFileAbsolutePath);
    void TsWrite(QString targetFileAbsolutePath);
    
    void tsFileDataSend(const TsFileData& data);
private:
    tsContext contextRead(QXmlStreamReader&) const;
    tsMessage msgRead(QXmlStreamReader&) const;

    void contextWrite(QDomElement& parent, const tsContext&, QDomDocument&);
    void msgAppend(QDomElement& parent, const tsMessage& curMsg, QDomDocument& doc);
    //tool function
    void AppendElementWithAttribute(QDomElement& parent, const QString& elementName,
        const QVariantMap& ElementAttriMap, QDomDocument& doc);

private:
    TsFileData m_TsData;
    QString m_srcFilePath;
    QString m_FileSign;
};

