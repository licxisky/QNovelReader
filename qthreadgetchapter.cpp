#include "qthreadgetchapter.h"

QThreadGetChapter::QThreadGetChapter(QUrl *url, const QString &unicode, const QRule &rule, int i)
{
    this->url = url;
    this->unicode = unicode;
    this->rule = rule;
    this->i = i;
}

void QThreadGetChapter::run()
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(*url));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(emitProgress(qint64, qint64)));
    loop.exec();
    QTextCodec *codec = QTextCodec::codecForName(unicode.toLocal8Bit().data());
    QString html = codec->toUnicode(reply->readAll());

    int pos_start = 0;
    int pos_end = 0;
    pos_start = html.indexOf(rule.chapterContentS);if(pos_start < 0) return;
    pos_end = html.indexOf(rule.chapterContentE, pos_start);if(pos_end < 0) return;
    QString chapter = html.mid(pos_start + rule.chapterContentS.length(), pos_end - pos_start - rule.chapterContentS.length());

    emit onFinished(chapter, i);
}

void QThreadGetChapter::emitProgress(qint64 v, qint64 m)
{
    emit downloadProgress(v, m);
}
