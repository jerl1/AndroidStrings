#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDirIterator>

#include "androidstring.h"
#include "androidstringmodel.h"

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
    void on_parseButton_clicked();
    void on_sourceButton_clicked();
    void on_excludeButton_clicked();
    void on_overlayButton_clicked();

    void on_exportButton_clicked();

private:
    Ui::MainWindow *ui;

    void selectDirectory(QLineEdit *line);
    void updateTreeWidget();
    QDirIterator *newDirIterator(QDir &source);
    void updateList(QList<AndroidString*> *list,
                    QLineEdit *source, QLineEdit *exclude = NULL);
    void overloadList();

    QList<AndroidString*> mList;
    AndroidStringModel *mModel;
};

#endif // MAINWINDOW_H
