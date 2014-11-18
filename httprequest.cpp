#include <QtNetwork>
#include <QMessageBox>
#include <QProgressDialog>
#include "ui_authenticationdialog.h"
#include "httprequest.h"
httprequest::httprequest(QObject *parent) :
    QObject(parent)
{
    replytext.clear();

    progressDialog = new QProgressDialog(0);


    connect(&qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(slotAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
#ifndef QT_NO_OPENSSL
    connect(&qnam, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif
//[1]
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
}

void httprequest::startRequest()
{
    if (method=="get") {
        reply = qnam.get(QNetworkRequest(url));
    } else if (method=="post") {
        QByteArray tmp;
        tmp.append(request);
        reply = qnam.post(QNetworkRequest(url),tmp);
    }

    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void httprequest::download(QString urltext,QString _method,QString _request)
{
    url=urltext;
    method=_method;
    request=_request;

//[2]
    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading."));
    emit setButtonStatus(false);

    // schedule the request
    httpRequestAborted = false;

    startRequest();
}

void httprequest::cancelDownload()
{
    emit setLabel(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    emit setButtonStatus(true);
}

void httprequest::httpFinished()
{
    if (httpRequestAborted) {
        reply->deleteLater();
        //[3]
        progressDialog->hide();
        return;
    }
//[4]
    progressDialog->hide();



    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        QMessageBox::information(0, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
        emit setButtonStatus(true);
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(0, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            url = newUrl;
            reply->deleteLater();
            startRequest();
            return;
        }
    } else {
        emit setLabel(tr("Downloaded."));
        emit setButtonStatus(true);
    }

    reply->deleteLater();
    reply = 0;
    emit sendReply(replytext);//emit signal send reply
    replytext.clear();
}

void httprequest::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    replytext.append(reply->readAll());
}

void httprequest::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;
//[5]
    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(bytesRead);
}


void httprequest::slotAuthenticationRequired(QNetworkReply*,QAuthenticator *authenticator)
{
    QDialog dlg;
    Ui::Dialog ui;
    ui.setupUi(&dlg);
    dlg.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm()).arg(url.host()));

    // Did the URL have information? Fill the UI
    // This is only relevant if the URL-supplied credentials were wrong
    ui.userEdit->setText(url.userName());
    ui.passwordEdit->setText(url.password());

    ui.userEdit->setText("type username");
    ui.passwordEdit->setText("type password");

    if (dlg.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }

}

#ifndef QT_NO_OPENSSL
void httprequest::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }

    if (QMessageBox::warning(0, tr("HTTP"),
                             tr("One or more SSL errors has occurred: %1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
#endif
