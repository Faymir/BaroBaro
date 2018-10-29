#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>
#include <QPixmap>
#include <QByteArray>

const quint8 NONDEFINI=0,INFO=1,MSGPUBLIC=2,MSGPRIVE=3,AVATAR=4,USERNAME=5,FICHIER=6;

class Client
{
    public:

        Client(QTcpSocket* socket = Q_NULLPTR,QString name="",QPixmap * avatar = Q_NULLPTR);
        ~Client();

        const QString getPseudo(void) const {   return m_pseudo;    }
        QPixmap getPixmap (void)    const   {   return *m_avatar;   }
        QTcpSocket * getSocket(void) const  {   return m_socket;    }

        void setPseudo(const QString& pseudo);
        void setAvatar(const QString& path);
        void setSocket(QTcpSocket* socket);
        void write(const QByteArray &paquet);

    protected:
        QTcpSocket *m_socket;
        QString m_pseudo;
        QPixmap *m_avatar;
};

#endif // CLIENT_H
