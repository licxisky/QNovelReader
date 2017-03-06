#ifndef QTHREADGETCHAPTER_H
#define QTHREADGETCHAPTER_H

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

class QThreadGetChapter : public QThread
{
    Q_OBJECT
public:
    QThreadGetChapter(QUrl *url, const QString &unicode, const QRule &rule, int i = 0);

protected:
    void run();

public slots:
    void emitProgress(qint64, qint64);

signals:
    void downloadProgress(qint64, qint64);
    void onFinished(const QString, int);

private:
    QUrl *url;
    QString unicode;
    QRule rule;
    int i;
};

#endif // QTHREADGETCHAPTER_H
