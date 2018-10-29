#ifndef PRIVATEWINDOW_H
#define PRIVATEWINDOW_H

#include <QDialog>
#include "ui_privatewindow.h"

namespace Ui {
class PrivateWindow;
}

class PrivateWindow : public QDialog, public Ui::PrivateWindow
{
    Q_OBJECT

public:
    explicit PrivateWindow(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags() );
    ~PrivateWindow();

private:
};

#endif // PRIVATEWINDOW_H
