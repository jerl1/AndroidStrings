#include "androidstringreader.h"

AndroidStringReader::AndroidStringReader(QList<AndroidString*> *list, const QString &path)
{
    static const QString strValues("values");
    static const QString strUnknow("Unknow");
    static const QString strDefault("Default");
    mList = list;
    int valuesIndex = path.lastIndexOf(strValues);

    //Keep the path until strValues
    mPath = path;
    if(valuesIndex > 0)
        mPath.truncate(valuesIndex - 1);

    //Look for the language from the path
    if (valuesIndex == -1) {
        mLanguage = strUnknow;
    } else {
        QString lang = path.right(path.length() - valuesIndex);
        if (lang == strValues)
            mLanguage = strDefault;
        else
            mLanguage = lang.right(lang.length() - lang.indexOf('-') - 1);
    }
}

bool AndroidStringReader::readFile(QIODevice *device)
{
    setDevice(device);

    while (!atEnd() && !hasError()) {
        QXmlStreamReader::TokenType token = readNext();

        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }

        if(token == QXmlStreamReader::StartElement) {
            if(name() == "ressources") {
                continue;
            }
            bool add = false;
            AndroidString str;
            str.setAndroidLabel(attributes().value("name").toString());

            if (name() == "string") {
                add = readString(str);
            } else if (name() == "string-array") {
                add = readStringArray(str);
            } else if (name() == "plurals") {
                add = readQuantity(str);
            }

            if (add) {
                str.setPath(mPath);
                str.setLanguage(mLanguage);
                mList->append(new AndroidString(&str));
            }
        }
    }

    clear();
    return !hasError();
}

bool AndroidStringReader::readString(AndroidString &str)
{
    str.setType(AndroidString::TypeString);
    str.appendTranslation(readElementText());
    return true;
}

bool AndroidStringReader::readStringArray(AndroidString &str)
{
    str.setType(AndroidString::TypeArray);
    while (!atEnd() && !hasError()) {
        QXmlStreamReader::TokenType token = readNext();
        if(token == QXmlStreamReader::StartElement) {
            str.appendTranslation(readElementText());
        } else if(token == QXmlStreamReader::EndElement) {
            break;
        }
    }

    return true;
}

bool AndroidStringReader::readQuantity(AndroidString &str)
{
    //TODO
    str.setType(AndroidString::TypeQuantity);
    qDebug(qPrintable("Skipping quantity"));
    return false;
}
