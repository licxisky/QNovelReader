#include "qthreaddownloadnovelchapter.h"

QThreadDownLoadNovelChapter::QThreadDownLoadNovelChapter(QUrl *url, QString unicode, QRule rule, QString filename)
{
    this->url = url;
    this->unicode = unicode;
    this->rule = rule;
    this->filename = filename;
}

void QThreadDownLoadNovelChapter::run()
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

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        emit onFinished(false);
        return;
    }
    QTextStream out(&file);
    out << chapter;
    out.flush();
    file.close();
    emit onFinished(true);
}
