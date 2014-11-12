#include "androidstring.h"

AndroidString::AndroidString(QObject *parent) :
    QObject(parent)
{
}

AndroidString::AndroidString(AndroidString &str, QObject *parent) :
    QObject(parent)
{
    setPath(str.path());
    setAndroidLabel(str.androidLabel());
    setLanguage(str.language());
    setText(str.text());
    setType(str.type());
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

QStringList AndroidString::text() const
{
    return mText;
}

void AndroidString::setText(const QStringList &text)
{
    mText = text;
}

void AndroidString::appendText(const QString &text)
{
    mText.append(text);
}

AndroidString::AndroidStringType AndroidString::type() const
{
    return mType;
}

void AndroidString::setType(const AndroidStringType &type)
{
    mType = type;
}
