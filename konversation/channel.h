/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
    channel.h  -  The class that controls a channel
    begin:     Wed Jan 23 2002
    copyright: (C) 2002 by Dario Abatianni
    email:     eisfuchs@tigress.com
*/

#ifndef CHANNEL_H
#define CHANNEL_H

#include <qtimer.h>
#include <qstring.h>
#include "server.h"
#include "chatwindow.h"
#include "channelnick.h"

/*
  @author Dario Abatianni
*/

class QPushButton;
class QCheckBox;
class QLabel;
class QTimer;
class QListViewItem;
class QHBox;
class QStringList;
class QSplitter;
class QGrid;
class QComboBox;

class KLineEdit;

class Nick;
class NickListView;
class QuickButton;
class ModeButton;
class IRCInput;
class NickChangeDialog;
class TopicComboBox;

class NickList : public QPtrList<Nick>
{
  public:
    QString completeNick(const QString& pattern, bool& complete, QStringList& found, bool skipNonAlfaNum);
  protected:
    virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2);
};

class Channel : public ChatWindow
{
  Q_OBJECT

  public:
#ifdef USE_MDI
    Channel(QString caption);
#else
    Channel(QWidget* parent);
#endif
    ~Channel();

    void setName(const QString& newName);
    void setKey(const QString& newKey);
    QString getKey();

#ifdef USE_NICKINFO
    ChannelNickPtr getChannelNick(const QString &ircnick);
    void joinNickname(ChannelNickPtr channelNick);
    void removeNick(ChannelNickPtr channelNick, const QString &reason, bool quit);
    void kickNick(ChannelNickPtr channelNick, const ChannelNick &kicker, const QString &reason);
    void addNickname(ChannelNickPtr channelNick);
    void nickRenamed(const QString &oldNick, const NickInfo& channelnick);	    
    void addPendingNickList(const ChannelNickList& pendingChannelNickList);
#else
    void joinNickname(const QString& nickname,const QString& hostname);
    void addNickname(const QString& nickname,const QString& hostmask,
                     bool admin,bool owner,bool op,bool halfop,bool voice);
    void removeNick(const QString& nickname, const QString& reason, bool quit);
    void kickNick(const QString& nickname, const QString& kicker, const QString& reason);
    void renameNick(const QString& nickname,const QString& newName);
    void addPendingNickList(const QStringList& nickList);
#endif
    Nick *getNickByName(const QString& lookname);
    QPtrList<Nick> getNickList();

    void setPendingNicks(bool state);
    bool getPendingNicks();

    void adjustNicks(int value);
    void adjustOps(int value);
    void updateNicksOps();

    void setTopic(const QString& topic);
    void setTopic(const QString& nickname,const QString& topic); // Overloaded
    void setTopicAuthor(const QString& author);
#ifdef USE_NICKINFO
    void updateMode(ChannelNickPtr sourceChannelNick, char mode, bool plus, const QString &parameter);
#else   
    void updateMode(const QString &nick,char mode,bool plus, const QString &parameter);
#endif
    void updateModeWidgets(char mode, bool plus, const QString &parameter);
    void updateQuickButtons(QStringList newButtonList);
    void updateFonts();
    void updateStyleSheet();

    virtual QString getTextInLine();
    virtual void closeYourself();
    virtual bool frontView();
    virtual bool searchView();
    bool editAddressee(const QString &uid);
	
    bool allowNotifications() { return m_allowNotifications; }

    QStringList getSelectedNicksList();
  signals:
    void newText(QWidget* channel,const QString& highlightColor, bool important);
    void prefsChanged();
    void sendFile();
  public slots:
    void setNickname(const QString& newNickname);
    void channelTextEntered();
    void sendChannelText(const QString& line);
    void newTextInView(const QString& highlightColor, bool important);
    void adjustFocus();
    void showQuickButtons(bool show);
    void showModeButtons(bool show);
    void appendInputText(const QString& s);
    virtual void indicateAway(bool show);
    void showTopic(bool show);

    void setAllowNotifications(bool allow) { m_allowNotifications = allow; }

  protected slots:
    void purgeNicks();
    void completeNick();
    void endCompleteNick();
    void quickButtonClicked(const QString& definition);
    void modeButtonClicked(int id,bool on);
    void channelLimitChanged();

    void popupCommand(int id);                // Will be connected to NickListView::popupCommand()
    void doubleClickCommand(QListViewItem*);  // Will be connected to NickListView::doubleClicked()
    // Dialogs
    void changeNickname(const QString& newNickname);
    // will be called when the user types a new topic in the topic line
    void requestNewTopic(const QString& newTopic);
    // connected to IRCInput::textPasted() - used to handle large/multiline pastings
    void textPasted(QString text);
    // connected to IRCInput::sendFile()
    void sendFileMenu();
    void autoUserhost();
    void autoUserhostChanged(bool state);
    void nicknameComboboxChanged(int index);

    void closeYourself(ChatWindow* view); // USE_MDI
    void serverQuit(const QString& reason); // USE_MDI
    
    void processPendingNicks();

  protected:
    void showEvent(QShowEvent* event);
#ifdef USE_NICKINFO
    // use with caution! does not check for duplicates
    void fastAddNickname(ChannelNickPtr channelnick);
#else
    // use with caution! does not check for duplicates
    void fastAddNickname(const QString& nickname,const QString& hostmask,
                         bool admin,bool owner,bool op,bool halfop,bool voice);
#endif
    int nicks;
    int ops;

    // are there still nicks to be added by /names reply?
    bool pendingNicks;

    // to take care of redraw problem if hidden
    bool quickButtonsChanged;
    bool quickButtonsState;
    bool modeButtonsChanged;
    bool modeButtonsState;
    bool splitterChanged;
    bool awayChanged;
    bool awayState;

    bool topicAuthorUnknown;

    unsigned int completionPosition;

    QSplitter* splitter;
    QString topic; // Caches current topic
    TopicComboBox* topicLine;
    QLabel* topicLabel;
    QStringList topicHistory;
    QHBox* modeBox;

    QString key;

    ModeButton* modeT;
    ModeButton* modeN;
    ModeButton* modeS;
    ModeButton* modeI;
    ModeButton* modeP;
    ModeButton* modeM;
    ModeButton* modeK;
    ModeButton* modeL;

    KLineEdit* limit;

    QLabel* nicksOps;
    NickListView* nicknameListView;
    QString abgCache;                   // caches the alternate background color
    QHBox* commandLineBox;
    QComboBox* nicknameCombobox;
    QString oldNick;
    QLabel* awayLabel;
    QGrid* buttonsGrid;
    IRCInput* channelInput;

    NickChangeDialog* nickChangeDialog;
    NickList nicknameList;
    QStringList selectedNicksList;
    QPtrList<QuickButton> buttonList;
    QTimer userhostTimer;

    bool m_allowNotifications;
    
    ChannelNickList::iterator m_listIterator;
    ChannelNickList m_pendingChannelNickList;
    int m_opsToAdd;
    
    QTimer* m_processingTimer;
};
#endif
