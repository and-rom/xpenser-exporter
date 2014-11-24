#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QProgressDialog>

QT_BEGIN_NAMESPACE
class QProgressDialog;
class QSslError;
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

class httprequest : public QObject
{
    Q_OBJECT
public:
    explicit httprequest(QObject *parent = 0);
    void startRequest();

public slots:
    //void cancelDownload();
    void download(QString,QString,QString);
private slots:
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void slotAuthenticationRequired(QNetworkReply*,QAuthenticator *);
#ifndef QT_NO_OPENSSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif
private:
    QString replytext;
    QString method;
    QString request;
    QProgressDialog *progressDialog;

    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    bool httpRequestAborted;
signals:
    void setLabel(QString);
    void setButtonStatus(bool);
    void sendReply(QString);
};

#endif // HTTPREQUEST_H
