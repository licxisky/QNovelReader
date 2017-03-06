#ifndef QNOVELREADER_H
#define QNOVELREADER_H

#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include <QFile>
#include <QList>
#include <QLabel>
#include <QString>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QCloseEvent>
#include <QTextStream>
#include <QProgressBar>
#include <QSystemTrayIcon>
#include <QStringListModel>

#include "qrule.h"
#include "qnovel.h"
#include "qchapter.h"
#include "qreadhistory.h"
#include "qsearchhistory.h"

#include "qthreadgetnovels.h"
#include "qthreadgetchapters.h"
#include "qthreadgetchapter.h"

namespace Ui {
class QNovelReader;
}

class QNovelReader : public QMainWindow
{
    Q_OBJECT

public:
    explicit QNovelReader(QWidget *parent = 0);
    ~QNovelReader();

public slots:
    void getNovels();                           //获取小说列表
    void getChapters(QModelIndex);              //获取小说章节目录
    void getChapter(QModelIndex);               //获取小说章节

    void updateProgress(qint64, qint64);        //更新进度条
    void updateListView(const QList<QNovel>);   //更新ListView显示小说
    void updateListView(const QList<QChapter>); //更新ListView显示章节目录
    void updateTextArea(const QString, int);         //更新TextArea显示章节内容

    void setTextEditFont(QFont);                //设置TextEdit的字体
    void setTextEditFontPointSize(QString);     //设置TextEdit的字号

    void refreshChapter();                      //刷新章节
    void getNextChapter();                      //下一章
    void getPreviousChapter();                  //上一章

    void downloadChapter();                     //下载当前章节
    void downlaodNovel();                       //下载当前小说

    void changeHost(int);                       //改变小说来源主机

    void exitNovelReader();                     //退出程序

    void onDoubleClickReadHistory(QModelIndex); //双击阅读历史
    void updateListViewReadHistory(const QList<QChapter>);
    void updateTextAreaReadHistory(const QString, int i);

    void importReadHistoryFile();               //导入阅读历史文件
    void exportReadHistoryFile();               //导出阅读历史文件

    void deleteSelectedReadHistory();
    void deleteSelectedSearchHistory();

    void updateChapterContent(const QString, int);


private:
    //变量
    Ui::QNovelReader *ui;                       //界面

    int novelId;                                //当前阅读的小说id
    int chapterId;                              //当前阅读的章节id
    int hostId;                                 //当前主机id

    QString ruleFileName;                       //规则文件名
    QString readHistoryFileName;                //阅读历史文件名
    QString searchHistoryFileName;              //搜索历史文件名

    QProgressBar *progressBar;                     //状态栏进度条
    QLabel *label;                              //状态栏标签

    QSystemTrayIcon *trayIcon;                  //托盘区

    QList<QNovel> novelList;                    //小说列表
    QList<QRule> ruleList;                      //规则列表
    QList<QReadHistory> readHistoryList;        //阅读历史列表
    QList<QSearchHistory> searchHistoryList;    //搜索历史列表

    //方法
    void closeEvent(QCloseEvent *e);            //窗口关闭（重载）

    void initHostComboBox();                    //初始化主机选择框
    void initSystemTrayIcon();                  //初始化托盘区图标

    void readRule();                            //读取规则
    void writeRule();                           //写入规则

    void readReadHistory();                     //读取阅读历史
    void writeReadHistory();                    //写入阅读历史
    void updateReadHistory();                   //更新阅读历史
    void updateListViewReadHistory();           //更新阅读历史界面
    int readHistoryIsContainsNovel(const QNovel &novel);

    void readSearchHistory();                   //读取搜索历史
    void writeSearchHistory();                  //写入搜索历史
    void updateSearchHistory();                 //更新搜索历史
    void updateListViewSearchHistory();         //更新搜索历史界面

    QString currentTime(const QString &format); //返回当前时间
    int isContainsHost(const QString &host);
    QByteArray readFile(const QString &filename);
    bool writeFile(const QString &filename, const QByteArray &filecontent);

    void getNextFiveChapters();
};

#endif // QNOVELREADER_H
