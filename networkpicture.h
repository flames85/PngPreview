#ifndef __NETWORK_PICTURE_H__
#define __NETWORK_PICTURE_H__

#include <QObject>
#include <QUrl>

class QNetworkAccessManager;
class QPixmap;
class QNetworkReply;

class NetworkPicture : public QObject
{
    Q_OBJECT
public:
    explicit NetworkPicture(QObject *parent=0);
    virtual ~NetworkPicture();
    void Get(const QUrl &url);
signals:
    void ShowNetworkPicture(QNetworkReply *);
protected:

private slots:

    void replyFinished(QNetworkReply *reply);

public:
    QUrl                     m_url;

private:
    QNetworkAccessManager   *m_manager;

};

#endif // __NETWORK_PICTURE_H__
