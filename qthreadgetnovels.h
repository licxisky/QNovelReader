#ifndef QTHREADGETNOVELS_H
#define QTHREADGETNOVELS_H

#include <QObject>
#include <QThread>
#include <QUrl>
#include <QList>
#include <QTextCodec>
#include <QEventLoop>
#include <QNetworkReply>
#include <QStringListModel>
#include <QNetworkAccessManager>

#include "qrule.h"
#include "qnovel.h"

class QThreadGetNovels : public QThread
{
    Q_OBJECT
public:
    QThreadGetNovels(QUrl *url, const QString &unicode, const QRule &rule);

protected:
    void run();

public slots:
    void emitProgress(qint64 v, qint64 m);

signals:
    void downloadProgress(qint64, qint64);
    void onFinished(const QList<QNovel>);

private:
    QUrl *url;
    QString unicode;
    QRule rule;
};

#endif // QTHREADGETNOVELS_H
