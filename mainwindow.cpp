#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>

#include "androidstringreader.h"
#include "androidstringmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mModel = new AndroidStringModel(mList);
    ui->treeView->setModel(mModel);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mModel;

    while (!mList.isEmpty())
        delete mList.takeFirst();
}

void MainWindow::updateTreeWidget()
{
    delete mModel;
    mModel = new AndroidStringModel(mList);
    ui->treeView->setModel(mModel);
}

void MainWindow::selectDirectory(QLineEdit *line)
{
    if (line == NULL)
        return;

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.selectFile(line->text());

    if (dialog.exec()) {
        QStringList files;
        files = dialog.selectedFiles();
        line->setText(files[0]);
    }
}

void MainWindow::on_frameworkButton_clicked()
{
    selectDirectory(ui->frameworkLine);
}

void MainWindow::on_deviceButton_clicked()
{
    selectDirectory(ui->deviceLine);
}

void MainWindow::on_parseButton_clicked()
{
    while (!mList.isEmpty())
        delete mList.takeFirst();

    //Look for all xml files
    QDir directory(ui->frameworkLine->text());
    directory.setNameFilters(QStringList("*tring*.xml"));
    directory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable);

    QDirIterator xmlIterator(directory, QDirIterator::Subdirectories);
    while (xmlIterator.hasNext()) {
        QDir xmlDir = xmlIterator.next();
        QFile file(xmlDir.path());
        file.open(QFile::ReadOnly | QFile::Text);

        AndroidStringReader reader(&mList, QFileInfo(file).absolutePath());
        if (reader.readFile(&file) == false) {
            qWarning(qPrintable(QString("Parsing KO: ") + xmlDir.path()));
        } else {
            /*qDebug(qPrintable(QString("Parsing OK: ") + xmlDir.path()));*/
        }
    }

    updateTreeWidget();
}
