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

    QList<AndroidString*> *mList;
    QString mPath;
    QString mLanguage;
};

#endif // ANDROIDSTRINGREADER_H
