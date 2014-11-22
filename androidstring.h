#ifndef ANDROIDSTRING_H
#define ANDROIDSTRING_H

#include <QObject>
#include <QStringList>

class AndroidString : public QObject
{
    Q_OBJECT
public:
    explicit AndroidString(QObject *parent = 0);
    explicit AndroidString(const AndroidString *str, QObject *parent = 0);

    static bool sort(const AndroidString *as1, const AndroidString *as2);

    static const QChar CsvSeparator;
    static const QChar quote;

    typedef enum _AndroidStringType {
        TypeString = 0,
        TypeArray,
        TypeQuantity
    } AndroidStringType;

    typedef enum _AndroidStringStatus {
        TypeNew = 0,
        TypeOverided,
        TypeOverlayNew
    } AndroidStringStatus;

    QString path() const;
    void setPath(const QString &path);

    QString androidLabel() const;
    void setAndroidLabel(const QString &androidLabel);

    QString language() const;
    void setLanguage(const QString &language);

    QStringList translation() const;
    void setTranslation(const QStringList &translation);
    void appendTranslation(const QString &translation);
    QString formatedTranslation() const;

    AndroidStringType type() const;
    void setType(const AndroidStringType &type);

    QString exportCSV();

    AndroidStringStatus status() const;
    void setStatus(const AndroidStringStatus &status);

private:
    QString mPath;
    QString mAndroidLabel;
    QString mLanguage;
    QStringList mTranslation;

    AndroidStringType mType;
    AndroidStringStatus mStatus;

    QString csvFormat(QString input);
};

#endif // ANDROIDSTRING_H
