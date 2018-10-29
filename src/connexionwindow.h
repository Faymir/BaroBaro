#ifndef CONNEXIONWINDOW_H
#define CONNEXIONWINDOW_H

#include <QDialog>
#include "ui_connexionwindow.h"

namespace Ui {
class ConnexionWindow;
}

class ConnexionWindow : public QDialog,public Ui::ConnexionWindow
{
    Q_OBJECT

public:
    explicit ConnexionWindow(QWidget *parent = 0 , Qt::WindowFlags f = Qt::WindowFlags() );
    ~ConnexionWindow();

private:
};

#endif // CONNEXIONWINDOW_H
