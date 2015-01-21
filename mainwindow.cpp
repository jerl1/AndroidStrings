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

bool MainWindow::updateList(QList<AndroidString*> *list,
                            QLineEdit *source, QLineEdit *exclude)
{
    bool aborted = false;

    if ((source == NULL) || (source->text().size() <= 0))
        return aborted;

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

        if (mProcess->abort()) {
            aborted = true;
            break;
        }
    }

    delete sourcesIterator;

    return aborted;
}

bool MainWindow::overloadList()
{
    bool aborted = false;

    //Now retrieved all that have been overloaded
    QList<AndroidString*> overloadedList;
    updateList(&overloadedList, ui->overlayLine);
    int nb_overided = 0, nb_overlay = 0;

    foreach (AndroidString *overStr, overloadedList) {
        foreach (AndroidString *sourceStr, mList) {
            if (AndroidString::compare(overStr, sourceStr) == 0) {
                overStr->setStatus(AndroidString::TypeOverided);
                nb_overided += mList.removeAll(sourceStr);
                mList.append(overStr);
                overloadedList.removeOne(overStr);
                break;
            }

            if (mProcess->abort()) {
                aborted = true;
                break;
            }
        }
    }
    qDebug(qPrintable(QString("Number of translation overided: %1").arg(nb_overided)));

    foreach (AndroidString *overStr, overloadedList) {
        if (mProcess->abort()) {
            aborted = true;
            break;
        }
        overStr->setStatus(AndroidString::TypeOverlayNew);
        nb_overlay += 1;
        mList.append(overStr);
        overloadedList.removeOne(overStr);
    }
    qDebug(qPrintable(QString("Number of translation overlay added: %1").arg(nb_overlay)));

    //In case of an abort there is some datas allocated
    qDeleteAll(overloadedList);

    return aborted;
}

void MainWindow::on_parseButton_clicked()
{
    //Create a fake list just for the updating process
    QList<AndroidString*> tmpList;

    mProcess = new ProcessingDialog(this);
    mProcess->show();

    AndroidStringModel *tmpModel = mModel;
    mModel = new AndroidStringModel(tmpList);
    ui->treeView->setModel(mModel);
    delete tmpModel;

    //Clear the list of translation
    while (!mList.isEmpty())
        delete mList.takeFirst();

    ParseThread *workerThread = new ParseThread(this);
    connect(workerThread, SIGNAL(ParseThread::resultReady(const int&)),
            this, SLOT(MainWindow::handleResults(const bool&)));
    connect(workerThread, SIGNAL(ParseThread::finished),
            workerThread, SLOT(QObject::deleteLater));
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
            stream << "Path" << AndroidString::CsvSeparator;
            stream << "Android label" << AndroidString::CsvSeparator;
            stream << "Language" << AndroidString::CsvSeparator;
            stream << "Type" << AndroidString::CsvSeparator;
            stream << "Text" << "\n";
            foreach (AndroidString *str, mList) {
                stream << str->exportCSV();
            }
            file.close();
        }
    }
}

void MainWindow::handleResults(const bool &aborted)
{
    if (aborted == false) {
        //Sort result
        std::sort(mList.begin(), mList.end(), AndroidString::sort);
        mProcess->setStep(3);

        updateTreeWidget();
        mProcess->setStep(4);
    } else {
        //Clear the list of translation
        while (!mList.isEmpty())
            delete mList.takeFirst();
    }

    qDebug(qPrintable(QString("Number of translation: %1").arg(mList.size())));

    delete mProcess;
}

bool MainWindow::parserRun()
{
    bool aborted = updateList(&mList, ui->sourceLine, ui->excludeLine);
    mProcess->setStep(1);

    if (aborted == false) {
        overloadList();
        mProcess->setStep(2);
    }

    return aborted;
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
