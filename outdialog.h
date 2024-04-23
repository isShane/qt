#ifndef OUTDIALOG_H
#define OUTDIALOG_H

#include <QDialog>
#include "formouttable.h"

namespace Ui {
class outDialog;
}

class outDialog : public QDialog
{
    Q_OBJECT

public:
    explicit outDialog(QWidget *parent = nullptr);
    ~outDialog();
    typedef struct {
        int timer;
        int countLog;
    }outArg;

    outArg getParam() const;
private:
    Ui::outDialog *ui;
    FormOutTable *mOutTable = new FormOutTable;
    outArg oArg;

signals:
    void IsOk(int time, int count);
};

#endif // OUTDIALOG_H
