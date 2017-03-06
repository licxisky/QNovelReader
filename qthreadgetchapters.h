#ifndef QTHREADGETCHAPTERS_H
#define QTHREADGETCHAPTERS_H

#include <QUrl>
#include <QList>
#include <QObject>
#include <QThread>
#include <QTextCodec>
#include <QEventLoop>
#include <QNetworkReply>
#include <QStringListModel>
#include <QNetworkAccessManager>

#include "qrule.h"
#include "qchapter.h"

class QThreadGetChapters : public QThread
{
    Q_OBJECT
public:
    QThreadGetChapters(QUrl *url, const QString &unicode, const QRule &rule);

protected:
    void run();
    QString parent();

public slots:
    void emitProgress(qint64, qint64);

signals:
    void downloadProgress(qint64, qint64);
    void onFinished(const QList<QChapter>);

private:
    QUrl *url;
    QString unicode;
    QRule rule;
    QString parentUrl;
};

#endif // QTHREADGETCHAPTERS_H
