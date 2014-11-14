#include "androidstring.h"

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
    static const QString separator = ";";
//    static const QChar sep = 10;
    QString csv;
    csv += path() + separator;
    csv += androidLabel() + separator;
    csv += language() + separator;
    switch (type()) {
        case TypeString:
            csv += QString("string") + separator;
            break;
        case TypeArray:
            csv += QString("array") + separator;
            break;
        case TypeQuantity:
            csv += QString("quantity") + separator;
            break;
    }
//    csv += '"' + mTranslation.join(sep) + '"';
    csv += mTranslation.join(separator);

    return csv;
}
