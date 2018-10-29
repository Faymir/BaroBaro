#include "privatewindow.h"

PrivateWindow::PrivateWindow(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent,f)
{
    setupUi(this);
    this->setWindowModality(Qt::NonModal);
    this->setWindowTitle("Message Privé");
}

PrivateWindow::~PrivateWindow()
{
}
