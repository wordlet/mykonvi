/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  query.h  -  description
  begin:     Mon Jan 28 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/

#ifndef QUERY_H
#define QUERY_H

#include <qstring.h>

#include "chatwindow.h"

/*
  @author Dario Abatianni
*/

/* TODO: Idle counter to close query after XXX minutes of inactivity */
/* TODO: Use /USERHOST to check if queries are still valid */

class QLineEdit;
class QCheckBox;
class QLabel;

class IRCInput;

class Query : public ChatWindow
{
  Q_OBJECT

  public:
#ifdef USE_MDI
    Query(QString caption);
#else
    Query(QWidget* parent);
#endif
    ~Query();

    void setName(const QString& newName);
    void setHostmask(const QString& newHostmask);
    void updateFonts();
    virtual QString getTextInLine();
    virtual void closeYourself();
    virtual bool frontView();
    virtual bool searchView();

    virtual void setChannelEncoding(const QString& encoding);
    virtual QString getChannelEncoding();

  signals:
    void newText(QWidget* query,const QString& highlightColor,bool important);
    void sendFile(const QString& recipient);

  public slots:
    void sendQueryText(const QString& text);
    void appendInputText(const QString& s);
    virtual void indicateAway(bool show);

  protected slots:
    void queryTextEntered();
    void sendFileMenu();
    void newTextInView(const QString& highlightColor,bool important);
    // connected to IRCInput::textPasted() - used to handle large/multiline pastes
    void textPasted(QString text);
    void popup(int id);
#ifdef USE_MDI
    void serverQuit(const QString& reason);
#endif
  protected:
    void showEvent(QShowEvent* event);
    /** Called from ChatWindow adjustFocus */
    virtual void childAdjustFocus();
    
    bool awayChanged;
    bool awayState;

#ifdef USE_MDI
    virtual void closeYourself(ChatWindow* view);
#endif

    QString queryName;
    QString hostmask;
    QString buffer;

    QLineEdit* queryHostmask;
    QLabel* awayLabel;
    IRCInput* queryInput;
};

#endif
