#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPixmap>
#include <QUrl>
#include <qDebug>
#include "networkpicture.h"

NetworkPicture::NetworkPicture(QObject *parent):
    QObject(parent),
    m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(replyFinished(QNetworkReply*)));
}

NetworkPicture::~NetworkPicture()
{

}

void NetworkPicture::Get(const QUrl &url)
{
    m_url = url;
    m_manager->get(QNetworkRequest(url));
}

void NetworkPicture::replyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "replyFinished ok!";
        //获取字节流构造 QPixmap 对象
        emit ShowNetworkPicture(reply);
    }
}
