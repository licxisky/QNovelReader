#include "qthreadgetchapters.h"

QThreadGetChapters::QThreadGetChapters(QUrl *url, const QString &unicode, const QRule &rule)
{
    this->url = url;
    this->unicode = unicode;
    this->rule = rule;

}

void QThreadGetChapters::run()
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(*url));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(emitProgress(qint64, qint64)));
    loop.exec();
    QTextCodec *codec = QTextCodec::codecForName(unicode.toLocal8Bit().data());
    QString html = codec->toUnicode(reply->readAll());

    int start = 0, end = 0;

    parentUrl = parent();
    QList<QChapter> chapters;
    QString p= url->url();
    while((start = html.indexOf(rule.chapterUrlS, start)) != -1)
    {

        QChapter chapter;
        end = html.indexOf(rule.chapterUrlE, start + rule.chapterUrlS.length());if(end < 0) break;
        chapter.url = html.mid(start + rule.chapterUrlS.length(), end - start - rule.chapterUrlS.length());
        if(chapter.url.at(0) == '/') chapter.url = rule.hostUrl.url() + chapter.url;
        else if(chapter.url.mid(0, 7) != "http://") chapter.url = parentUrl + chapter.url;
        start = html.indexOf(rule.chapterNameS, end + rule.chapterUrlE.length()); if(start < 0) break;
        end = html.indexOf(rule.chapterNameE, start + rule.chapterNameS.length());if(end < 0) break;
        chapter.name = html.mid(start + rule.chapterNameS.length(), end - start - rule.chapterNameS.length());
        chapters.push_back(chapter);
    }
    emit onFinished(chapters);
}

QString QThreadGetChapters::parent()
{
    QString s = url->url();
    int i = s.lastIndexOf('/');
    return i < 7 ? s : s.mid(0, i+1);
}

void QThreadGetChapters::emitProgress(qint64 m, qint64 v)
{
    emit downloadProgress(v, m);
}
