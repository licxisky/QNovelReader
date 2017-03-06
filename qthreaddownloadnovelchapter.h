#ifndef QTHREADDOWNLOADNOVELCHAPTER_H
#define QTHREADDOWNLOADNOVELCHAPTER_H

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include "qrule.h"

class QThreadDownLoadNovelChapter : public QThread
{
    Q_OBJECT
public:
    QThreadDownLoadNovelChapter(QUrl *url, QString unicode, QRule rule, QString filename);

protected:
    void run();

public slots:
    void emitProgress(qint64, qint64);

signals:
    void downloadProgress(qint64, qint64);
    void onFinished(bool);

private:
    QUrl *url;
    QString unicode;
    QRule rule;
    QString filename;

};

#endif // QTHREADDOWNLOADNOVELCHAPTER_H
