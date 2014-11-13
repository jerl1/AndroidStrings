#include "androidstringreader.h"

#include <QXmlStreamAttributes>

const QString AndroidStringReader::xml_string("string");
const QString AndroidStringReader::xml_string_array("string-array");
const QString AndroidStringReader::xml_plurals("plurals");

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
            if(name() == "ressources")
                continue;

            bool add = false;
            AndroidString str;
            str.setAndroidLabel(attributes().value("name").toString());

            if (name() == xml_string) {
                add = readString(str);
            } else if (name() == xml_string_array) {
                add = readStringArray(str);
            } else if (name() == xml_plurals) {
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
    return readEndToken(str, xml_string);
}

bool AndroidStringReader::readStringArray(AndroidString &str)
{
    str.setType(AndroidString::TypeArray);
    return readEndToken(str, xml_string_array);
}

bool AndroidStringReader::readQuantity(AndroidString &str)
{
    str.setType(AndroidString::TypeQuantity);
    return readEndToken(str, xml_plurals);
}

static void append(AndroidString &astr, QString &str)
{
    str = str.simplified();
    if ((str.size() > 0) && (str[0] == '"'))
        str = str.mid(1, str.size() - 2);

    astr.appendTranslation(str);
}

bool AndroidStringReader::readEndToken(AndroidString &str, QString endToken)
{
    QString item;
    while (!atEnd() && !hasError()) {
        QXmlStreamReader::TokenType token = readNext();
        switch (token) {
            case QXmlStreamReader::Characters:
                if (isWhitespace() == false)
                    item.append(text().string());
                break;

            case QXmlStreamReader::StartElement:
                if (endToken == xml_plurals) {
                    QString quantity = attributes().value("quantity").toString();
                    if (quantity.size() > 0)
                        item += quantity + ": ";
                }
                if (name() == "g") {
                    //This is a xliff:g
                    item += readElementText();
                }

                break;

            case QXmlStreamReader::EndElement:
                if (name() == endToken) {
                    if (item.size() > 0) {
                        append(str, item);
                    }
                    return true;
                } else {
                    append(str, item);
                    item.clear();
                }
                break;

            case QXmlStreamReader::Invalid:
                qDebug(qPrintable(errorString()));
                return false;
            default:
                break;
        }
    }

    return !hasError();
}
