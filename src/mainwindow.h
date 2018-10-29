#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtNetwork>
#include <QFile>
#include <QDir>
#include "ui_mainwindow.h"
#include "connexionwindow.h"
#include "client.h"
#include <QtDebug>
#include <QSound>
#include <privatewindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, private Ui::MainWindow, private Client
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void envoyerATous(const QByteArray &fileContend, const QString &fileName);
    void envoyerATous(const QString &msg);
    void setMsmAreaState(bool state);
    void enregistrerFichier(const QByteArray &fileContend, const QString &fileName);
    void afficherFichier(const QString& fileName,int type = -1);
    bool traiterMessagePublic(QDataStream& in);
    bool traiterMessagePrive(QDataStream& in);
    bool traiterInfo(QDataStream& in);
    bool traiterPseudo(QDataStream& in);
    bool traiterAvatar(QDataStream& in);
    bool traiterFichier(QDataStream& in);
    bool mySearch(const QString& str);
    void envoyer(const QString &msg,const quint8 type,const QString& destinataire = "" );
    void envoyer(const QPixmap &pixmap);

public slots:
    void connecte(void);
    void receptionDonnees(void);
    void deconnecte(void);
    void erreurSocket(QAbstractSocket::SocketError erreur);

    void on_m_retour_clicked(void);
    void on_m_connexion_clicked(void);
    void on_m_deconnexion_clicked(void);

    void on_m_messagePrivePB_clicked(void);
    void onFenPriveEnvoyerClicked(void);
    void onFenPriveAnnulerClicked(void);

    void onHideAction(bool checked);
    void onTabBarClicked();

    void on_m_envoyer_clicked(void);
    void on_m_fichierLE_returnPressed(void);
    void on_m_ouvrir_clicked(void);
    void on_m_envoyerPB_clicked(void);
    void on_m_messageLE_returnPressed(void);
    void on_m_pseudoLE_textChanged(QString str);
    void on_m_avatarPB_clicked(void);
    void onM_stopAll_clicked(void);

    void refreshProgressBar(qint64 value);
    void insererSmiley(void);



private:
    ConnexionWindow *fen;
    PrivateWindow *fenPrive;

    quint64 m_tailleFichier;
    QString m_nomFichier;
    quint16 m_nomFichierLen;
    quint8 m_type;
    quint64 m_tailleFichierEnvoye;
    quint64 byteWritten;

};

#endif // MAINWINDOW_H
