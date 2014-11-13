#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
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

    /* Cosmetic change */
    for (int i = 0; i < mModel->columnCount(); i++)
        ui->treeView->resizeColumnToContents(i);
}

QDirIterator *MainWindow::newDirIterator(QLineEdit *line)
{
    QDir sourcesDirectory(line->text());
    sourcesDirectory.setNameFilters(QStringList("*tring*.xml"));
    sourcesDirectory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable);
    sourcesDirectory.setSorting(QDir::Name);

    return new QDirIterator(sourcesDirectory, QDirIterator::Subdirectories);
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

void MainWindow::on_sourceButton_clicked()
{
    selectDirectory(ui->sourceLine);
}

void MainWindow::on_excludeButton_clicked()
{
    selectDirectory(ui->excludeLine);
}

void MainWindow::on_overlayButton_clicked()
{
    selectDirectory(ui->overlayLine);
}

void MainWindow::on_parseButton_clicked()
{
    while (!mList.isEmpty())
        delete mList.takeFirst();

    QString excludePath = ui->excludeLine->text();
    if (excludePath.size() == 0)
        excludePath = ";"; //Somethings not empty, else everythings will be excluded

    //Look for all xml files
    QDirIterator *sourcesIterator = newDirIterator(ui->sourceLine);
    while (sourcesIterator->hasNext()) {
        QDir xmlDir = sourcesIterator->next();

        if (xmlDir.path().startsWith(excludePath))
            continue;

        QFile file(xmlDir.path());
        file.open(QFile::ReadOnly | QFile::Text);
        QString smallPath = QFileInfo(file).absolutePath();
        smallPath.remove(0, ui->sourceLine->text().size() + 1);

        AndroidStringReader reader(&mList, smallPath);
        if (reader.readFile(&file) == false) {
            qWarning(qPrintable(QString("Parsing KO: ") + xmlDir.path()));
        } else {
            /*qDebug(qPrintable(QString("Parsing OK: ") + xmlDir.path()));*/
        }
    }
    delete sourcesIterator;

    //Sort result
    qSort(mList.begin(), mList.end(), AndroidString::sort);

    updateTreeWidget();
}
