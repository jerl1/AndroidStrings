#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>

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
    qDeleteAll(mList);
}

void MainWindow::updateTreeWidget()
{
    AndroidStringModel *tmpModel = mModel;
    mModel = new AndroidStringModel(mList);
    ui->treeView->setModel(mModel);
    delete tmpModel;

    /* Cosmetic change */
    for (int i = 0; i < mModel->columnCount(); i++)
        ui->treeView->resizeColumnToContents(i);
}

QDirIterator *MainWindow::newDirIterator(QDir &source)
{
    source.setNameFilters(QStringList("*tring*.xml"));
    source.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable);
    source.setSorting(QDir::Name);

    return new QDirIterator(source, QDirIterator::Subdirectories);
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

void MainWindow::updateList(QList<AndroidString*> *list,
                            QLineEdit *source, QLineEdit *exclude)
{
    if ((source == NULL) || (source->text().size() <= 0))
        return;

    QDir sourceDir;
    sourceDir = QDir(source->text());
    sourceDir.makeAbsolute();

    QString excludePath;
    if ((exclude != NULL) && (exclude->text().size() > 0))
        excludePath = QDir(exclude->text()).absolutePath();
    else
        excludePath = ";";

    //Look for all xml files
    QDirIterator *sourcesIterator = newDirIterator(sourceDir);
    while (sourcesIterator->hasNext()) {
        QDir xmlDir = sourcesIterator->next();
        xmlDir.makeAbsolute();

        if (xmlDir.path().startsWith(excludePath))
            continue;

        QFile file(xmlDir.path());
        file.open(QFile::ReadOnly | QFile::Text);
        QString smallPath = QFileInfo(file).absolutePath().remove(sourceDir.path() + "/");

        AndroidStringReader reader(list, smallPath);
        if (reader.readFile(&file) == false) {
            qWarning(qPrintable(QString("Parsing KO: ") + QFileInfo(file).absoluteFilePath()));
        } else {
            //qDebug(qPrintable(QString("Parsing OK: ") + QFileInfo(file).absoluteFilePath()));
        }
    }
    delete sourcesIterator;
}

void MainWindow::overloadList()
{
    //Now retrieved all that have been overloaded
    QList<AndroidString*> overloadedList;
    updateList(&overloadedList, ui->overlayLine);

    foreach (AndroidString *overStr, overloadedList) {
        foreach (AndroidString *sourceStr, mList) {
            if ((overStr->path() == sourceStr->path()) &&
                (overStr->androidLabel() == sourceStr->androidLabel()) &&
                (overStr->language() == sourceStr->language())) {
                overStr->setOverided(true);
                mList.removeAll(sourceStr);
                mList.append(overStr);
                break;
            }
        }
    }
}

void MainWindow::on_parseButton_clicked()
{
    //Create a fake list just for the updating process
    QList<AndroidString*> tmpList;

    updateUI(false);
    AndroidStringModel *tmpModel = mModel;
    mModel = new AndroidStringModel(tmpList);
    ui->treeView->setModel(mModel);
    delete tmpModel;

    //Clear the list of translation
    while (!mList.isEmpty())
        delete mList.takeFirst();

    ParseThread *workerThread = new ParseThread(this);
    connect(workerThread, &ParseThread::resultReady, this, &MainWindow::handleResults);
    connect(workerThread, &ParseThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

void MainWindow::on_exportButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export file"));
    if (filename.size() > 0) {
        QFile file(filename);
        if (file.open(QFile::WriteOnly|QFile::Truncate))
        {
            QTextStream stream(&file);
            stream <<  "Path;Android label;Language;Type;Text\n";
            foreach (AndroidString *str, mList) {
                stream << str->exportCSV() << "\n";
            }
            file.close();
        }
    }
}

void MainWindow::handleResults(const int &result)
{
    if (result > 0) {
        //Sort result
        qSort(mList.begin(), mList.end(), AndroidString::sort);

        updateTreeWidget();
    }
    updateUI(true);
}

int MainWindow::parserRun()
{
    updateList(&mList, ui->sourceLine, ui->excludeLine);
    overloadList();

    return 1;
}

void MainWindow::updateUI(const bool enable)
{
    ui->sourceButton->setEnabled(enable);
    ui->excludeButton->setEnabled(enable);
    ui->overlayButton->setEnabled(enable);
    ui->parseButton->setEnabled(enable);
    ui->exportButton->setEnabled(enable);
}

//------------------------------------------------

ParseThread::ParseThread(MainWindow *win) :
    mMainWindow(win)
{

}

void ParseThread::run()
{
    int result = mMainWindow->parserRun();
    emit resultReady(result);
}
