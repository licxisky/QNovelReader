#include "qthreadgetnovels.h"

QThreadGetNovels::QThreadGetNovels(QUrl *url, const QString &unicode, const QRule &rule)
{
    this->url = url;
    this->unicode = unicode;
    this->rule = rule;
}

void QThreadGetNovels::run()
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(*url));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(emitProgress(qint64, qint64)));
    loop.exec();
    QTextCodec *codec = QTextCodec::codecForName(unicode.toLocal8Bit().data());
    QString html = codec->toUnicode(reply->readAll());

    QList<QNovel> novelList;
    int start = 1, end = 1;
    while((start = html.indexOf(rule.novelUrlS, start)) != -1)
    {
        QNovel novel;
        novel.host = rule.hostUrl.host();
        end = html.indexOf(rule.novelUrlE, start + rule.novelUrlS.length());if(end < 0) return;
        novel.url = html.mid(start + rule.novelUrlS.length(), end - start - rule.novelUrlS.length());
        start = html.indexOf(rule.novelNameS, end + rule.novelUrlE.length()); if(start < 0) return;
        end = html.indexOf(rule.novelNameE, start + rule.novelNameS.length());if(end < 0) return;
        novel.name = html.mid(start + rule.novelNameS.length(), end - start - rule.novelNameS.length());
        start = html.indexOf(rule.novelAuthorS, end + rule.novelNameE.length()); if(start < 0) return;
        end = html.indexOf(rule.novelAuthorE, start + rule.novelAuthorS.length());if(end < 0) return;
        novel.author = html.mid(start + rule.novelAuthorS.length(), end - start - rule.novelAuthorS.length()).trimmed();
        novelList.push_back(novel);
    }
    emit onFinished(novelList);
}

void QThreadGetNovels::emitProgress(qint64 v, qint64 m)
{
    emit downloadProgress(v, m);
}
