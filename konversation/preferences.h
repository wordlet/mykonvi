/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  preferences.h  -  Class for application wide preferences
  begin:     Tue Feb 5 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com

  $Id$
*/

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <qobject.h>
#include <qlist.h>
#include <qsize.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qfont.h>

#include "serverentry.h"
#include "ignore.h"
#include "highlight.h"
#include "identity.h"

/*
  @author Dario Abatianni
*/

/*
Options still to be GUIfied:

Operator LEDs (int)
OperatorColor (int)
VoiceColor (int)
NoRightsColor (int)
*/

class Preferences : public QObject
{
  Q_OBJECT

  public:
    Preferences();
    ~Preferences();

    int serverWindowToolBarPos;
    int serverWindowToolBarStatus;
    int serverWindowToolBarIconText;
    int serverWindowToolBarIconSize;
    bool serverWindowStatusBarStatus;
    bool serverWindowMenuBarStatus;

    int addServer(const QString& serverString);
    void removeServer(int id);
    QString getServerByIndex(unsigned int);
    QString getServerById(int);
    ServerEntry* getServerEntryById(int id);
    int getServerIdByIndex(unsigned int);
    QValueList<int> getAutoConnectServerIDs();

    bool getAutoReconnect();
    void setAutoReconnect(bool state);
    bool getAutoRejoin();
    void setAutoRejoin(bool state);
    bool getBeep();
    void setBeep(bool state);

    void clearServerList();
    void changeServerProperty(int id,int property,const QString& value);
    void updateServer(int id,const QString& newDefinition);

    void setLog(bool state);
    bool getLog();
    void setLowerLog(bool state);
    bool getLowerLog();
    void setLogFollowsNick(bool state);
    bool getLogFollowsNick();
    void setLogPath(QString path);
    QString getLogPath();

    void setDccAddPartner(bool state);
    bool getDccAddPartner();
    void setDccCreateFolder(bool state);
    bool getDccCreateFolder();
    void setDccAutoGet(bool state);
    bool getDccAutoGet();
    void setDccBufferSize(unsigned long size);
    unsigned long getDccBufferSize();
    void setDccPath(QString path);
    QString getDccPath();
    void setDccRollback(unsigned long bytes);
    unsigned long getDccRollback();

    void setBlinkingTabs(bool blink);
    bool getBlinkingTabs();
    void setBringToFront(bool state);
    bool getBringToFront();

    // Geometry functions
    QSize getServerWindowSize();
    QSize& getHilightSize();
    QSize& getButtonsSize();
    QSize& getIgnoreSize();
    QSize& getNotifySize();
    QSize& getNicksOnlineSize();
    QSize& getNicknameSize();
    QSize& getColorConfigurationSize();
    void setServerWindowSize(QSize newSize);
    void setHilightSize(QSize newSize);
    void setButtonsSize(QSize newSize);
    void setIgnoreSize(QSize newSize);
    void setNotifySize(QSize newSize);
    void setNicksOnlineSize(QSize newSize);
    void setNicknameSize(QSize newSize);
    void setColorConfigurationSize(QSize newSize);

    int getNotifyDelay();
    void setNotifyDelay(int delay);
    bool getUseNotify();
    void setUseNotify(bool use);
    QStringList getNotifyList();
    QString getNotifyString();
    void setNotifyList(QStringList newList);

    QPtrList<Highlight> getHilightList();
    void setHilightList(QPtrList<Highlight> newList);
    void addHilight(QString newHilight,QColor color);

    void setHilightNick(bool state);      // shall we hilight the current nick?
    bool getHilightNick();

    void setHilightNickColor(QString color);
    QColor getHilightNickColor();

    void setHilightOwnLines(bool state);  // shall we hilight all our own lines?
    bool getHilightOwnLines();

    void setHilightOwnLinesColor(QString color);
    QColor getHilightOwnLinesColor();

    QStringList getButtonList();
    void setButtonList(QStringList newList);

    void addIgnore(QString newIgnore);
    void clearIgnoreList();
    QPtrList<Ignore> getIgnoreList();
    void setIgnoreList(QPtrList<Ignore> newList);

    void addIdentity(Identity* identity);
    void clearIdentityList();
    QPtrList<Identity> getIdentityList();
    const Identity& getIdentityByName(const QString& name);

    QString getIdent();
    void setIdent(QString ident);

    QString getRealName();
    void setRealName(QString name);

    QString getPartReason();
    void setPartReason(QString newReason);

    QString getKickReason();
    void setKickReason(QString newReason);

    void setShowAwayMessage(bool state);
    bool getShowAwayMessage();
    QString getAwayMessage();
    void setAwayMessage(QString newMessage);
    QString getUnAwayMessage();
    void setUnAwayMessage(QString newMessage);

    QString getNickname(int index);
    QStringList getNicknameList();
    void setNickname(int index,QString newName);
    void setNicknameList(QStringList newList);

    QString getChannelMessageColor();
    QString getQueryMessageColor();
    QString getServerMessageColor();
    QString getActionMessageColor();
    QString getBacklogMessageColor();
    QString getLinkMessageColor();
    QString getCommandMessageColor();
    QString getTimeColor();
    QString getTextViewBackground();

    void setChannelMessageColor(QString color);
    void setQueryMessageColor(QString color);
    void setServerMessageColor(QString color);
    void setActionMessageColor(QString color);
    void setBacklogMessageColor(QString color);
    void setLinkMessageColor(QString color);
    void setCommandMessageColor(QString color);
    void setTimeColor(QString color);
    void setTextViewBackground(QString color);

    void setNickCompleteSuffixStart(QString suffix);
    void setNickCompleteSuffixMiddle(QString suffix);
    QString getNickCompleteSuffixStart();
    QString getNickCompleteSuffixMiddle();

    int getOpLedColor();
    int getVoiceLedColor();
    int getNoRightsLedColor();
    void setOpLedColor(int color);
    void setVoiceLedColor(int color);
    void setNoRightsLedColor(int color);

    bool getFixedMOTD();
    void setFixedMOTD(bool fixed);

    void setCommandChar(QString newCommandChar);
    QString getCommandChar();

    void setCodec(QString newCodec);
    QString getCodec();

    QFont getTextFont();
    QFont getListFont();
    void setTextFont(QFont newFont);
    void setListFont(QFont newFont);
    void setTextFontRaw(QString rawFont);
    void setListFontRaw(QString newFont);

    void setTimestamping(bool state);
    bool getTimestamping();
    void setTimestampFormat(const QString& newFormat);
    const QString& getTimestampFormat();

    void setShowQuickButtons(bool state);
    bool getShowQuickButtons();

    void setShowModeButtons(bool state);
    bool getShowModeButtons();

    void setShowServerList(bool state);
    bool getShowServerList();

    void setChannelSplitter(QValueList<int> sizes);
    QValueList<int> getChannelSplitter();

    void setChannelDoubleClickAction(QString action);
    QString getChannelDoubleClickAction();

    void setNotifyDoubleClickAction(QString action);
    QString getNotifyDoubleClickAction();

    void setUseSpacing(bool state);
    bool getUseSpacing();
    void setSpacing(int newSpacing);
    int getSpacing();
    void setMargin(int newMargin);
    int getMargin();

  signals:
    void requestServerConnection(int number);
    void requestSaveOptions();

  protected:
    bool log;
    bool lowerLog;
    bool logFollowsNick;
    QString logPath;

    bool blinkingTabs;      // Do we want the LEDs on the tabs to blink?
    bool bringToFront;      // Do we want to see newly created tabs immediately?

    bool fixedMOTD;
    bool beep;

    bool dccAddPartner;
    bool dccCreateFolder;   // create folders for each DCC partner?
    bool dccAutoGet;
    unsigned long dccBufferSize;
    unsigned long dccRollback;  // Rollback for Resume
    QString dccPath;

    bool autoReconnect;
    bool autoRejoin;

    int notifyDelay;
    int opLedColor;
    int voiceLedColor;
    int noRightsLedColor;
    bool useNotify;

    bool timestamping;
    QString timestampFormat;

    bool showQuickButtons;
    bool showModeButtons;
    bool showServerList;

    QValueList<int> channelSplitter;
    
    QStringList notifyList;
    QString commandChar;

    QString nickCompleteSuffixStart;
    QString nickCompleteSuffixMiddle;

    QString channelDoubleClickAction;
    QString notifyDoubleClickAction;

    // rewrite these to use an array or list of some sort
    QString channelMessageColor;
    QString queryMessageColor;
    QString serverMessageColor;
    QString actionMessageColor;
    QString backlogMessageColor;
    QString linkMessageColor;
    QString commandMessageColor;
    QString timeColor;
    QString textViewBackground;

    // Geometries
    QSize serverWindowSize;
    QSize buttonsSize;
    QSize hilightSize;
    QSize ignoreSize;
    QSize notifySize;
    QSize nicksOnlineSize;
    QSize nicknameSize;
    QSize colorConfigurationSize;

    bool useSpacing;
    int spacing;
    int margin;

    QFont textFont;
    QFont listFont;

    QPtrList<ServerEntry> serverList;
    QPtrList<Highlight> hilightList;

    bool hilightNick;
    bool hilightOwnLines;
    QColor hilightNickColor;
    QColor hilightOwnLinesColor;

    QStringList buttonList;

    QPtrList<Ignore> ignoreList;

    QPtrList<Identity> identityList;

    QString codec;
};

#endif
