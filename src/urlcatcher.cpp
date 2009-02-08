/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  shows all URLs found by the client
  begin:     Die Mai 27 2003
  copyright: (C) 2003 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/

#include "urlcatcher.h"
#include "channel.h"
#include "server.h"
#include "application.h" ////// header renamed
#include "viewcontainer.h"

#include <qpushbutton.h>
#include <qregexp.h>
#include <qclipboard.h>
#include <qlayout.h>

#include <klocale.h>
#include <kdebug.h>
#include <k3listview.h>
#include <krun.h>
#include <kfiledialog.h>
#include <kprocess.h>
#include <kdeversion.h>
#include <kshell.h>
#include <k3listviewsearchline.h>
#include <kvbox.h>


UrlCatcher::UrlCatcher(QWidget* parent) : ChatWindow(parent)
{
    setName(i18n("URL Catcher"));
    setType(ChatWindow::UrlCatcher);

    urlListView=new K3ListView(this);
    urlListView->setObjectName("url_list_view");
    urlListView->addColumn(i18n("Nick"));
    urlListView->addColumn(i18n("URL"));
    urlListView->setFullWidth(true);
    urlListView->setAllColumnsShowFocus(true);
    QString urlListViewWT = i18n(
        "List of Uniform Resource Locators mentioned in any of the Konversation windows "
        "during this session.");
    urlListView->setWhatsThis(urlListViewWT);

    searchWidget = new K3ListViewSearchLineWidget(urlListView, this);
    searchWidget->setObjectName("search_line");
    searchWidget->setEnabled(false);

    KHBox* buttonBox=new KHBox(this);
    buttonBox->setSpacing(spacing());

    openUrlButton = new QPushButton(i18n("&Open URL"), buttonBox);
    openUrlButton->setObjectName("open_url_button");
    QString openUrlButtonWT = i18n(
        "<p>Select a <b>URL</b> above, then click this button to launch the "
        "application associated with the mimetype of the URL.</p>"
        "<p>In the <b>Settings</b>, under <b>Behavior</b> | <b>General</b>, "
        "you can specify a custom web browser for web URLs.</p>");
    openUrlButton->setWhatsThis(openUrlButtonWT);
    copyUrlButton = new QPushButton(i18n("&Copy URL"), buttonBox);
    copyUrlButton->setObjectName("copy_url_button");
    QString copyUrlButtonWT = i18n(
        "Select a <b>URL</b> above, then click this button to copy the URL to the clipboard.");
    copyUrlButton->setWhatsThis(copyUrlButtonWT);
    deleteUrlButton = new QPushButton(i18n("&Delete URL"), buttonBox);
    deleteUrlButton->setObjectName("delete_url_button");
    QString deleteUrlButtonWT = i18n(
        "Select a <b>URL</b> above, then click this button to delete the URL from the list.");
    deleteUrlButton->setWhatsThis(deleteUrlButtonWT);
    saveListButton = new QPushButton(i18n("Sa&ve List..."), buttonBox);
    saveListButton->setObjectName("save_list_button");
    QString saveListButtonWT = i18n(
        "Click to save the entire list to a file.");
    saveListButton->setWhatsThis(saveListButtonWT);
    clearListButton = new QPushButton(i18n("C&lear List"), buttonBox);
    clearListButton->setObjectName("clear_list_button");
    QString clearListButtonWT = i18n(
        "Click to erase the entire list.");
    clearListButton->setWhatsThis(clearListButtonWT);

    connect(urlListView,SIGNAL (executed(Q3ListViewItem*)),this,SLOT (openUrl(Q3ListViewItem*)) );
    connect(urlListView,SIGNAL (selectionChanged()),this,SLOT (urlSelected()) );

    connect(openUrlButton,SIGNAL (clicked()),this,SLOT (openUrlClicked()) );
    connect(copyUrlButton,SIGNAL (clicked()),this,SLOT (copyUrlClicked()) );
    connect(deleteUrlButton,SIGNAL (clicked()),this,SLOT (deleteUrlClicked()) );
    connect(saveListButton,SIGNAL (clicked()),this,SLOT (saveListClicked()) );
    connect(clearListButton,SIGNAL (clicked()),this,SLOT (clearListClicked()) );

    saveListButton->setEnabled(false);
    clearListButton->setEnabled(false);

    layout()->addWidget(searchWidget);
    layout()->addWidget(urlListView);
    layout()->addWidget(buttonBox);

    urlSelected();
}


UrlCatcher::~UrlCatcher()
{
}

void UrlCatcher::urlSelected()
{
    Q3ListViewItem* item=urlListView->selectedItem();
    if(item)
    {
        openUrlButton->setEnabled(true);
        copyUrlButton->setEnabled(true);
        deleteUrlButton->setEnabled(true);
    }
    else
    {
        openUrlButton->setEnabled(false);
        copyUrlButton->setEnabled(false);
        deleteUrlButton->setEnabled(false);
    }
}

void UrlCatcher::addUrl(const QString& who,const QString& url)
{
    new K3ListViewItem(urlListView,who,url);
    clearListButton->setEnabled(true);
    saveListButton->setEnabled(true);
    searchWidget->setEnabled(true);
}

void UrlCatcher::openUrl(Q3ListViewItem* item)
{
    QString url = item->text(1);
    if (!Preferences::self()->useCustomBrowser() || url.toLower().startsWith("mailto:") )
    {
        new KRun(KUrl(url), KonversationApplication::instance()->getMainWindow());
    }
    else
    {
        QString cmd = Preferences::webBrowserCmd();
        cmd.replace("%u", url);
        KProcess *proc = new KProcess;
        QStringList cmdAndArgs = KShell::splitArgs(cmd);
        *proc << cmdAndArgs;
        //    This code will also work, but starts an extra shell process.
        //    kDebug() "cmd = " << cmd;
        //    *proc << cmd;
        //    proc->setUseShell(true);
        proc->startDetached();
        delete proc;
    }
}

void UrlCatcher::openUrlClicked()
{
    Q3ListViewItem* item=urlListView->selectedItem();
    if(item) openUrl(item);
}

void UrlCatcher::copyUrlClicked()
{
    Q3ListViewItem* item=urlListView->selectedItem();
    if(item)
    {
        QClipboard *cb = qApp->clipboard();
        cb->setText(item->text(1),QClipboard::Selection);
        cb->setText(item->text(1),QClipboard::Clipboard);
    }
}

void UrlCatcher::deleteUrlClicked()
{
    Q3ListViewItem* item=urlListView->selectedItem();
    if(item)
    {
        emit deleteUrl(item->text(0),item->text(1));
        delete item;
        // select next item
        item=urlListView->currentItem();
        if(item) urlListView->setSelected(item,true);
        else
        {
            saveListButton->setEnabled(false);
            clearListButton->setEnabled(false);
            searchWidget->setEnabled(false);
        }
    }
}

void UrlCatcher::clearListClicked()
{
    urlListView->clear();
    saveListButton->setEnabled(false);
    clearListButton->setEnabled(false);
    urlSelected();
    emit clearUrlList();
}

void UrlCatcher::saveListClicked()
{
    // Ask user for file name
    QString fileName=KFileDialog::getSaveFileName(
        QString(),
        QString(),
        this,
        i18n("Save URL List"));

    if(!fileName.isEmpty())
    {
        // now save the list to disk
        QFile listFile(fileName);
        listFile.open(QIODevice::WriteOnly);
        // wrap the file into a stream
        QTextStream stream(&listFile);
        Q3ListViewItem* item=urlListView->itemAtIndex(0);
        while(item)
        {
            stream << item->text(0) << ": " << item->text(1) << endl;
            item=item->itemBelow();
        }                                         // while
    }
}

void UrlCatcher::childAdjustFocus()
{
}

// #include "./urlcatcher.moc"
