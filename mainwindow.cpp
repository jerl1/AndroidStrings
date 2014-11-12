#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>

#include "androidstringreader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    ui->treeWidget->clear();
    delete ui;

    while (!mList.isEmpty())
        delete mList.takeFirst();
}

void MainWindow::updateTreeWidget()
{
    //Clear the treeWidget
    ui->treeWidget->clear();

    foreach (AndroidString *android, mList) {
        AndroidStringItem *item = new AndroidStringItem();
        item->set(*android);
        ui->treeWidget->addTopLevelItem(item);
    }

    for (int i = 0; i < ui->treeWidget->columnCount(); i++)
        ui->treeWidget->resizeColumnToContents(i);
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
    mList.clear();

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

void AndroidStringItem::set(const QString path, const QString android,
                            const QString language, const QString text,
                            const AndroidString::AndroidStringType /*type*/)
{
    setData(AndroidStringItem::path, Qt::DisplayRole, path);
    setData(AndroidStringItem::android, Qt::DisplayRole, android);
    setData(AndroidStringItem::language, Qt::DisplayRole, language);
    setData(AndroidStringItem::text, Qt::DisplayRole, text);
}

void AndroidStringItem::set(const AndroidString &str)
{
    set(str.path(), str.androidLabel(), str.language(), str.text()[0], str.type());
}
