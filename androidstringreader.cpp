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
        QString lang = path;
        lang.remove(mPath + '/');
        if (lang == strValues)
            mLanguage = strDefault;
        else {
            mLanguage = lang;
            mLanguage.remove(strValues + '-');
        }
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
    return readUntilEndToken(str, xml_string);
}

bool AndroidStringReader::readStringArray(AndroidString &str)
{
    str.setType(AndroidString::TypeArray);
    return readUntilEndToken(str, xml_string_array);
}

bool AndroidStringReader::readQuantity(AndroidString &str)
{
    str.setType(AndroidString::TypeQuantity);
    return readUntilEndToken(str, xml_plurals);
}

static void append(AndroidString &astr, QString &str)
{
    str = str.simplified();
    astr.appendTranslation(str);
}

bool AndroidStringReader::readUntilEndToken(AndroidString &str, QString endToken)
{
    QString fulltext;
    while (!atEnd() && !hasError()) {
        QXmlStreamReader::TokenType token = readNext();
        switch (token) {
            case QXmlStreamReader::Characters:
                fulltext.append(text().string());
                break;

            case QXmlStreamReader::StartElement:
                if (name() == "item") {
                    if (endToken == xml_plurals) {
                        QString quantity = attributes().value("quantity").toString();
                        if (quantity.size() > 0)
                            fulltext += quantity + ": ";
                    }
                } else if (name() == "g") {
                    //<xliff:g id="number" example="123">%1$s</xliff:g>
                    QString id = attributes().value("id").toString();
                    QString example = attributes().value("example").toString();
                    fulltext += QString("<xliff:g");
                    if (id.size() > 0) {
                        fulltext += QString(" id=\"%1\"").arg(id);
                    }
                    if (example.size() > 0) {
                        fulltext += QString(" example=\"%1\"").arg(example);
                    }
                    fulltext += QString(">");
                } else {
                    fulltext += QString("<%1>").arg(name().toString());
                }
                break;

            case QXmlStreamReader::EndElement:
                if (name() == endToken) {
                    if (name() == xml_string) {
                        //Already added when find end of 'item'
                        append(str, fulltext);
                    }
                    return true;
                } else if (name() == "item") {
                    append(str, fulltext);
                    fulltext.clear();
                } else if (name() == "g") {
                    //<xliff:g id="number" example="123">%1$s</xliff:g>
                    fulltext += QString("</xliff:g>");
                } else {
                    fulltext += QString("</%1>").arg(name().toString());
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
