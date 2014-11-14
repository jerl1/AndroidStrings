#include "androidstring.h"

const QChar AndroidString::CsvSeparator = ';';
const QChar AndroidString::quote = '"';

AndroidString::AndroidString(QObject *parent) :
    QObject(parent)
{
    mType = TypeString;
    mOverided = false;
}

AndroidString::AndroidString(const AndroidString *str, QObject *parent) :
    QObject(parent)
{
    setPath(str->path());
    setAndroidLabel(str->androidLabel());
    setLanguage(str->language());
    setTranslation(str->translation());
    setType(str->type());
    setOverided(str->overided());
}

QString AndroidString::path() const
{
    return mPath;
}

void AndroidString::setPath(const QString &path)
{
    mPath = path;
}

QString AndroidString::androidLabel() const
{
    return mAndroidLabel;
}

void AndroidString::setAndroidLabel(const QString &androidLabel)
{
    mAndroidLabel = androidLabel;
}

QString AndroidString::language() const
{
    return mLanguage;
}

void AndroidString::setLanguage(const QString &language)
{
    mLanguage = language;
}

QStringList AndroidString::translation() const
{
    return mTranslation;
}

void AndroidString::setTranslation(const QStringList &translation)
{
    mTranslation = translation;
}

void AndroidString::appendTranslation(const QString &translation)
{
    mTranslation.append(translation);
}

QString AndroidString::formatedTranslation() const
{
    if (mType == TypeString) {
        if (mTranslation.size() > 0)
            return mTranslation.at(0);
        else
            return "EMPTY";
    } else {
        return mTranslation.join("\n");
    }
}

AndroidString::AndroidStringType AndroidString::type() const
{
    return mType;
}

void AndroidString::setType(const AndroidStringType &type)
{
    mType = type;
}

bool AndroidString::sort(const AndroidString *as1, const AndroidString *as2)
{
    //Check path
    int resultInt = as1->path().compare(as2->path(), Qt::CaseSensitive);
    if (resultInt == 0) {
        //Check label
        resultInt = as1->androidLabel().compare(as2->androidLabel(), Qt::CaseInsensitive);
        if (resultInt == 0) {
            resultInt = as1->language().compare(as2->language(), Qt::CaseSensitive);
        }
    }
    return resultInt < 0;
}

bool AndroidString::overided() const
{
    return mOverided;
}

void AndroidString::setOverided(bool overided)
{
    mOverided = overided;
}

QString AndroidString::exportCSV()
{
    QString csv;

    foreach (QString str, mTranslation) {
        csv += path() + CsvSeparator;
        csv += androidLabel() + CsvSeparator;
        csv += language() + CsvSeparator;
        switch (type()) {
            case TypeString:
                csv += QString("string") + CsvSeparator;
                break;
            case TypeArray:
                csv += QString("array") + CsvSeparator;
                break;
            case TypeQuantity:
                csv += QString("quantity") + CsvSeparator;
                break;
        }
        csv += csvFormat(str);
        csv += '\n';
    }

    return csv;
}

QString AndroidString::csvFormat(const QString input)
{
    QString output = input;
    //Should we add a '"' at the end of the line?
    if (output.size() > 0) {
        int look = output.indexOf(CsvSeparator);
        if ((output.at(0) != quote) && (look >= 0)) {
            //does not start by '"' but there is a separator inside...
            output.prepend(quote);
        }

        if ((output.at(0) == quote) && (output.at(output.size() - 1) != quote)) {
            output.append(('"'));
        }

        while (look > 0) {
            if (output.at(look-1) != '\\') {
                output.insert(look, '\\');
                look = output.indexOf(CsvSeparator, look + 2);
            } else {
                look = output.indexOf(CsvSeparator, look + 1);
            }
        }
    }

    return output;
}
