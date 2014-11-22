#ifndef PROCESSINGDIALOG_H
#define PROCESSINGDIALOG_H

#include <QDialog>

namespace Ui {
class ProcessingDialog;
}

class ProcessingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessingDialog(QWidget *parent = 0);
    ~ProcessingDialog();

    int step() const;
    void setStep(int step);

    bool abort() { return mAbort; }

private slots:
    void on_buttonBox_rejected();

private:
    Ui::ProcessingDialog *ui;

    int mStep;
    bool mAbort;
};

#endif // PROCESSINGDIALOG_H
