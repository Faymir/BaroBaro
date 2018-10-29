#include "mainwindow.h"

const qint64 max= 114621090;
const qint64 max2= 80000000;

//regler la confimration d'envoie de fichier pour qu'elle soit envoyee uniquement a celui qui a envoyer le fichier
//pseudo unique
//icon pour type
//drag and drop
//envoyer fichier en prive
//smiley en prive
//avatar
//section du message vien de se connecter pour actualiser la liste

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),Client(new QTcpSocket(this),"",new QPixmap(":img/defAvatar"))
{
    setupUi(this);
    this->setWindowTitle("Baro");
    this->setWindowIcon(QIcon(":img/logo"));
    QDir current;
    current.mkdir(tr("Fichiers_recus"));
    current.setPath(tr("./Fichiers_recus"));
    current.mkdir(tr("Images"));
    current.mkdir(tr("Videos"));
    current.mkdir(tr("Audios"));
    current.mkdir(tr("docs"));
    QDir::setSearchPaths("img", QStringList(current.currentPath() + tr("/Images")));
    QDir::setSearchPaths("mix", QStringList(current.currentPath() + tr("/Audios")));
    QDir::setSearchPaths("vid", QStringList(current.currentPath() + tr("/Videos")));
    QDir::setSearchPaths("docs", QStringList(current.currentPath() + tr("/docs")));

    //centrer les conversations
    QTextCursor cursor = m_page->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignCenter);
    cursor.mergeBlockFormat(textBlockFormat);
    m_page->setTextCursor(cursor);

    m_tailleFichier = {0};
    m_nomFichier = {""};
    m_nomFichierLen = {0};
    m_type = {0};
    m_tailleFichierEnvoye = {0};
    byteWritten = {0};

    m_page->setReadOnly(true);
    fen = new ConnexionWindow(this);
    fenPrive = new PrivateWindow(this);
    QToolButton* hideButton = new QToolButton();
    hideButton->setDefaultAction(m_hide);
    hideButton->setAutoRaise(true);
    labelAvatar->setPixmap(*m_avatar);

    tabWidget->setCornerWidget(hideButton);
    tabWidget->setMaximumHeight(tabWidget->tabBar()->height());
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);


    setMsmAreaState(false);
    connect(m_socket,SIGNAL(readyRead()),this,SLOT(receptionDonnees()));
    connect(m_socket,SIGNAL(connected()),this,SLOT(connecte()));
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(deconnecte()));
    connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(erreurSocket(QAbstractSocket::SocketError)));

    connect(m_hide, SIGNAL(toggled(bool)), this, SLOT(onHideAction(bool)));
    connect(tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(onTabBarClicked()));


    connect(fen->m_connexion,SIGNAL(clicked(bool)),this,SLOT(on_m_connexion_clicked()));
    connect(fen->m_deconnexion,SIGNAL(clicked(bool)),this,SLOT(on_m_deconnexion_clicked()));
    connect(fen->m_ipLE,SIGNAL(returnPressed()),this,SLOT(on_m_connexion_clicked()));
    connect(fen->m_portSB,SIGNAL(editingFinished()),this,SLOT(on_m_connexion_clicked()));
    connect(fen->m_stopAll,SIGNAL(clicked(bool)),this,SLOT(onM_stopAll_clicked()));
    connect(m_pseudoLE,SIGNAL(returnPressed()),this,SLOT(on_m_connexion_clicked()));

    connect(fenPrive->m_envoyer,SIGNAL(clicked(bool)),this,SLOT(onFenPriveEnvoyerClicked()));
    connect(fenPrive->m_annuler,SIGNAL(clicked(bool)),this,SLOT(onFenPriveAnnulerClicked()));
    connect(fenPrive->m_message,SIGNAL(returnPressed()),this,SLOT(onFenPriveEnvoyerClicked()));


    connect(m_socket,SIGNAL(bytesWritten(qint64)),this,SLOT(refreshProgressBar(qint64)));

    connect(pushButton,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_2,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_3,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_4,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_5,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_6,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_7,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_8,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_9,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_10,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_11,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_12,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_13,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_14,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));
    connect(pushButton_15,SIGNAL(clicked(bool)),this,SLOT(insererSmiley()));

    fen->show();
    //qDebug() << pushButton->objectName();
    //m_page->append("<img src=\":smiley/s0\">");

}

//fonctions
void MainWindow::envoyerATous(const QByteArray &fileContend, const QString &fileName)
{
    QByteArray paquet;
    QDataStream out(&paquet,QIODevice::WriteOnly);

    out << static_cast<quint64>(fileContend.size());
    out << static_cast<quint8>(FICHIER);
    out << static_cast<quint16>(fileName.size());
    out << fileName;

    int nbrBlocks=fileContend.size() / max2;
    m_tailleFichierEnvoye = fileContend.size();

    if(fileContend.size()>max2)
    {
       // qDebug() <<"taille fichier: " <<fileContend.size();
        //qDebug() << "nombre blocks: " << nbrBlocks;
        //qDebug() << "1er blocks: " << fileContend.left(max2).size();
        out << fileContend.left(max2);
    }
    else
        out << fileContend;

   /* out.device()->seek(0);

    out << static_cast<quint64>(paquet.size() - (sizeof(quint64) + sizeof(quint8) + sizeof(quint16)) - len);

    out << static_cast<quint8>(FICHIER);
    out << static_cast<quint16>(len);*/

    //qDebug() << "debut de l'envoie du contenu";



    if(fileContend.size()>max2)
    {
        m_socket->write(paquet);

        for(int i=0;i<nbrBlocks-1;i++)
        {
            m_socket->write(fileContend.mid(max2*(i+1),max2));
            //qDebug() << i+2<<"e blocks: " << fileContend.mid(max2*(i+1),max2).size();
            paquet.clear();
        }
        if(fileContend.size() - max2*nbrBlocks >0)
        {
            m_socket->write(fileContend.mid(max2*nbrBlocks));
            //qDebug() << "Dernier blocks: " << fileContend.mid(max2*nbrBlocks).size();
        }
    }
    else
        m_socket->write(paquet);

    //qDebug() << "Fin de l'envoie du contenu";

    m_fichierLE->clear();
    m_fichierLE->setFocus();
}
void MainWindow::envoyerATous(const QString &msg)
{
    QByteArray paquet;
    QDataStream out(&paquet,QIODevice::WriteOnly);

    out << static_cast<quint64>(0);
    out << static_cast<quint8>(0);
    out << static_cast<quint16>(0);
    out << m_pseudo;

    int len = paquet.size() - static_cast<int>(sizeof(quint64) + sizeof(quint8) + sizeof(quint16));

    out << msg;

    out.device()->seek(0);

    out << static_cast<quint64>(paquet.size() - (sizeof(quint64) + sizeof(quint8) + sizeof(quint16)) - len);
    out << static_cast<quint8>(MSGPUBLIC);
    out << static_cast<quint16>(len);
    m_tailleFichierEnvoye = paquet.size();

    m_socket->write(paquet);

    m_messageLE->clear();
    m_messageLE->setFocus();
}

void MainWindow::envoyer(const QString &msg, const quint8 type, const QString &destinataire)
{
    QByteArray paquet;
    QDataStream out(&paquet,QIODevice::WriteOnly);

    out << static_cast<quint64>(0);
    out << static_cast<quint8>(0);
    int len = {0};
    int lenDest={0};

    out << static_cast<quint16>(0);
    out << m_pseudo;
    len = paquet.size() - static_cast<int>(sizeof(quint64) + sizeof(quint8) + sizeof(quint16));


    if(type==MSGPRIVE)
    {
        out<<static_cast<quint16>(destinataire.size());
        out<<destinataire;
        lenDest = paquet.size() - static_cast<int>(sizeof(quint64) + sizeof(quint8) + 2*sizeof(quint16)) -len;
    }

    if(type!=USERNAME)
    {
        out << msg;
    }

    out.device()->seek(0);

    if(type!=USERNAME)
    {
        if(type==MSGPRIVE)
            out << static_cast<quint64>(paquet.size() - (sizeof(quint64) + sizeof(quint8) + 2*sizeof(quint16)) - len - lenDest);
        else
            out << static_cast<quint64>(paquet.size() - (sizeof(quint64) + sizeof(quint8) + sizeof(quint16)) - len);
    }
    else
        out << static_cast<quint64>(0);

    out << static_cast<quint8>(type);
    out << static_cast<quint16>(len);

    m_tailleFichierEnvoye = paquet.size();
    m_socket->write(paquet);

    if(type==MSGPRIVE)
    {
        fenPrive->m_message->clear();
        fenPrive->m_message->setFocus();
    }
}

void MainWindow::envoyer(const QPixmap &pixmap)
{
    QByteArray paquet;
    QDataStream out(&paquet,QIODevice::WriteOnly);

    out << static_cast<quint64>(0);
    out << static_cast<quint8 0);
    out << static_cast<quint16>(0);
    out << m_pseudo;

    qDebug() <<"pixmap size (fonction envoyer) : "<< pixmap.toImage().byteCount();

    int len = paquet.size() - static_cast<int>(sizeof(quint64) + sizeof(quint8) + sizeof(quint16));

    out << pixmap;

    out.device()->seek(0);

    out << static_cast<quint64>(paquet.size() - (sizeof(quint64) + sizeof(quint8) + sizeof(quint16)) - len);
    out << static_cast<quint8>(AVATAR);
    out << static_cast<quint16>(len);

    m_tailleFichierEnvoye = paquet.size();

    qDebug() << "debut de l'envoie du pixmap";

    m_socket->write(paquet);

    qDebug() << "fin de l'envoie du pixmap";

}

void MainWindow::setMsmAreaState(bool state)
{
    m_page->setEnabled(state);
    fichierLabel->setEnabled(state);
    m_fichierLE->setEnabled(state);
    m_ouvrir->setEnabled(state);
    m_envoyer->setEnabled(state);
    m_envoyerPB->setEnabled(state);
    m_messageLE->setEnabled(state);
    labelMessage->setEnabled(state);
    m_pseudoLE->setEnabled(!state);
}



void MainWindow::enregistrerFichier(const QByteArray &fileContend, const QString &fileName)
{

    //QFile file;
    if(fileName.contains(".mp4",Qt::CaseInsensitive) ||
       fileName.contains(".avi",Qt::CaseInsensitive) ||
       fileName.contains(".flv",Qt::CaseInsensitive) ||
       fileName.contains(".mpg",Qt::CaseInsensitive) ||
       fileName.contains(".mkv",Qt::CaseInsensitive) ||
       fileName.contains(".3gp",Qt::CaseInsensitive) ||
       fileName.contains(".wmv",Qt::CaseInsensitive) ||
       fileName.contains(".mov",Qt::CaseInsensitive) ||
       fileName.contains(".vob",Qt::CaseInsensitive))
    {
        QString str={tr("./Fichiers_recus/Videos/")+fileName};
        QFile file(str);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,"",tr("Erreur lors de l'enregistrement du fichier: ")  + file.errorString());
            return;
        }
        file.write(fileContend);

        file.close();
    }
    else if(fileName.contains(".jpg",Qt::CaseInsensitive) ||
            fileName.contains(".png",Qt::CaseInsensitive) ||
            fileName.contains(".jpeg",Qt::CaseInsensitive) ||
            fileName.contains(".bmp",Qt::CaseInsensitive) ||
            fileName.contains(".gif",Qt::CaseInsensitive) ||
            fileName.contains(".ico",Qt::CaseInsensitive) ||
            fileName.contains(".tif",Qt::CaseInsensitive) ||
            fileName.contains(".pcx",Qt::CaseInsensitive) ||
            fileName.contains(".tga",Qt::CaseInsensitive))
    {
        QString str={tr("./Fichiers_recus/Images/")+fileName};
        QFile file(str);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,"",tr("Erreur lors de l'enregistrement du fichier: ")  + file.errorString());
            return;
        }
        file.write(fileContend);

        file.close();
    }
    else if(fileName.contains(".mp3",Qt::CaseInsensitive) ||
            fileName.contains(".wma",Qt::CaseInsensitive) ||
            fileName.contains(".ape",Qt::CaseInsensitive) ||
            fileName.contains(".flac",Qt::CaseInsensitive) ||
            fileName.contains(".aac",Qt::CaseInsensitive) ||
            fileName.contains(".mmf",Qt::CaseInsensitive) ||
            fileName.contains(".amr",Qt::CaseInsensitive) ||
            fileName.contains(".m4a",Qt::CaseInsensitive) ||
            fileName.contains(".m4r",Qt::CaseInsensitive) ||
            fileName.contains(".ogg",Qt::CaseInsensitive) ||
            fileName.contains(".wav",Qt::CaseInsensitive) ||
            fileName.contains(".fla",Qt::CaseInsensitive) ||
            fileName.contains(".mp2",Qt::CaseInsensitive))
    {
        QString str={tr("./Fichiers_recus/Audios/")+fileName};
        QFile file(str);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,"",tr("Erreur lors de l'enregistrement du fichier: ")  + file.errorString());
            return;
        }
        file.write(fileContend);

        file.close();
    }
    else if(fileName.contains(".pdf",Qt::CaseInsensitive) ||
            fileName.contains(".doc",Qt::CaseInsensitive) ||
            fileName.contains(".docx",Qt::CaseInsensitive) ||
            fileName.contains(".xls",Qt::CaseInsensitive) ||
            fileName.contains(".txt",Qt::CaseInsensitive) ||
            fileName.contains(".c",Qt::CaseInsensitive) ||
            fileName.contains(".cpp",Qt::CaseInsensitive) ||
            fileName.contains(".java",Qt::CaseInsensitive) ||
            fileName.contains(".h",Qt::CaseInsensitive))
    {
        QString str={tr("./Fichiers_recus/docs/")+fileName};
        QFile file(str);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,"",tr("Erreur lors de l'enregistrement du fichier: ")  + file.errorString());
            return;
        }
        file.write(fileContend);

        file.close();
    }
    else
    {
        QFile file("./Fichiers_recus/"+fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,"",tr("Erreur lors de l'enregistrement du fichier: ")  + file.errorString());
            return;
        }
        file.write(fileContend);

        file.close();
    }


}

void MainWindow::afficherFichier(const QString &fileName, int type)
{

    if(fileName.contains(".ogg"))
        m_page->append("<video  src=\"" + fileName + "\" controls> Video</video>");
    else if(fileName.contains(".jpg",Qt::CaseInsensitive) ||
            fileName.contains(".png",Qt::CaseInsensitive) ||
            fileName.contains(".jpeg",Qt::CaseInsensitive) ||
            fileName.contains(".bmp",Qt::CaseInsensitive) ||
            fileName.contains(".gif",Qt::CaseInsensitive) ||
            fileName.contains(".ico",Qt::CaseInsensitive) ||
            fileName.contains(".tif",Qt::CaseInsensitive) ||
            fileName.contains(".pcx",Qt::CaseInsensitive) ||
            fileName.contains(".tga",Qt::CaseInsensitive))
    {
        QPixmap img;
        QString str("./Fichiers_recus/Images/");;
        if(type!=-1)
            str="";
        bool fileloaded=img.load(str+fileName);
        if(!fileloaded)
            qDebug() << "Erreur lors du chargement de l'image" << fileName;

        if(img.width()>=200 || img.height()>=200)
            m_page->append(tr("<img src=\"") + str + fileName + "\" widht=\"200\"  height=\"200\"\\>");
        else
            m_page->append(tr("<img src=\"") + str + fileName + "\" \\ >");
    }
    else
        m_page->append("<img src=\"" + fileName + "\" \\>");

    if(type==-1)
        m_page->append(fileName);
}

bool MainWindow::traiterMessagePublic(QDataStream& in)
{
    if(!traiterPseudo(in))
        return false;

    if(m_socket->bytesAvailable() < (qint64)m_tailleFichier)
        return false;

    QString msg={""};

    in >> msg;

    QString str={""};

    str+="<strong>";

    if(m_nomFichier==m_pseudo)
        str+="Vous";
    else
        str+=m_nomFichier;

    str+="</strong>: ";
    str+=msg + "<br>";
    m_page->append(str);
    if(m_nomFichier==m_pseudo)
    {

    }
    return true;
}

bool MainWindow::traiterMessagePrive(QDataStream& in)
{
    if(!traiterPseudo(in))
        return false;

    if(m_socket->bytesAvailable() < (qint64)m_tailleFichier)
        return false;

    QString msg={""};

    in >> msg;

    QString str={""};
    str+="<Font color=blue>";

    str+="<strong>";

    if(m_nomFichier==m_pseudo)
        str+="Vous";
    else
        str+=m_nomFichier;

    str+="</strong>: ";
    str+=msg;
    str+="</Font><br>";
    m_page->append(str);

    if(m_nomFichier==m_pseudo)
    {
        QTextCursor cursor = m_page->textCursor();
        QTextBlockFormat textBlockFormat = cursor.blockFormat();
        textBlockFormat.setAlignment(Qt::AlignCenter);
        cursor.mergeBlockFormat(textBlockFormat);
        m_page->setTextCursor(cursor);
    }
    else
    {
        QTextCursor cursor = m_page->textCursor();
        QTextBlockFormat textBlockFormat = cursor.blockFormat();
        textBlockFormat.setAlignment(Qt::AlignLeft);
        cursor.mergeBlockFormat(textBlockFormat);
        m_page->setTextCursor(cursor);
    }

    return true;

}

bool MainWindow::traiterInfo(QDataStream& in)
{
    if(m_socket->bytesAvailable() < (qint64)m_tailleFichier)
        return false;

    QString msg={""};

    in >> msg;
    QString str=msg;
    msg="<Font color=red><em>"+msg+"</em></Font>";
    m_page->append(msg);

    if(str.contains(tr("viens de se connecter")))
        listWidget->addItem(str.section(' ',0,0));
    else if(str.contains(tr("viens de se deconnecter")))
    {
        str = str.section(' ',0,0);
        for(int i=0; i<listWidget->count(); i++)
        {
            if(listWidget->item(i)->text().compare(str,Qt::CaseInsensitive) == 0)
            {
                listWidget->takeItem(i);
                break;
            }
        }
    }

    return true;
}

bool MainWindow::traiterPseudo(QDataStream& in)
{
    if(m_nomFichierLen == 0)
    {
        if(m_socket->bytesAvailable() < static_cast<int>(sizeof(quint16)))
            return false;

        in >> m_nomFichierLen;
    }

    if(m_nomFichier.isEmpty())
    {
        if(m_socket->bytesAvailable() < m_nomFichierLen)
            return false;

        in >> m_nomFichier;
    }
    qDebug() << "Pseudo est: " <<m_nomFichier;

    if(!mySearch(m_nomFichier))
        listWidget->addItem(m_nomFichier);

    return true;
}

bool MainWindow::traiterAvatar(QDataStream& in)
{
    if(!traiterPseudo(in))
        return false;

    if(m_socket->bytesAvailable() < (qint64)m_tailleFichier)
        return false;

    QPixmap pixmap;

    in >> pixmap;

    //rechercher user
    for(int i=0;i<listWidget->count();i++)
    {
        if(listWidget->item(i)->text().compare(m_nomFichier) == 0)
        {
            listWidget->item(i)->setIcon(QIcon(pixmap));
            break;
        }
    }

    //appliquer l'avatar comme icone de l'element de la listwidget

    return true;

}

bool MainWindow::traiterFichier(QDataStream& in)
{
    if(m_nomFichierLen == 0)
    {
        if(m_socket->bytesAvailable() < static_cast<int>(sizeof(quint16)))
            return false;

        in >> m_nomFichierLen;
    }

    if(m_nomFichier.isEmpty())
    {
        if(m_socket->bytesAvailable() < m_nomFichierLen)
            return false;

        in >> m_nomFichier;
    }


    quint64 bytes = m_socket->bytesAvailable();
    if(bytes < m_tailleFichier)
    {
        m_chargement->setValue(static_cast<int>((double)bytes/(double)m_tailleFichier * 100));
        return false;
    }

    QByteArray file;
    file = m_socket->readAll().right(m_tailleFichier);
    //in >> file;
    qDebug() <<"Taille file= " <<file.size() << "\t taille m_taille = " <<m_tailleFichier;

    enregistrerFichier(file,m_nomFichier);
    afficherFichier(m_nomFichier);

    m_chargement->setValue(0);
    envoyer("Fichier recu!",INFO);

    return true;
}

bool MainWindow::mySearch(const QString& str)
{
    bool found = false;
    for(int i=0; i<listWidget->count(); i++)
    {
        if(listWidget->item(i)->text().compare(str) == 0)
        {
            found = true;
            break;
        }
    }

    return found;
}

//slots

void MainWindow::receptionDonnees()
{
    QDataStream in(m_socket);

    if(m_tailleFichier == 0)
    {
        if(m_socket->bytesAvailable() < static_cast<qint64>(sizeof(quint64)))
            return;

        in >> m_tailleFichier;
    }

    if(m_type == 0)
    {
        if(m_socket->bytesAvailable() < static_cast<int>(sizeof(quint8)))
            return;

        in >> m_type;
    }

    switch (m_type)
    {
        case NONDEFINI:
            qDebug() << "non definit a été recu par le client n= " << m_tailleFichier;
        break;

        case INFO:
        if(!traiterInfo(in))
        {
            qDebug() << "traitement de Info pas encore ok";
            return;
        }
        else
            qDebug() << "traitement de INFO ok";
        break;

        case MSGPUBLIC:
        if(!traiterMessagePublic(in))
        {
            qDebug() << "traitement de MSGPUBLIC pas encore ok";
            return;
        }
        else
            qDebug() << "traitement de MSGPUBLIC ok";
        break;

        case MSGPRIVE:
            if(!traiterMessagePrive(in))
            {
                qDebug() << "traitement de MSGPRIVE pas encore ok";
                return;
            }
            else
                qDebug() << "traitement de MSGPRIVE ok";
            break;

        case AVATAR:
            if(!traiterAvatar(in))
                return;
        break;

        case USERNAME:
            if(!traiterPseudo(in))
                return;
        break;

    case FICHIER:
        if(!traiterFichier(in))
        {
            qDebug() << "traitement de FICHIER pas encore ok";
            return;
        }
        else
            qDebug() << "traitement de FICHIER ok";
        break;

        default:
            qDebug() << "Cas Indéfini dans la fonction de reception de données: peut etre un probleme de formatage des paquets";
        break;
    }


    if(m_type!=NONDEFINI)
    {
        QApplication::alert(this);
        QSound::play(":mix/alert");
    }

    m_tailleFichier = 0;
    m_nomFichier.clear();
    m_nomFichierLen = 0;
    m_type = 0;
}

void MainWindow::connecte()
{
    fen->done(1);
    envoyer("viens de se connecter",INFO);
    //envoyer(*m_avatar);
    setMsmAreaState(true);
    m_page->append(tr("<em>Connexion Réussie!</em>"));
    fen->m_connexion->setEnabled(true);
}

void MainWindow::deconnecte()
{
    fen->show();
    setMsmAreaState(false);
    m_page->append(tr("<em>Vous êtes maintenant déconnecté du serveur</em>"));
    fen->m_connexion->setEnabled(true);
}

void MainWindow::erreurSocket(QAbstractSocket::SocketError erreur)
{
    switch (erreur)
    {

    case QAbstractSocket::HostNotFoundError:
        m_page->append(tr("<em>ERREUR: Le serveur n'a pu etre trouvé. Vérifier l'IP et le PORT.</em>"));
        break;

    case QAbstractSocket::ConnectionRefusedError:
        m_page->append(tr("<em>ERREUR: Le serveur a refusé la connexion. Vérifier si le ") +
                                  tr("programme \"QTchat_serveur\" a bien été démarré. Vérifier aussi ") +
                                  tr("l'IP et le PORT.</em>"));
       break;

    case QAbstractSocket::RemoteHostClosedError:
        m_page->append(tr("<em>ERREUR: Le serveur a coupé la connexion.</em>"));
        break;

    default:
        m_page->append(tr("<em>Erreur: ") + m_socket->errorString() + tr(".</em>"));
        break;
    }

    fen->m_connexion->setEnabled(true);
}

void MainWindow::on_m_connexion_clicked()
{
    if(m_pseudoLE->text().isEmpty())
    {
        QMessageBox::warning(this,tr("ERREUR"),tr("Il faut spécifier un pseudo utilisateur."));
        return;
    }
    m_page->append(tr("<em>Tentative de connection en cours...</em>"));
    fen->m_connexion->setEnabled(false);
    m_socket->abort();
    m_socket->connectToHost(fen->m_ipLE->text(),fen->m_portSB->value());

}


void MainWindow::on_m_envoyer_clicked()
{
   if(m_fichierLE->text().isEmpty())
       return;

   QString fileName={""};

   /*if(m_fichierLE->text().contains(".ogg"))
       m_page->append("<video controls src=\"" + m_fichierLE->text() + "\" > Video</video>");
   else if(m_fichierLE->text().contains(".jpg",Qt::CaseInsensitive) ||
           m_fichierLE->text().contains(".png",Qt::CaseInsensitive) ||
           m_fichierLE->text().contains(".jpeg",Qt::CaseInsensitive) ||
           m_fichierLE->text().contains(".bmp",Qt::CaseInsensitive) ||
           m_fichierLE->text().contains(".tif",Qt::CaseInsensitive) ||
           m_fichierLE->text().contains(".gif",Qt::CaseInsensitive))
       m_page->append("<img src=\"" + m_fichierLE->text() + "\" widht=\"200\"  height=\"200\"\\>");
   else
       m_page->append("<img src=\"" + m_fichierLE->text() + "\" \\>");

*/

   afficherFichier(m_fichierLE->text(),0);
   fileName = m_fichierLE->text().section("/",-1);


   QFile file(m_fichierLE->text());
   if (!file.open(QIODevice::ReadOnly))
   {
       QMessageBox::critical(this,"",tr("Erreur lors de la lecture du fichier: ")  + file.errorString());
       return;
   }

   qDebug() << "ouverture ok";
   qDebug() << "taille du fichier: "<< file.size();

   //QByteArray fileContend;
   //fileContend.resize(file.size()*8);
   //fileContend = file.readAll();
   //qDebug() << "taille de byteArray: "<< fileContend.size();
   envoyerATous(file.readAll(),fileName);
   qDebug() << "envoie ok";
   file.close();


   m_page->append(fileName);
}

void MainWindow::on_m_fichierLE_returnPressed()
{
    on_m_envoyer_clicked();
}

void MainWindow::on_m_deconnexion_clicked()
{
    envoyer("viens de se deconnecter",INFO);
    m_socket->disconnectFromHost();
}

void MainWindow::on_m_ouvrir_clicked()
{
    QString filePath = {""};
    filePath = QFileDialog::getOpenFileName(this);
    m_fichierLE->setText(filePath);
    m_envoyer->setFocus();

}

void MainWindow::onHideAction(bool checked)
{
    if (checked)
    {
        tabWidget->setMaximumHeight(tabWidget->tabBar()->height());
        tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_hide->setText("▴");
    }
    else
    {
        tabWidget->setMaximumHeight(220); // just a very big number
        tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_hide->setText("▾");
    }
}

void MainWindow::onTabBarClicked()
{
    m_hide->setChecked(false);
}


void MainWindow::on_m_retour_clicked()
{
    fen->show();
}

void MainWindow::on_m_envoyerPB_clicked()
{
    if(m_messageLE->text().isEmpty())
        return;
    envoyerATous(m_messageLE->text());
}

void MainWindow::on_m_messageLE_returnPressed()
{
    on_m_envoyerPB_clicked();
}

void MainWindow::on_m_pseudoLE_textChanged(QString str)
{
    m_pseudo = str;
}

void MainWindow::on_m_avatarPB_clicked()
{
    QString filePath = {""};
    QFileDialog *dialogue = new QFileDialog(this);
    filePath = dialogue->getOpenFileName(this,QString(),QString(),tr("Image(*.png *.jpg *.jepg *.gif *.tif *.bmp *.xpm)"));
    if(filePath != "")
    {
        m_avatar->load(filePath);
        *m_avatar = m_avatar->scaled(100,100,Qt::KeepAspectRatio);
        labelAvatar->setPixmap(m_avatar->scaled(100,100,Qt::KeepAspectRatio));
        labelAvatar->setMaximumWidth(100);
        labelAvatar->setMaximumHeight(100);
        //envoyer(m_avatar->scaled(100,100,Qt::KeepAspectRatio));
    //m_fichierLE->setText(filePath);
    }

}

void MainWindow::onM_stopAll_clicked(void)
{
    envoyer("viens de se deconnecter",INFO);
    m_socket->abort();
    int n=listWidget->count();

    for(int i=0;i<n;i++)
        listWidget->takeItem(0);
}

void MainWindow::on_m_messagePrivePB_clicked(void)
{
    fenPrive->exec();
}

void MainWindow::onFenPriveEnvoyerClicked(void)
{
    bool selected=false;

    for(int i=0;i<listWidget->count();i++)
    {
        if(listWidget->item(i)->isSelected())
            selected=true;
    }
    if(selected)
    {
        if(fenPrive->m_message->text().isEmpty())
            return;
        envoyer(fenPrive->m_message->text(),MSGPRIVE,listWidget->currentItem()->text());
    }
}

void MainWindow::onFenPriveAnnulerClicked(void)
{
    fenPrive->done(0);
}

void MainWindow::refreshProgressBar(qint64 value)
{
    byteWritten +=value;
    int a = static_cast<int>((double)byteWritten/(double)m_tailleFichierEnvoye * 100);
   // qDebug() <<"taille = " << m_tailleFichierEnvoye << "\tvalue= "<<value<<"\t a= "<< a;
    m_chargement->setValue(a);
    if(byteWritten >= m_tailleFichierEnvoye)
        byteWritten = 0;
}

void MainWindow::insererSmiley(void)
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    int res=0;

    for(auto a:button->objectName())
        res+=(int)a.unicode();

    switch (res) {

    case 1084: //pushButton somme des unicodes
        envoyer("<img src=\":smiley/s0\">",MSGPUBLIC);
        break;

    case 1229://pushButton_2
        envoyer("<img src=\":smiley/s1\">",MSGPUBLIC);
        break;

    case 1230://pushButton_3
        envoyer("<img src=\":smiley/s2\">",MSGPUBLIC);
        break;

    case 1231:
        envoyer("<img src=\":smiley/s3\">",MSGPUBLIC);
        break;

    case 1232:
        envoyer("<img src=\":smiley/s4\">",MSGPUBLIC);
        break;

    case 1233:
        envoyer("<img src=\":smiley/s5\">",MSGPUBLIC);
        break;

    case 1234:
        envoyer("<img src=\":smiley/s6\">",MSGPUBLIC);
        break;

    case 1235:
        envoyer("<img src=\":smiley/s7\">",MSGPUBLIC);
        break;

    case 1236:
        envoyer("<img src=\":smiley/s8\">",MSGPUBLIC);
        break;

    case 1276:
        envoyer("<img src=\":smiley/s9\">",MSGPUBLIC);
        break;

    case 1277:
        envoyer("<img src=\":smiley/s10\">",MSGPUBLIC);
        break;

    case 1278:
        envoyer("<img src=\":smiley/s11\">",MSGPUBLIC);
        break;

    case 1279:
        envoyer("<img src=\":smiley/s12\">",MSGPUBLIC);
        break;

    case 1280:
        envoyer("<img src=\":smiley/s13\">",MSGPUBLIC);
        break;

    case 1281:
        envoyer("<img src=\":smiley/s14\">",MSGPUBLIC);
        break;

    default:
        break;
    }
}

MainWindow::~MainWindow()
{
}

