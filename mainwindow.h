#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDirIterator>
#include <QThread>

#include "androidstring.h"
#include "androidstringmodel.h"
#include "processingdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool parserRun();

private slots:
    void on_parseButton_clicked();
    void on_sourceButton_clicked();
    void on_excludeButton_clicked();
    void on_overlayButton_clicked();
    void on_exportButton_clicked();

    void handleResults(const bool &aborted);

private:
    Ui::MainWindow *ui;

    void selectDirectory(QLineEdit *line);
    void updateTreeWidget();
    QDirIterator *newDirIterator(QDir &source);
    bool updateList(QList<AndroidString*> *list,
                    QLineEdit *source, QLineEdit *exclude = NULL);
    bool overloadList();

    QList<AndroidString*> mList;
    AndroidStringModel *mModel;
    ProcessingDialog *mProcess;
};

class ParseThread : public QThread
{
    Q_OBJECT

public:
    ParseThread(MainWindow *win);
    void run();

private:
    MainWindow *mMainWindow;

signals:
    void resultReady(const bool &result);
};

#endif // MAINWINDOW_H
