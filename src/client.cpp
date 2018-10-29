#include "client.h"

Client::Client(QTcpSocket* socket,QString name,QPixmap * avatar)
{
    if(socket != Q_NULLPTR)
        m_socket=socket;
    else
        m_socket = new QTcpSocket;

    m_pseudo = name;

    if(avatar != Q_NULLPTR)
        m_avatar = avatar;
    else
        m_avatar = new QPixmap();
}

void Client::setPseudo(const QString &pseudo)
{
    m_pseudo = pseudo;
}

void Client::setAvatar(const QString &path)
{
    m_avatar->load(path);
}

void Client::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
}

void Client::write(const QByteArray &paquet)
{
    m_socket->write(paquet);
}

Client::~Client()
{
}
