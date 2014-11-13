#ifndef ANDROIDSTRINGREADER_H
#define ANDROIDSTRINGREADER_H

#include <QXmlStreamReader>
#include "androidstring.h"

class AndroidStringReader : public QXmlStreamReader
{
public:
    AndroidStringReader(QList<AndroidString*> *list, const QString &path);
    bool readFile(QIODevice *device);

private:
    void read();
    bool readString(AndroidString &str);
    bool readStringArray(AndroidString &str);
    bool readQuantity(AndroidString &str);
    bool readEndToken(AndroidString &str, QString endToken);

    QList<AndroidString*> *mList;
    QString mPath;
    QString mLanguage;

    static const QString xml_string;
    static const QString xml_string_array;
    static const QString xml_plurals;
};

#endif // ANDROIDSTRINGREADER_H
