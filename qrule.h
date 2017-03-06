#ifndef QRULE_H
#define QRULE_H

#include <QObject>
#include <QUrl>

class QRule
{
public:
    QRule();
    QUrl hostUrl;
    QString hostName;
    QString novelUrlS;
    QString novelUrlE;
    QString novelNameS;
    QString novelNameE;
    QString novelAuthorS;
    QString novelAuthorE;
    QString chapterUrlS;
    QString chapterUrlE;
    QString chapterNameS;
    QString chapterNameE;
    QString chapterContentS;
    QString chapterContentE;
    QString hostSearchS;
    QString hostSearchE;
};

#endif // QRULE_H
