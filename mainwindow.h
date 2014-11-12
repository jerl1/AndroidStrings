#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTreeWidgetItem>
#include "androidstring.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_frameworkButton_clicked();

    void on_deviceButton_clicked();

    void on_parseButton_clicked();

private:
    Ui::MainWindow *ui;

    void selectDirectory(QLineEdit *line);
    void updateTreeWidget();

    QList<AndroidString*> mList;
};

class AndroidStringItem: public QTreeWidgetItem
{
private:
    typedef enum _column
    {
        path = 0,
        android,
        language,
        text,
        type
    } column;

    void set(const QString path, const QString android,
             const QString language, const QString text, const AndroidString::AndroidStringType type);

public:
    void set(const AndroidString &str);
};

#endif // MAINWINDOW_H
