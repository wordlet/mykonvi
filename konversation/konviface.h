#ifndef KONV_IFACE_H
#define KONV_IFACE_H "$Id$"

#include <qobject.h>
#include <qstringlist.h>

#include <dcopobject.h>

#include "ignore.h"

class KonvIface : virtual public DCOPObject
{
    K_DCOP

  k_dcop:
    virtual void raw(const QString& server,const QString& command) = 0;
    virtual void say(const QString& server,const QString& target,const QString& command) = 0;
    virtual void info(const QString& string) = 0;
    virtual void debug(const QString& string) = 0;
    virtual void error(const QString& string) = 0;
    virtual void registerEventHook (const QString &type, const QString &criteria, const QString &app, const QString &object, const QString &signal) = 0;
    virtual void unregisterEventHook (int id) = 0;
    virtual bool isIgnore (int serverid, const QString &hostmask, int type) = 0;
    virtual QString getNickname (int serverid) = 0;
};

class KonvIdentityIface : virtual public DCOPObject
{
    K_DCOP
  k_dcop:

    virtual void setrealName(const QString &identity, const QString& name) = 0;
    virtual QString getrealName(const QString &identity) = 0;
    virtual void setIdent(const QString &identity, const QString& ident) = 0;
    virtual QString getIdent(const QString &identity) = 0;

    virtual void setNickname(const QString &identity, int index,const QString& nick) = 0;
    virtual QString getNickname(const QString &identity, int index) = 0;

    virtual void setBot(const QString &identity, const QString& bot) = 0;
    virtual QString getBot(const QString &identity) = 0;
    virtual void setPassword(const QString &identity, const QString& password) = 0;
    virtual QString getPassword(const QString &identity) = 0;

    virtual void setNicknameList(const QString &identity, const QStringList& newList) = 0;
    virtual QStringList getNicknameList(const QString &identity) = 0;

    virtual void setPartReason(const QString &identity, const QString& reason) = 0;
    virtual QString getPartReason(const QString &identity) = 0;
    virtual void setKickReason(const QString &identity, const QString& reason) = 0;
    virtual QString getKickReason(const QString &identity) = 0;

    virtual void setShowAwayMessage(const QString &identity, bool state) = 0;
    virtual bool getShowAwayMessage(const QString &identity) = 0;

    virtual void setAwayMessage(const QString &identity, const QString& message) = 0;
    virtual QString getAwayMessage(const QString &identity) = 0;
    virtual void setReturnMessage(const QString &identity, const QString& message) = 0;
    virtual QString getReturnMessage(const QString &identity) = 0;
};

class KonvPreferencesIface : virtual public DCOPObject
{
    K_DCOP
  k_dcop:

    virtual bool getAutoReconnect() = 0;
    virtual void setAutoReconnect(bool state) = 0;
    virtual bool getAutoRejoin() = 0;
    virtual void setAutoRejoin(bool state) = 0;
    virtual bool getBeep() = 0;
    virtual void setBeep(bool state) = 0;
    virtual void clearServerList() = 0;
    virtual void changeServerProperty(int id,int property,const QString& value) = 0;
    virtual void updateServer(int id,const QString& newDefinition) = 0;
    virtual void setLog(bool state) = 0;
    virtual bool getLog() = 0;
    virtual void setLowerLog(bool state) = 0;
    virtual bool getLowerLog() = 0;
    virtual void setLogFollowsNick(bool state) = 0;
    virtual bool getLogFollowsNick() = 0;
    virtual void setLogPath(QString path) = 0;
    virtual QString getLogPath() = 0;
    virtual void setDccAddPartner(bool state) = 0;
    virtual bool getDccAddPartner() = 0;
    virtual void setDccCreateFolder(bool state) = 0;
    virtual bool getDccCreateFolder() = 0;
    virtual void setDccAutoGet(bool state) = 0;
    virtual bool getDccAutoGet() = 0;
    virtual void setDccBufferSize(unsigned long size) = 0;
    virtual unsigned long getDccBufferSize() = 0;
    virtual void setDccPath(QString path) = 0;
    virtual QString getDccPath() = 0;
    virtual void setDccRollback(unsigned long bytes) = 0;
    virtual unsigned long getDccRollback() = 0;
    virtual void setBlinkingTabs(bool blink) = 0;
    virtual bool getBlinkingTabs() = 0;
    virtual void setBringToFront(bool state) = 0;
    virtual bool getBringToFront() = 0;
    virtual void setCloseButtonsOnTabs(bool state) = 0;
    virtual bool getCloseButtonsOnTabs() = 0;
    virtual int getNotifyDelay() = 0;
    virtual void setNotifyDelay(int delay) = 0;
    virtual bool getUseNotify() = 0;
    virtual void setUseNotify(bool use) = 0;
    virtual QStringList getNotifyList() = 0;
    virtual QString getNotifyString() = 0;
    virtual void setNotifyList(QStringList newList) = 0;
    virtual bool addNotify(QString newPattern) = 0;
    virtual bool removeNotify(QString pattern) = 0;
    virtual void addIgnore(QString newIgnore) = 0;
    virtual void clearIgnoreList() = 0;
    //QPtrList<Ignore> getIgnoreList() = 0;
    virtual void setIgnoreList(QPtrList<Ignore> newList) = 0;
    virtual void setChannelMessageColor(QString color) = 0;
    virtual void setQueryMessageColor(QString color) = 0;
    virtual void setServerMessageColor(QString color) = 0;
    virtual void setActionMessageColor(QString color) = 0;
    virtual void setBacklogMessageColor(QString color) = 0;
    virtual void setLinkMessageColor(QString color) = 0;
    virtual void setCommandMessageColor(QString color) = 0;
    virtual void setTimeColor(QString color) = 0;
    virtual void setTextViewBackground(QString color) = 0;
    virtual void setNickCompleteSuffixStart(QString suffix) = 0;
    virtual void setNickCompleteSuffixMiddle(QString suffix) = 0;
    virtual QString getNickCompleteSuffixStart() = 0;
    virtual QString getNickCompleteSuffixMiddle() = 0;

};

#endif
