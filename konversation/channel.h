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

#include "chatwindow.h"

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
    QString completeNick(const QString& pattern, bool& complete, QStringList& found);
  protected:
    virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2);
};

class Channel : public ChatWindow
{
  Q_OBJECT

  public:
    Channel(QWidget* parent);
    ~Channel();

    void setName(const QString& newName);
    void setKey(const QString& newKey);
    QString getKey();

    void joinNickname(const QString& nickname,const QString& hostname);
    void renameNick(const QString& nickname,const QString& newName);
    void addNickname(const QString& nickname,const QString& hostmask,
                     bool admin,bool owner,bool op,bool halfop,bool voice);
    void removeNick(const QString& nickname, const QString& reason, bool quit);
    void kickNick(const QString& nickname, const QString& kicker, const QString& reason);
    Nick *getNickByName(const QString& lookname);
    QPtrList<Nick> getNickList();
    void addPendingNickList(const QStringList& nickList);

    void setPendingNicks(bool state);
    bool getPendingNicks();

    void adjustNicks(int value);
    void adjustOps(int value);
    void updateNicksOps();

    void setTopic(const QString& topic);
    void setTopic(const QString& nickname,const QString& topic); // Overloaded
    void setTopicAuthor(const QString& author);
    void updateMode(const QString &nick,char mode,bool plus, const QString &parameter);
    void updateModeWidgets(char mode, bool plus, const QString &parameter);
    void updateQuickButtons(QStringList newButtonList);
    void updateFonts();
    void updateStyleSheet();

    virtual QString getTextInLine();
    virtual void closeYourself();
    virtual bool frontView();
    virtual bool searchView();
    
    bool allowNotifications() { return m_allowNotifications; }

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

  protected:
    QStringList getSelectedNicksList();
    void showEvent(QShowEvent* event);

    // use with caution! does not check for duplicates
    void fastAddNickname(const QString& nickname,const QString& hostmask,
                         bool admin,bool owner,bool op,bool halfop,bool voice);

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
    //QPushButton* nicknameButton;
    QComboBox* nicknameCombobox;
    QLabel* awayLabel;
    QGrid* buttonsGrid;
    IRCInput* channelInput;

    NickChangeDialog* nickChangeDialog;
    NickList nicknameList;
    QStringList selectedNicksList;
    QPtrList<QuickButton> buttonList;
    QTimer userhostTimer;
    
    bool m_allowNotifications;
};

#endif
