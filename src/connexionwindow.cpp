#include "connexionwindow.h"

ConnexionWindow::ConnexionWindow(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent,f)
{
    setupUi(this);
    this->setWindowModality(Qt::WindowModal);
    this->setWindowTitle("Connexion au Serveur");
}

ConnexionWindow::~ConnexionWindow()
{
}
