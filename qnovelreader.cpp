#include "qnovelreader.h"
#include "ui_qnovelreader.h"

QNovelReader::QNovelReader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QNovelReader)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":images/du.png"));

    qRegisterMetaType<QList<QNovel>>("QList<QNovel>");
    qRegisterMetaType<QList<QChapter>>("QList<QChapter>");

    ruleFileName            = "RULE.rule";
    readHistoryFileName     = "READHISTORY.history";
    searchHistoryFileName   = "SEARCHHISTORY.history";

    readRule();
    readReadHistory();
    readSearchHistory();

    updateListViewReadHistory();
    updateListViewSearchHistory();

    label = new QLabel();
    label->setMaximumHeight(14);
    progressBar = new QProgressBar();
    progressBar->setMaximumSize(250, 14);

    progressBar->hide();

    ui->statusBar->addWidget(label);
    ui->statusBar->addWidget(progressBar);

    initHostComboBox();
    initSystemTrayIcon();

    ui->pushButton_download_this_chapter->setEnabled(false);
    ui->pushButton_download_this_novel->setEnabled(false);
    ui->pushButton_next_chapter->setEnabled(false);
    ui->pushButton_previous_chapter->setEnabled(false);
    ui->pushButton_refresh->setEnabled(false);

    connect(ui->pushButton_search, SIGNAL(clicked()), this, SLOT(getNovels()));
    connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(refreshChapter()));
    connect(ui->pushButton_next_chapter, SIGNAL(clicked()), this, SLOT(getNextChapter()));
    connect(ui->comboBox_host, SIGNAL(currentIndexChanged(int)), this, SLOT(changeHost(int)));
    connect(ui->pushButton_previous_chapter, SIGNAL(clicked()), this, SLOT(getPreviousChapter()));
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(setTextEditFont(QFont)));
    connect(ui->pushButton_read_history_import, SIGNAL(clicked()), this, SLOT(importReadHistoryFile()));
    connect(ui->pushButton_read_history_export, SIGNAL(clicked()), this, SLOT(exportReadHistoryFile()));
    connect(ui->pushButton_read_history_delete, SIGNAL(clicked()), this, SLOT(deleteSelectedReadHistory()));
    connect(ui->pushButton_search_history_delete, SIGNAL(clicked()), this, SLOT(deleteSelectedSearchHistory()));
    connect(ui->listView_read_history, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClickReadHistory(QModelIndex)));
    connect(ui->comboBox_font_point_size, SIGNAL(currentTextChanged(QString)), this, SLOT(setTextEditFontPointSize(QString)));
}

QNovelReader::~QNovelReader()
{
    delete ui;
    delete progressBar;
    delete label;
    delete trayIcon;
}

void QNovelReader::getNovels()
{
    if(hostId < 0) return;

    disconnect(ui->listView_novel_and_chapter_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(getChapter(QModelIndex)));


    ui->textEdit_content->setText("");
    ui->pushButton_search->setEnabled(false);
    label->setText(tr("正在搜索小说..."));

    QString url = ruleList.at(hostId).hostSearchS + ui->lineEdit_search->text().toUtf8().toPercentEncoding() + ruleList.at(hostId).hostSearchE;
    QThreadGetNovels *thread = new QThreadGetNovels(new QUrl(url), "UTF-8", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QList<QNovel>)), this, SLOT(updateListView(QList<QNovel>)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    updateSearchHistory();
    updateListViewSearchHistory();
    writeSearchHistory();

}

void QNovelReader::getChapters(QModelIndex index)
{
    if(hostId < 0) return;
    novelId = index.row();

    disconnect(ui->listView_novel_and_chapter_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(getChapters(QModelIndex)));

    label->setText(tr("正在抓取目录..."));

    QString url = novelList.at(novelId).url;
    QThreadGetChapters *thread = new QThreadGetChapters(new QUrl(url), "GBK", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QList<QChapter>)), this, SLOT(updateListView(QList<QChapter>)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
}

void QNovelReader::getChapter(QModelIndex index)
{
    if(novelId < 0) return;
    chapterId = index.row();

    //缓存机制
    if(novelList.at(novelId).chapters.at(chapterId).content != "")
    {
        updateTextArea(novelList.at(novelId).chapters.at(chapterId).content, 0);

        return;
    }

    label->setText(tr("正在抓取章节..."));

    QString url = novelList.at(novelId).chapters.at(chapterId).url;
    QThreadGetChapter *thread = new QThreadGetChapter(new QUrl(url), "GBK", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QString, int)), this, SLOT(updateTextArea(QString, int)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    getNextFiveChapters();
}

void QNovelReader::updateProgress(qint64 v, qint64 m)
{
    progressBar->setValue(v);
    progressBar->setMaximum(m);
}

void QNovelReader::updateListView(const QList<QNovel> novelList)
{
    this->novelList = novelList;

    QStringList qstringList;

    for(int i = 0; i < novelList.size(); i++)
        qstringList += novelList.at(i).name;

    ui->listView_novel_and_chapter_list->setModel(new QStringListModel(qstringList));
    novelId = -1;

    label->setText(tr("小说抓取完成"));
    ui->pushButton_search->setEnabled(true);

    ui->pushButton_download_this_chapter->setEnabled(false);
    ui->pushButton_download_this_novel->setEnabled(false);
    ui->pushButton_next_chapter->setEnabled(false);
    ui->pushButton_previous_chapter->setEnabled(false);
    ui->pushButton_refresh->setEnabled(false);

    ui->label_novel_name->setText("");
    ui->label_author_name->setText("");
    ui->label_chapter_name->setText("");

    connect(ui->listView_novel_and_chapter_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(getChapters(QModelIndex)));
}

void QNovelReader::updateListView(const QList<QChapter> chapterList)
{
    novelList[novelId].chapters = chapterList;

    QStringList qstringList;

    for(int i = 0; i < chapterList.size(); i++)
        qstringList += chapterList.at(i).name;

    ui->listView_novel_and_chapter_list->setModel(new QStringListModel(qstringList));
    chapterId = -1;

    label->setText(tr("目录抓取完成"));

    ui->pushButton_download_this_novel->setEnabled(true);

    ui->label_novel_name->setText(novelList.at(novelId).name);
    ui->label_author_name->setText(novelList.at(novelId).author);
    ui->label_chapter_name->setText("");

    connect(ui->listView_novel_and_chapter_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(getChapter(QModelIndex)));
}

void QNovelReader::updateTextArea(const QString content, int i)
{
    novelList[novelId].chapters[chapterId + i].content = content;

    ui->textEdit_content->setText(content);

    label->setText(tr("章节抓取完成"));

    ui->pushButton_download_this_chapter->setEnabled(true);
    ui->pushButton_next_chapter->setEnabled(true);
    ui->pushButton_previous_chapter->setEnabled(true);
    ui->pushButton_refresh->setEnabled(true);

    ui->label_chapter_name->setText(novelList.at(novelId).chapters.at(chapterId).name);

    updateReadHistory();
    updateListViewReadHistory();
}

void QNovelReader::setTextEditFont(QFont font)
{
    font.setPointSize(ui->textEdit_content->font().pointSize());
    ui->textEdit_content->setFont(font);
}

void QNovelReader::setTextEditFontPointSize(QString size)
{
    QFont ft = ui->textEdit_content->font();
    ft.setPointSize(size.toInt());
    ui->textEdit_content->setFont(ft);
}

void QNovelReader::refreshChapter()
{
    label->setText(tr("正在抓取章节..."));

    QString url = novelList.at(novelId).chapters.at(chapterId).url;
    QThreadGetChapter *thread = new QThreadGetChapter(new QUrl(url), "GBK", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QString, int)), this, SLOT(updateTextArea(QString, int)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
}

void QNovelReader::getNextChapter()
{
    if(chapterId >= novelList.at(novelId).chapters.size() - 1)
    {
        QMessageBox::information(NULL, tr("提示"), tr("已经是最后一章了！"), QMessageBox::Yes);
        return;
    }
    chapterId++;


    //缓存机制
    if(novelList.at(novelId).chapters.at(chapterId).content != "")
    {
        updateTextArea(novelList.at(novelId).chapters.at(chapterId).content, 0);

        return;
    }

    label->setText(tr("正在抓取章节..."));

    QString url = novelList.at(novelId).chapters.at(chapterId).url;
    QThreadGetChapter *thread = new QThreadGetChapter(new QUrl(url), "GBK", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QString, int)), this, SLOT(updateTextArea(QString, int)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    getNextFiveChapters();
}

void QNovelReader::getPreviousChapter()
{
    if(chapterId == 0)
    {
        QMessageBox::information(NULL, tr("提示"), tr("这是第一章！"), QMessageBox::Yes);
        return;
    }
    chapterId--;

    //缓存机制
    if(novelList.at(novelId).chapters.at(chapterId).content != "")
    {
        updateTextArea(novelList.at(novelId).chapters.at(chapterId).content, 0);

        return;
    }

    label->setText(tr("正在抓取章节..."));

    QString url = novelList.at(novelId).chapters.at(chapterId).url;
    QThreadGetChapter *thread = new QThreadGetChapter(new QUrl(url), "GBK", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QString, int)), this, SLOT(updateTextArea(QString, int)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    getNextFiveChapters();
}

void QNovelReader::downloadChapter()
{

}

void QNovelReader::downlaodNovel()
{

}

void QNovelReader::changeHost(int index)
{
    hostId = index;
}

void QNovelReader::exitNovelReader()
{
    QMessageBox::StandardButton sure;
    sure = QMessageBox::question(this, tr("退出程序"), tr("确认退出程序?"), QMessageBox::Yes | QMessageBox::No);

    if (sure == QMessageBox::No) hide();
    else if (sure == QMessageBox::Yes) close();
}

void QNovelReader::onDoubleClickReadHistory(QModelIndex index)
{
    QReadHistory r = readHistoryList.at(index.row());

    novelList.clear();
    QNovel t(r.novelName, r.novelAuthor, "http://" + r.novelUrl.host(), r.novelUrl.url());
    novelList.push_back(t);
    novelId = 0;
    chapterId = r.chapterId;
    if((hostId = isContainsHost(t.host)) == -1)
    {
        QMessageBox::warning(NULL, tr("错误"), tr("不存在主机")+t.host+"!", QMessageBox::Yes);
        return;
    }
    ui->comboBox_host->setCurrentIndex(hostId);

    disconnect(ui->listView_novel_and_chapter_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(getChapters(QModelIndex)));

    label->setText(tr("正在抓取目录..."));

    QString url = novelList.at(novelId).url;
    QThreadGetChapters *thread = new QThreadGetChapters(new QUrl(url), "GBK", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QList<QChapter>)), this, SLOT(updateListViewReadHistory(QList<QChapter>)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));



}

void QNovelReader::updateListViewReadHistory(const QList<QChapter> chapterList)
{
    novelList[novelId].chapters = chapterList;

    QStringList qstringList;

    for(int i = 0; i < chapterList.size(); i++)
        qstringList += chapterList.at(i).name;

    ui->listView_novel_and_chapter_list->setModel(new QStringListModel(qstringList));

    label->setText(tr("目录抓取完成"));

    ui->pushButton_download_this_novel->setEnabled(true);

    ui->label_novel_name->setText(novelList.at(novelId).name);
    ui->label_author_name->setText(novelList.at(novelId).author);
    ui->label_chapter_name->setText("");

    label->setText(tr("正在抓取章节..."));

    QString url = novelList.at(novelId).chapters.at(chapterId).url;
    QThreadGetChapter *thread = new QThreadGetChapter(new QUrl(url), "GBK", ruleList.at(hostId));
    thread->start();
    connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
    connect(thread, SIGNAL(onFinished(QString, int)), this, SLOT(updateTextAreaReadHistory(QString, int)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    ui->tabWidget->setCurrentIndex(0);

    connect(ui->listView_novel_and_chapter_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(getChapter(QModelIndex)));
}

void QNovelReader::updateTextAreaReadHistory(const QString content, int i)
{
    novelList[novelId].chapters[chapterId + i].content = content;

    ui->textEdit_content->setText(content);

    label->setText(tr("章节抓取完成"));

    ui->pushButton_download_this_chapter->setEnabled(true);
    ui->pushButton_next_chapter->setEnabled(true);
    ui->pushButton_previous_chapter->setEnabled(true);
    ui->pushButton_refresh->setEnabled(true);

    ui->label_chapter_name->setText(novelList.at(novelId).chapters.at(chapterId).name);
}

void QNovelReader::importReadHistoryFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("导入阅读历史"), "", tr("历史文件 (*.history);;所有文件 (*.*)"));
    if(path.isNull())
    {

    }
    else
    {
        if(writeFile(readHistoryFileName, readFile(path)))
        {
            readReadHistory();
            updateListViewReadHistory();
        }
    }
}

void QNovelReader::exportReadHistoryFile()
{
    QString path = QFileDialog::getSaveFileName(this, tr("导出阅读历史"), "HISTORY.history", tr("历史文件 (*.history);;所有文件 (*.*)"));
    if(path.isNull())
    {

    }
    else
    {
        if(writeFile(path, readFile(readHistoryFileName)))
        {
        }
    }
}

void QNovelReader::deleteSelectedReadHistory()
{
    int index = ui->listView_read_history->currentIndex().row();
    if(index != -1)
    {
        readHistoryList.removeAt(index);
        updateListViewReadHistory();
        writeReadHistory();
    }
}

void QNovelReader::deleteSelectedSearchHistory()
{
    int index = ui->listView_search_history->currentIndex().row();
    if(index != -1)
    {
        searchHistoryList.removeAt(index);
        updateListViewSearchHistory();
        writeSearchHistory();
    }
}

void QNovelReader::updateChapterContent(const QString content, int i)
{
    novelList[novelId].chapters[chapterId + i].content = content;
}

void QNovelReader::closeEvent(QCloseEvent *e)
{
//    e->ignore();
//    hide();
}

void QNovelReader::initHostComboBox()
{
    for(int i = 0; i < ruleList.size(); i++)
        ui->comboBox_host->addItem(ruleList.at(i).hostName+"("+ruleList.at(i).hostUrl.host()+")");
    hostId = ruleList.size() == 0 ? -1 : 0;
}

void QNovelReader::initSystemTrayIcon()
{
    QList<QAction *> actionList;
    QMenu *menu = new QMenu();
    actionList.push_back(new QAction(tr("退出")));
    menu->addActions(actionList);
    connect(actionList[0], SIGNAL(triggered()), this, SLOT(exitNovelReader()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/du.png"));
    trayIcon->setToolTip(tr("舒悦小说阅读器"));
    trayIcon->setContextMenu(menu);
    trayIcon->show();


}

void QNovelReader::readRule()
{
    ruleList.clear();
    QFile file(ruleFileName);
    if(!file.open(QIODevice::ReadOnly))
    {
//        QMessageBox::information(NULL, tr("提示"), tr("规则文件打开失败，程序功能受限！"), QMessageBox::Yes);
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QStringList qstringList =
                        in.readLine().
                            split(QRegExp(QString() +
                                  "(HOSTNAME: )|( HOSTURL: )|" +
                                  "( HOSTSEARCHURLS: )|( HOSTSEARCHURLE: )|" +
                                  "( NOVELURLS: )|( NOVELURLE: )|" +
                                  "( NOVELNAMES: )|( NOVELNAMEE: )|" +
                                  "( NOVELAUTHORS: )|( NOVELAUTHORE: )|" +
                                  "( CHAPTERURLS: )|( CHAPTERURLE: )|" +
                                  "( CHAPTERNAMES: )|( CHAPTERNAMEE: )|" +
                                  "( CHAPTERCONTENTS: )|( CHAPTERCONTENTE: )"));

        if(qstringList.size() != 17) continue;

        QRule r;
        r.hostName             = qstringList.at(1);
        r.hostUrl              = QUrl(qstringList.at(2));
        r.hostSearchS          = qstringList.at(3);
        r.hostSearchE          = qstringList.at(4);
        r.novelUrlS            = qstringList.at(5);
        r.novelUrlE            = qstringList.at(6);
        r.novelNameS           = qstringList.at(7);
        r.novelNameE           = qstringList.at(8);
        r.novelAuthorS         = qstringList.at(9);
        r.novelAuthorE         = qstringList.at(10);
        r.chapterUrlS          = qstringList.at(11);
        r.chapterUrlE          = qstringList.at(12);
        r.chapterNameS         = qstringList.at(13);
        r.chapterNameE         = qstringList.at(14);
        r.chapterContentS      = qstringList.at(15);
        r.chapterContentE      = qstringList.at(16);
        ruleList.push_back(r);
    }
    in.flush();
    file.close();
}

void QNovelReader::writeRule()
{
    QFile file(ruleFileName);
    if(!file.open(QIODevice::WriteOnly))
    {
//        QMessageBox::information(NULL,
//                                 tr("提示"),
//                                 tr("规则文件写入失败，请检查文件是否只读！"),
//                                 QMessageBox::Yes);
        return;
    }
    QTextStream out(&file);
    for(int i = 0; i < ruleList.size(); i++)
    {
        QRule r = ruleList.at(i);
        out << "HOSTNAME: "         << r.hostName
            << " HOSTURL: "         << r.hostUrl.url()
            << " HOSTSEARCHURLS: "  << r.hostSearchS
            << " HOSTSEARCHURLE: "  << r.hostSearchE
            << " NVOELURLS: "       << r.novelUrlS
            << " NOVELURLE: "       << r.novelUrlE
            << " NOVELNAMES: "      << r.novelNameS
            << " NOVELNAMEE: "      << r.novelNameE
            << " NOVELAUTHORS: "    << r.novelAuthorS
            << " NOVELAUTHORE: "    << r.novelAuthorE
            << " CHAPTERURLS: "     << r.chapterUrlS
            << " CHAPTERURLE: "     << r.chapterUrlE
            << " CHAPTERNAMES: "    << r.chapterNameS
            << " CHAPTERNAMEE: "    << r.chapterNameE
            << " CHAPTERCONTENT: "  << r.chapterContentS
            << " CHAPTERCONTENT: "  << r.chapterContentE
            << endl;
    }
    out.flush();
    file.close();
}

void QNovelReader::readReadHistory()
{
    readHistoryList.clear();
    QFile file(readHistoryFileName);
    if(!file.open(QIODevice::ReadOnly))
    {
//        QMessageBox::information(NULL, tr("提示"), tr("阅读历史文件打开失败！"), QMessageBox::Yes);
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QStringList qstringList =
                        in.readLine().
                            split(QRegExp(QString() +
                                  "(TIME: )|( NOVELURL: )|( NOVELNAME: )|( NOVELAUTHOR: )|( CHAPTERID: )|( CHAPTERNAME: )"));

        if(qstringList.size() != 7) continue;

        QReadHistory r;
        r.readTime      = qstringList.at(1);
        r.novelUrl      = QUrl(qstringList.at(2));
        r.novelName     = qstringList.at(3);
        r.novelAuthor   = qstringList.at(4);
        r.chapterId     = QString(qstringList.at(5)).toInt();
        r.chapterName   = qstringList.at(6);
        readHistoryList.push_back(r);
    }
    in.flush();
    file.close();
}

void QNovelReader::writeReadHistory()
{
    QFile file(readHistoryFileName);
    if(!file.open(QIODevice::WriteOnly))
    {
//        QMessageBox::information(NULL, tr("提示"), tr("阅读历史写入失败，请检测文件是否只读！"), QMessageBox::Yes);
        return;
    }
    QTextStream out(&file);
    for(int i = 0; i < readHistoryList.size(); i++)
    {
        QReadHistory r = readHistoryList.at(i);
        out << "TIME: "         << r.readTime
            << " NOVELURL: "    << r.novelUrl.url()
            << " NOVELNAME: "   << r.novelName
            << " NOVELAUTHOR: " << r.novelAuthor
            << " CHAPTERID: "   << r.chapterId
            << " CHAPTERNAME: " << r.chapterName
            << endl;
    }
    out.flush();
    file.close();
}

void QNovelReader::updateReadHistory()
{
    int index;
    if((index = readHistoryIsContainsNovel(novelList.at(novelId))) != -1)
    {
        if(readHistoryList.at(index).chapterId < chapterId)
        {
            readHistoryList[index].chapterId = chapterId;
            readHistoryList[index].chapterName = novelList.at(novelId).chapters.at(chapterId).name;
            readHistoryList[index].readTime = currentTime("yyyy-MM-dd hh:mm:ss ddd");
            QReadHistory temp = readHistoryList.at(index);
            readHistoryList.removeAt(index);
            readHistoryList.push_front(temp);
            writeReadHistory();
        }
    }
    else
    {
        QReadHistory temp(currentTime("yyyy-MM-dd hh:mm:ss ddd"),
                          novelList.at(novelId).name,
                          novelList.at(novelId).author,
                          QUrl(novelList.at(novelId).url),
                          chapterId,
                          novelList.at(novelId).chapters.at(chapterId).name
                          );
        readHistoryList.push_front(temp);
        writeReadHistory();
    }
}

void QNovelReader::updateListViewReadHistory()
{
    QStringList qstringList;
    for(int i = 0; i < readHistoryList.size(); i++)
    {
        qstringList.push_back(readHistoryList.at(i).novelName + "  " +readHistoryList.at(i).chapterName);
    }
    ui->listView_read_history->setModel(new QStringListModel(qstringList));
}

int QNovelReader::readHistoryIsContainsNovel(const QNovel &novel)
{
    for(int i = 0; i < readHistoryList.size(); i++)
    {
        if(novel.url == readHistoryList.at(i).novelUrl.url())
            return i;
    }
    return -1;
}

void QNovelReader::readSearchHistory()
{
    searchHistoryList.clear();
    QFile file(searchHistoryFileName);
    if(!file.open(QIODevice::ReadOnly))
    {
//        QMessageBox::information(NULL, tr("提示"), tr("搜索历史文件打开失败！"), QMessageBox::Yes);
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QStringList qstringList =
                        in.readLine().
                            split(QRegExp(QString() +
                                  "(TIME: )|( KEYWARD: )|( HOST: )"));

        if(qstringList.size() != 4) continue;

        QSearchHistory s;
        s.searchTime    = qstringList.at(1);
        s.keyWard       = qstringList.at(2);
        s.host          = QUrl(qstringList.at(3));
        searchHistoryList.push_back(s);
    }
    in.flush();
    file.close();
}

void QNovelReader::writeSearchHistory()
{
//    if(ui->lineEdit_search->text() == "") return;
    QFile file(searchHistoryFileName);
    if(!file.open(QIODevice::WriteOnly))
    {
//        QMessageBox::information(NULL, tr("提示"), tr("搜索历史文件写入失败，请检查文件是否只读！"), QMessageBox::Yes);
        return;
    }
    QTextStream out(&file);
    for(int i = 0; i < searchHistoryList.size(); i++)
    {
        out << "TIME: " << searchHistoryList.at(i).searchTime
            << " KEYWARD: " << searchHistoryList.at(i).keyWard
            << " HOST: " << searchHistoryList.at(i).host.url()
            << endl;
    }

    out.flush();
    file.close();
}

void QNovelReader::updateSearchHistory()
{
    if(ui->lineEdit_search->text() == "") return;
    searchHistoryList.push_back(QSearchHistory(currentTime("yyyy-MM-dd hh:mm:ss ddd"),
                                               ui->lineEdit_search->text(),
                                               ruleList.at(hostId).hostUrl));
}

void QNovelReader::updateListViewSearchHistory()
{
    QStringList qstringList;
    for(int i = 0; i < searchHistoryList.size(); i++)
    {
        qstringList.push_back(searchHistoryList.at(i).keyWard);
    }
    ui->listView_search_history->setModel(new QStringListModel(qstringList));
}

QString QNovelReader::currentTime(const QString &format)
{
    return QDateTime::currentDateTime().toString(format);
}

int QNovelReader::isContainsHost(const QString &host)
{
    for(int i = 0; i < ruleList.size(); i++)
        if(("http://" + ruleList.at(i).hostUrl.host()) == host)
            return i;
    return -1;
}

QByteArray QNovelReader::readFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        return "";
    }
    QByteArray c = file.readAll();
    file.close();
    return c;
}

bool QNovelReader::writeFile(const QString &filename, const QByteArray &filecontent)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    file.write(filecontent);
    file.close();
    return true;
}

void QNovelReader::getNextFiveChapters()
{
    for(int i = 1; i < 6; i++)
    {
        if((i + chapterId) >= novelList.at(novelId).chapters.size()) break;
        if(novelList.at(novelId).chapters.at(chapterId + i).content == "") continue;
        QString url = novelList.at(novelId).chapters.at(chapterId+i).url;
        QThreadGetChapter *thread = new QThreadGetChapter(new QUrl(url), "GBK", ruleList.at(hostId));
        thread->start();
        connect(thread, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
        connect(thread, SIGNAL(onFinished(QString, int)), this, SLOT(updateChapterContent(QString, int)));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    }
}
