#include "processingdialog.h"
#include "ui_processingdialog.h"

ProcessingDialog::ProcessingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessingDialog),
    mStep(0), mAbort(false)
{
    ui->setupUi(this);
    setModal(true);
}

ProcessingDialog::~ProcessingDialog()
{
    delete ui;
}
int ProcessingDialog::step() const
{
    return mStep;
}

void ProcessingDialog::setStep(int step)
{
    mStep = step;

    ui->labelParsing->setEnabled(mStep > 0);
    ui->labelOverlay->setEnabled(mStep > 1);
    ui->labelSorting->setEnabled(mStep > 2);

    if (mStep > 3) {
        close();
    }
}

void ProcessingDialog::on_buttonBox_rejected()
{
    mAbort = true;
}
