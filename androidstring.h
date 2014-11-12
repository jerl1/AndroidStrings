#ifndef ANDROIDSTRING_H
#define ANDROIDSTRING_H

#include <QObject>
#include <QStringList>

class AndroidString : public QObject
{
    Q_OBJECT
public:
    explicit AndroidString(QObject *parent = 0);
    explicit AndroidString(AndroidString &str, QObject *parent = 0);

    typedef enum _AndroidStringType {
        string = 0,
        array,
        quantity
    } AndroidStringType;

    QString path() const;
    void setPath(const QString &path);

    QString androidLabel() const;
    void setAndroidLabel(const QString &androidLabel);

    QString language() const;
    void setLanguage(const QString &language);

    QStringList text() const;
    void setText(const QStringList &text);
    void appendText(const QString &text);

    AndroidStringType type() const;
    void setType(const AndroidStringType &type);

private:
    QString mPath;
    QString mAndroidLabel;
    QString mLanguage;
    QStringList mText;

    AndroidStringType mType;
};

#endif // ANDROIDSTRING_H
