/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  konversationmainwindow.cpp  -  The main window where all other views go
  begin:     Don Apr 17 2003
  copyright: (C) 2003 by Dario Abatianni, Peter Simonsson
  email:     eisfuchs@tigress.com, psn@linux.se
*/

#include "konversationmainwindow.h"

#include <kaccel.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kmenubar.h>
#include <kkeydialog.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <kpopupmenu.h>

#include <kabc/addressbook.h>
#include <kabc/errorhandler.h>
#include "linkaddressbook/addressbook.h"

#ifdef USE_MDI
#include <ktabwidget.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#endif


#ifndef KDE_MAKE_VERSION
#define KDE_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))
#endif

#ifndef KDE_IS_VERSION
#define KDE_IS_VERSION(a,b,c) ( KDE_VERSION >= KDE_MAKE_VERSION(a,b,c) )
#endif

#if KDE_IS_VERSION(3,1,1)
#define USE_KNOTIFY
#include <knotifydialog.h>
#endif

#include "ledtabwidget.h"
#include "chatwindow.h"
#include "konversationapplication.h"
#include "ircview.h"
#include "server.h"
#include "statuspanel.h"
#include "channel.h"
#include "query.h"
#include "rawlog.h"
#include "channellistpanel.h"
#include "dccpanel.h"
#include "dcctransferhandler.h"
#include "nicksonline.h"
#include "konsolepanel.h"
#include "urlcatcher.h"
#include "irccolorchooser.h"
#include "trayicon.h"
#include "tabaction.h"
#include "dccchat.h"
#include "serverlistdialog.h"
#include "insertchardialog.h"

#ifdef USE_MDI
KonversationMainWindow::KonversationMainWindow() : KMdiMainFrm(0,"mdi_main_form")
#else
KonversationMainWindow::KonversationMainWindow() : KMainWindow()
#endif
{
  // Init variables before anything else can happen
  frontView=0;
  previousFrontView=0;
  searchView=0;
  frontServer=0;
  urlCatcherPanel=0;
  dccPanel=0;
  dccPanelOpen=false;
  m_closeApp = false;

  dccTransferHandler=new DccTransferHandler(this);

  nicksOnlinePanel=0;

#ifdef USE_MDI
  switchToTabPageMode();
#else
  viewContainer=new LedTabWidget(this,"main_window_tab_widget");
  setCentralWidget(viewContainer);
  updateTabPlacement();
#endif

  KStdAction::quit(this,SLOT(quitProgram()),actionCollection()); // file_quit

#if KDE_IS_VERSION(3, 1, 0)
  setStandardToolBarMenuEnabled(true);
#else
  showToolBarAction=KStdAction::showToolbar(this,SLOT(showToolbar()),actionCollection()); // options_show_toolbar
#endif
#if KDE_IS_VERSION(3, 1, 90)
  createStandardStatusBarAction();
#else
  showStatusBarAction=KStdAction::showStatusbar(this,SLOT(showStatusbar()),actionCollection()); // options_show_statusbar
#endif
  showMenuBarAction=KStdAction::showMenubar(this,SLOT(showMenubar()),actionCollection()); // options_show_menubar
  KStdAction::configureToolbars(this, SLOT(openToolbars()), actionCollection());
#ifdef USE_KNOTIFY
  KAction *configureNotificationsAction = KStdAction::configureNotifications(this,SLOT(openNotifications()), actionCollection());  // options_configure_notifications
#endif

  KStdAction::keyBindings(this,SLOT(openKeyBindings()),actionCollection()); // options_configure_key_binding
  KAction *preferencesAction = KStdAction::preferences(this,SLOT(openPreferences()),actionCollection()); // options_configure

  new KAction(i18n("&Server List"), 0, 0, this, SLOT(openServerList()), actionCollection(), "open_server_list");
  new KAction(i18n("Quick &Connect"), "connect_creating", 0, this, SLOT(openQuickConnectDialog()), actionCollection(), "quick_connect_dialog");

  new KAction(i18n("&Watched Nicks Online"), 0, 0, this, SLOT(openNicksOnlinePanel()), actionCollection(), "open_nicksonline_window");
  new KAction(i18n("&Open Logfile"), 0, 0, this, SLOT(openLogfile()), actionCollection(), "open_logfile");

  new KAction(i18n("&Channel List"), 0, 0, this, SLOT(openChannelList()), actionCollection(), "open_channel_list");
  new KAction(i18n("&URL Catcher"), 0, 0, this, SLOT(addUrlCatcher()), actionCollection(), "open_url_catcher");

  new KAction(i18n("&New Konsole"), "openterm", 0, this, SLOT(addKonsolePanel()), actionCollection(), "open_konsole");

  // Actions to navigate through the different pages
  new KAction(i18n("&Next Tab"), "next",KShortcut("Alt+Right"),this,SLOT(nextTab()),actionCollection(),"next_tab");
  new KAction(i18n("&Previous Tab"), "previous",KShortcut("Alt+Left"),
    this,SLOT(previousTab()),actionCollection(),"previous_tab");
  new KAction(i18n("Close &Tab"),"tab_remove",KShortcut("Ctrl+w"),this,SLOT(closeTab()),actionCollection(),"close_tab");
  new TabAction(i18n("Go to Tab Number %1").arg( 1),0,KShortcut("Alt+1"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_1");
  new TabAction(i18n("Go to Tab Number %1").arg( 2),1,KShortcut("Alt+2"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_2");
  new TabAction(i18n("Go to Tab Number %1").arg( 3),2,KShortcut("Alt+3"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_3");
  new TabAction(i18n("Go to Tab Number %1").arg( 4),3,KShortcut("Alt+4"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_4");
  new TabAction(i18n("Go to Tab Number %1").arg( 5),4,KShortcut("Alt+5"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_5");
  new TabAction(i18n("Go to Tab Number %1").arg( 6),5,KShortcut("Alt+6"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_6");
  new TabAction(i18n("Go to Tab Number %1").arg( 7),6,KShortcut("Alt+7"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_7");
  new TabAction(i18n("Go to Tab Number %1").arg( 8),7,KShortcut("Alt+8"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_8");
  new TabAction(i18n("Go to Tab Number %1").arg( 9),8,KShortcut("Alt+9"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_9");
  new TabAction(i18n("Go to Tab Number %1").arg(10),9,KShortcut("Alt+0"),this,SLOT(goToTab(int)),actionCollection(),"go_to_tab_0");

  new KAction(i18n("&Clear Window"),0,KShortcut("Ctrl+L"),this,SLOT(clearWindow()),actionCollection(),"clear_window");
  
  KStdAction::find(this, SLOT(findText()), actionCollection());
  KStdAction::findNext(this, SLOT(findNextText()), actionCollection());
  
  new KAction(i18n("&IRC Color..."), "colorize", CTRL+Key_K, this, SLOT(addIRCColor()), actionCollection(), "irc_colors");
  new KAction(i18n("&Remember Line"), 0,  KShortcut("Ctrl+R") , this, SLOT(insertRememberLine()), actionCollection(), "insert_remember_line");
  new KAction(i18n("&Character..."), 0, 0, this, SLOT(insertCharacter()), actionCollection(), "insert_character");
  
  new KAction(i18n("Close &All Open Queries"), 0, KShortcut("F11"), this, SLOT(closeQueries()), actionCollection(), "close_queries");

  new KAction(i18n("Tabpage Mode"),"tabpage",0,this,SLOT (switchToTabPageMode()),actionCollection(),"mdi_tabpage_mode");
  new KAction(i18n("Toplevel Mode"),"toplevel",0,this,SLOT (switchToToplevelMode()),actionCollection(),"mdi_toplevel_mode");
  new KAction(i18n("Childframe Mode"),"childframe",0,this,SLOT (switchToChildframeMode()),actionCollection(),"mdi_childframe_mode");
  new KAction(i18n("IDEAl Mode"),"ideal",0,this,SLOT (switchToIDEAlMode()),actionCollection(),"mdi_ideal_mode");

  // Initialize KMainWindow->statusBar()
  statusBar();
  statusBar()->insertItem(i18n("Ready."),StatusText,1);
  statusBar()->insertItem("lagometer",LagOMeter,0,true);
  // Show "Lag unknown"
  resetLag();
  statusBar()->setItemAlignment(StatusText,QLabel::AlignLeft);

  // Initialize KMainWindow->menuBar()
  showMenubar();

#ifdef USE_MDI
  connect(this,SIGNAL (viewActivated(KMdiChildView*)),this,SLOT (changeToView(KMdiChildView*)) );
#else
  connect( viewContainer,SIGNAL (currentChanged(QWidget*)),this,SLOT (changeView(QWidget*)) );
  connect( viewContainer,SIGNAL (closeTab(QWidget*)),this,SLOT (closeView(QWidget*)) );
  connect(this, SIGNAL (closeTab(int)), viewContainer, SLOT (tabClosed(int)));
#endif

  // set up system tray
  tray = new TrayIcon(this);
  connect(this, SIGNAL(startNotification(QWidget*)), tray, SLOT(startNotification(QWidget*)));
  connect(this, SIGNAL(endNotification(QWidget*)), tray, SLOT(endNotification(QWidget*)));
  connect(tray, SIGNAL(quitSelected()), this, SLOT(quitProgram()));
#if KDE_VERSION >= KDE_MAKE_VERSION(3, 2, 0)
  KPopupMenu *trayMenu = tray->contextMenu();
#ifdef USE_KNOTIFY
  configureNotificationsAction->plug(trayMenu);
#endif
  preferencesAction->plug(trayMenu);
#endif

  // decide whether to show the tray icon or not
  updateTrayIcon();

#ifdef USE_MDI
  createGUI(0);
#else
  createGUI();
#endif

  resize(700, 500);  // Give the app a sane default size
  setAutoSaveSettings();
#if KDE_VERSION < KDE_MAKE_VERSION(3, 1, 0)
  showToolBarAction->setChecked(KonversationApplication::preferences.getShowToolBar());
  showToolbar();
#endif
#if KDE_VERSION < KDE_MAKE_VERSION(3, 1, 90)
  showStatusBarAction->setChecked(KonversationApplication::preferences.getShowStatusBar());
  showStatusbar();
#endif
  showMenuBarAction->setChecked(KonversationApplication::preferences.getShowMenuBar());
  showMenubar();

  // set up KABC with a nice gui error dialog
  KABC::GuiErrorHandler *m_guiErrorHandler = new KABC::GuiErrorHandler(this);
  Konversation::Addressbook::self()->getAddressBook()->setErrorHandler(m_guiErrorHandler);

  // demo how to add additional dock windows
//  QListView* dockList=new QListView(this);
//  addToolWindow(dockList,KDockWidget::DockLeft,getMainDockWidget());
  
  if(KonversationApplication::preferences.getOpenWatchedNicksAtStartup()) {
    openNicksOnlinePanel();
  }
}

KonversationMainWindow::~KonversationMainWindow()
{
  deleteDccPanel();
  if(dccTransferHandler) delete dccTransferHandler;
}

void KonversationMainWindow::switchToTabPageMode()
{
#ifdef USE_MDI
  KMdiMainFrm::switchToTabPageMode();
  tabWidget()->setTabReorderingEnabled(true);
  tabWidget()->setHoverCloseButton(true);
  updateTabPlacement();
  m_pTaskBar->switchOn(false);
#if QT_VERSION >= 0x030200
  KPushButton* closeBtn = new KPushButton(this);
  closeBtn->setPixmap(KGlobal::iconLoader()->loadIcon("tab_remove", KIcon::Small));
  closeBtn->resize(22, 22);
  closeBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  closeBtn->hide();
  tabWidget()->setCornerWidget(closeBtn);
  connect(closeBtn, SIGNAL(clicked()), this, SLOT(closeActiveWindow()));
#endif
#endif
}

void KonversationMainWindow::switchToIDEAlMode()
{
#ifdef USE_MDI
  KMdiMainFrm::switchToIDEAlMode();
  m_pTaskBar->switchOn(true);
#endif
}

void KonversationMainWindow::switchToChildframeMode()
{
#ifdef USE_MDI
  KMdiMainFrm::switchToChildframeMode();
  m_pTaskBar->switchOn(true);
#endif
}

void KonversationMainWindow::switchToToplevelMode()
{
#ifdef USE_MDI
  KMdiMainFrm::switchToToplevelMode();
  m_pTaskBar->switchOn(true);
#endif
}

void KonversationMainWindow::updateTabPlacement()
{
#ifdef USE_MDI
  if(tabWidget())
    tabWidget()->setTabPosition((KonversationApplication::preferences.getTabPlacement()==Preferences::Top) ?
                                 QTabWidget::Top : QTabWidget::Bottom);
#else
  viewContainer->setTabPosition((KonversationApplication::preferences.getTabPlacement()==Preferences::Top) ?
                                 QTabWidget::Top : QTabWidget::Bottom);
#endif
}

void KonversationMainWindow::openPreferences()
{
  emit openPrefsDialog();
}

void KonversationMainWindow::openKeyBindings()
{
  KKeyDialog::configure(actionCollection());
}

void KonversationMainWindow::showToolbar()
{
#if KDE_VERSION < KDE_MAKE_VERSION(3, 1, 0)
  if(showToolBarAction->isChecked()) toolBar("mainToolBar")->show();
  else toolBar("mainToolBar")->hide();

  KonversationApplication::preferences.setShowToolBar(showToolBarAction->isChecked());
#endif
}

void KonversationMainWindow::showMenubar()
{
  if(showMenuBarAction->isChecked()) menuBar()->show();
  else
  {
    QString accel=showMenuBarAction->shortcut().toString();
    KMessageBox::information(this,i18n("<qt>This will hide the menu bar completely."
                                       "You can show it again by typing %1.</qt>").arg(accel),
                                       "Hide menu bar","HideMenuBarWarning");
    menuBar()->hide();
  }

  KonversationApplication::preferences.setShowMenuBar(showMenuBarAction->isChecked());
}

void KonversationMainWindow::showStatusbar()
{
#if KDE_VERSION < KDE_MAKE_VERSION(3, 1, 90)
  if(showStatusBarAction->isChecked()) statusBar()->show();
  else statusBar()->hide();

  KonversationApplication::preferences.setShowStatusBar(showStatusBarAction->isChecked());
#endif
}
/** Call this when you have already put a message in the serverView window, and want a message in the front most
 *  window if it's on the same server, but not put the message twice.
 */
void KonversationMainWindow::appendToFrontmostIfDifferent(const QString& type,const QString& message,ChatWindow* serverView)
{
  Q_ASSERT(serverView); if(!serverView) return;
  updateFrontView();
  if(frontView && (ChatWindow *)frontView != serverView && frontView->getServer()==serverView->getServer())
    frontView->appendServerMessage(type,message);
}
void KonversationMainWindow::appendToFrontmost(const QString& type,const QString& message,ChatWindow* serverView)
{
  // TODO: Make it an option to direct all status stuff into the status panel
  Q_ASSERT(serverView); if(!serverView) return;
  updateFrontView();
  if(!frontView ||                                                  // Check if the frontView can actually display text or ...
     serverView->getServer()!=frontView->getServer() ||             // if it does not belong to this server or...
     KonversationApplication::preferences.getRedirectToStatusPane())// if the user decided to force it.
  {
    // if not, take server specified fallback view instead
    serverView->appendServerMessage(type,message);
    // FIXME: this signal should be sent from the status panel instead, so it
    //        can be using the correct highlight color, would be more consistent
    //        anyway!
    newText(serverView,QString::null,true);
  }
  else
    frontView->appendServerMessage(type,message);
}

#ifdef USE_MDI
void KonversationMainWindow::addMdiView(ChatWindow* view,int color,bool on, bool weinitiated)
#else
void KonversationMainWindow::addView(ChatWindow* view,int color,const QString& label,bool on, bool weinitiated)
#endif
{
  // TODO: Make sure to add DCC status tab at the end of the list and all others
  // before the DCC tab. Maybe we should also make sure to order Channels
  // Queries and DCC chats in groups
#ifdef USE_MDI
  addWindow(view);
  view->setLedColor(color);
  if(tabWidget()) tabWidget()->setTabIconSet(view,images.getLed(color,false,true));
  KMdiTaskBarButton* button=m_pTaskBar->getButton(view);
  button->setIconSet(images.getLed(color,false,true));
#else
  viewContainer->addTab(view,label,color,on);
#endif
  // Check, if user was typing in old input line
  bool doBringToFront=true;

  // make sure that bring to front only works when the user wasn't typing something
  if(frontView)
  {
    if(!frontView->getTextInLine().isEmpty()) doBringToFront=false;
  }

  if(!KonversationApplication::preferences.getFocusNewQueries() && view->getType()==ChatWindow::Query && !weinitiated)
	  doBringToFront = false;

  // bring view to front unless it's a raw log window or the user was typing
  if(KonversationApplication::preferences.getBringToFront() && doBringToFront &&
    view->getType()!=ChatWindow::RawLog)
  {
    showView(view);
  }

#ifdef USE_MDI
  connect(view,SIGNAL (online(ChatWindow*,bool)),this,SLOT (setTabOnline(ChatWindow*,bool)) );
  connect(view,SIGNAL (chatWindowCloseRequest(ChatWindow*)),this,SLOT (closeWindow(ChatWindow*)) );
  connect(view,SIGNAL (setNotification(ChatWindow*,const QIconSet&,const QString&)),this,SLOT (setWindowNotification(ChatWindow*,const QIconSet&,const QString&)) );
#else
  connect(view,SIGNAL (online(ChatWindow*,bool)),viewContainer,SLOT (setTabOnline(ChatWindow*,bool)) );
#endif
}

void KonversationMainWindow::showView(ChatWindow* view)
{
#ifdef USE_MDI
  view->raise();
  if(tabWidget()) tabWidget()->showPage(view);
#else
  // Don't bring Tab to front if TabWidget is hidden. Otherwise QT gets confused
  // and shows the Tab as active but will display the wrong pane
  if(viewContainer->isVisible())
  {
    // TODO: add adjustFocus() here?
    viewContainer->showPage(view);
  }
#endif
}

#ifdef USE_MDI
void KonversationMainWindow::setWindowNotification(ChatWindow* view,const QIconSet& iconSet,const QString& color) // USE_MDI
#else
void KonversationMainWindow::setWindowNotification(ChatWindow*,const QIconSet&,const QString&) // USE_MDI
#endif
{
#ifdef USE_MDI
  if(tabWidget())
  {
    tabWidget()->setTabIconSet(view,iconSet);
    tabWidget()->setTabColor(view,QColor(color));
  }
  KMdiTaskBarButton* button=m_pTaskBar->getButton(view);
  button->setIconSet(iconSet);
  button->setPaletteForegroundColor(QColor(color));
#endif
}

#ifdef USE_MDI

void KonversationMainWindow::setTabOnline(ChatWindow* view,bool online)
{
/*
  // just testing here... ignore this bit for now
  KMdiTaskBarButton* button=m_pTaskBar->getButton(view);
  if(tabWidget()) tabWidget()->setTabColor(view,QColor("#eeeeee"));
  button->setText("<qt><b>fgfg</b></qt>");
*/
}
#else
void KonversationMainWindow::setTabOnline(ChatWindow* ,bool ) {}
#endif
#ifdef USE_MDI
void KonversationMainWindow::closeWindow(ChatWindow* viewToClose) // USE_MDI
#else
void KonversationMainWindow::closeWindow(ChatWindow*) // USE_MDI
#endif
{
#ifdef USE_MDI
  ChatWindow* view=static_cast<ChatWindow*>(viewToClose);
  if(view)
  {
    // if this view was the front view, delete the pointer
    // JOHNFLUX - move to previous view
    if(view==previousFrontView) previousFrontView=0;
    if(view==frontView) frontView=previousFrontView;

    emit endNotification(viewToClose);

    ChatWindow::WindowType viewType=view->getType();

    QString viewName=view->getName();
    // the views should know by themselves how to close

    if(viewType==ChatWindow::Status)            ;
    else if(viewType==ChatWindow::Channel)      ;
    else if(viewType==ChatWindow::ChannelList)  ;
    else if(viewType==ChatWindow::Query)        ;
    else if(viewType==ChatWindow::RawLog)       ;
    else if(viewType==ChatWindow::DccChat)      ;

    else if(viewType==ChatWindow::DccPanel)     ;
    else if(viewType==ChatWindow::Konsole)      ;
    else if(viewType==ChatWindow::UrlCatcher)   urlCatcherPanel=0;
    else if(viewType==ChatWindow::NicksOnline)  nicksOnlinePanel=0;

    // FIXME: don't delete dcc panel until we know how to safely hide
    //        and show it
    if(viewType==ChatWindow::DccPanel)
    {
      /*
//    according to the documentation this should remove the panel, but it does not work.
      removeWindowFromMdi(viewToClose);
      dccPanelOpen=false;
      */
    }
    else
    {
      KMdiMainFrm::closeWindow(viewToClose);
      viewToClose->deleteLater();
    }
/*
    else if(viewType==ChatWindow::Notice);
    else if(viewType==ChatWindow::SNotice);
*/
  }
#endif
}

void KonversationMainWindow::closeActiveWindow() // USE_MDI
{
#ifdef USE_MDI
  closeWindow(static_cast<ChatWindow*>(activeWindow()));
#endif
}

void KonversationMainWindow::closeView(QWidget* viewToClose)
{
#ifndef USE_MDI
  ChatWindow* view=static_cast<ChatWindow*>(viewToClose);
  if(view)
  {
    // if this view was the front view, delete the pointer
    if(view==previousFrontView) previousFrontView=0;
    if(view==frontView) frontView=previousFrontView;

    emit endNotification(viewToClose);

    ChatWindow::WindowType viewType=view->getType();

    QString viewName=view->getName();
    // the views should know by themselves how to close

    if(viewType==ChatWindow::Status)            view->closeYourself();
    else if(viewType==ChatWindow::Channel)      view->closeYourself();
    else if(viewType==ChatWindow::ChannelList)  view->closeYourself();
    else if(viewType==ChatWindow::Query)        view->closeYourself();
    else if(viewType==ChatWindow::RawLog)       view->closeYourself();
    else if(viewType==ChatWindow::DccChat)      view->closeYourself();

    else if(viewType==ChatWindow::DccPanel)     closeDccPanel();
    else if(viewType==ChatWindow::Konsole)      closeKonsolePanel(view);
    else if(viewType==ChatWindow::UrlCatcher)   closeUrlCatcher();
    else if(viewType==ChatWindow::NicksOnline)  closeNicksOnlinePanel();

/*
    else if(viewType==ChatWindow::Notice);
    else if(viewType==ChatWindow::SNotice);
*/
  }
#endif
}

void KonversationMainWindow::openLogfile()
{
  if(frontView)
  {
    ChatWindow* view=static_cast<ChatWindow*>(frontView);
    ChatWindow::WindowType viewType=view->getType();
    if(viewType==ChatWindow::Channel ||
       viewType==ChatWindow::Query ||
       viewType==ChatWindow::Status)
    {
      view->openLogfile();
    }
    else
    {
    }
  }
}

void KonversationMainWindow::addKonsolePanel()
{
#ifdef USE_MDI
  KonsolePanel* panel=new KonsolePanel(i18n("Konsole"));
  addMdiView(panel,3);
#else
  KonsolePanel* panel=new KonsolePanel(getViewContainer());
  addView(panel,3,i18n("Konsole"));
  connect(panel,SIGNAL (deleted(ChatWindow*)),this,SLOT (closeKonsolePanel(ChatWindow*)) );
#endif
}

void KonversationMainWindow::closeKonsolePanel(ChatWindow* konsolePanel)
{
#ifndef USE_MDI
  getViewContainer()->removePage(konsolePanel);
  // tell QT to delete the panel during the next event loop since we are inside a signal here
  konsolePanel->deleteLater();
#endif
}

void KonversationMainWindow::openChannelList()
{
  if(frontServer)
  {
    ChannelListPanel* panel=frontServer->getChannelListPanel();
    if(panel) {
#ifdef USE_MDI
      panel->show();
      if(tabWidget()) tabWidget()->showPage(panel);
#else
      getViewContainer()->showPage(panel);
#endif
    } else {
      int ret = KMessageBox::warningContinueCancel(this,i18n("Using this function may result in a lot "
                                         "of network traffic. If your connection is not fast "
                                         "enough, it is possible that your client will be "
                                         "disconnected by the server."), i18n("Channel List Warning"),
                                         KStdGuiItem::cont(), "ChannelListWarning");

      if(ret == KMessageBox::Continue) {
        frontServer->addChannelListPanel();
      }
    }
  }
  else
  {
    KMessageBox::information(this,
                             i18n(
                                  "The channel list can only be opened from a "
                                  "query, channel or status window to find out, "
                                  "which server this list belongs to."
                                 ),
                             i18n("Channel List"),
                             "ChannelListNoServerSelected");
  }
}

void KonversationMainWindow::addUrlCatcher()
{
  // if the panel wasn't open yet
  if(urlCatcherPanel==0)
  {
#ifdef USE_MDI
    urlCatcherPanel=new UrlCatcher(i18n("URL Catcher"));
    addMdiView(urlCatcherPanel,2,true);
#else
    urlCatcherPanel=new UrlCatcher(getViewContainer());
    addView(urlCatcherPanel,2,i18n("URL Catcher"),true);
#endif

    KonversationApplication *konvApp=static_cast<KonversationApplication *>(KApplication::kApplication());
    connect(konvApp,SIGNAL (catchUrl(const QString&,const QString&)),
        urlCatcherPanel,SLOT (addUrl(const QString&,const QString&)) );
    connect(urlCatcherPanel,SIGNAL (deleteUrl(const QString&,const QString&)),
                      konvApp,SLOT (deleteUrl(const QString&,const QString&)) );
    connect(urlCatcherPanel,SIGNAL (clearUrlList()),
                      konvApp,SLOT (clearUrlList()) );

    QStringList urlList=konvApp->getUrlList();
    for(unsigned int index=0;index<urlList.count();index++)
    {
      QString urlItem=urlList[index];
      urlCatcherPanel->addUrl(urlItem.section(' ',0,0),urlItem.section(' ',1,1));
    } // for
  }
}

void KonversationMainWindow::closeUrlCatcher()
{
  // if there actually is a dcc panel
  if(urlCatcherPanel)
  {
    delete urlCatcherPanel;
    urlCatcherPanel=0;
  }
}

void KonversationMainWindow::addDccPanel()
{
  // if the panel wasn't open yet
  if(dccPanel==0)
  {
#ifdef USE_MDI
    dccPanel=new DccPanel(i18n("DCC Status"));
    addMdiView(dccPanel,3);
#else
    dccPanel=new DccPanel(getViewContainer());
    addView(dccPanel,3,i18n("DCC Status"));
#endif
    dccPanelOpen=true;
  }
  // show already opened panel
  else
  {
    if(!dccPanelOpen)
    {
#ifdef USE_MDI
      addMdiView(dccPanel,3);
#else
      addView(dccPanel,3,i18n("DCC Status"));
#endif
      dccPanelOpen=true;
    }
    // no highlight color for DCC panels
    newText(dccPanel,QString::null,true);
  }
}

DccPanel* KonversationMainWindow::getDccPanel()
{
  return dccPanel;
}

void KonversationMainWindow::closeDccPanel()
{
  // if there actually is a dcc panel
  if(dccPanel)
  {
     // hide it from view, does not delete it
#ifndef USE_MDI
    getViewContainer()->removePage(dccPanel);
#endif
    dccPanelOpen=false;
  }
}

void KonversationMainWindow::deleteDccPanel()
{
  if(dccPanel)
  {
    closeDccPanel();
    delete dccPanel;
    dccPanel=0;
  }
}

void KonversationMainWindow::addDccChat(const QString& myNick,const QString& nick,const QString& numericalIp,const QStringList& arguments,bool listen)
{
  if(frontServer)
  {
#ifdef USE_MDI
    DccChat* dccChatPanel=new DccChat(QString("-"+nick+"-"),frontServer,myNick,nick,arguments,listen);
    addMdiView(dccChatPanel,3);
#else
    DccChat* dccChatPanel=new DccChat(getViewContainer(),frontServer,myNick,nick,arguments,listen);
    addView(dccChatPanel,3,dccChatPanel->getName());
#endif

    connect(dccChatPanel,SIGNAL (newText(QWidget*,const QString&,bool)),this,SLOT (newText(QWidget*,const QString&,bool)) );

    if(listen) frontServer->queue(QString("PRIVMSG %1 :\x01%2 CHAT chat %3 %4\x01").arg(nick).arg("DCC").arg(numericalIp).arg(dccChatPanel->getPort()));
  }
}

StatusPanel* KonversationMainWindow::addStatusView(Server* server)
{
#ifdef USE_MDI
  StatusPanel* statusView=new StatusPanel(server->getServerName());
#else
  StatusPanel* statusView=new StatusPanel(getViewContainer());
#endif

  // first set up internal data ...
  statusView->setServer(server);
  statusView->setIdentity(server->getIdentity());
  statusView->setName(server->getServerName());

  // ... then put it into the tab widget, otherwise we'd have a race with server member
#ifdef USE_MDI
  addMdiView(statusView,2,false);
#else
  addView(statusView,2,server->getServerName(),false);
#endif

  connect(statusView,SIGNAL (newText(QWidget*,const QString&,bool)),this,SLOT (newText(QWidget*,const QString&,bool)) );
  connect(statusView,SIGNAL (sendFile()),server,SLOT (requestDccSend()) );
  connect(statusView,SIGNAL (prefsChanged()),this,SLOT (channelPrefsChanged()) );
  connect(server,SIGNAL (awayState(bool)),statusView,SLOT (indicateAway(bool)) );

  // make sure that frontServer gets set on adding the first status panel, too,
  // since there won't be a changeView happening
  if(!frontServer) frontServer=server;

  return statusView;
}

Channel* KonversationMainWindow::addChannel(Server* server, const QString& name)
{
#ifdef USE_MDI
  Channel* channel=new Channel(name);
#else
  Channel* channel=new Channel(getViewContainer());
#endif

  channel->setServer(server);
  channel->setName(name);

#ifdef USE_MDI
  addMdiView(channel,1);
#else
  addView(channel,1,name);
#endif

  connect(channel,SIGNAL (newText(QWidget*,const QString&,bool)),this,SLOT (newText(QWidget*,const QString&,bool)) );
  connect(channel,SIGNAL (prefsChanged()),this,SLOT (channelPrefsChanged()) );
  connect(server,SIGNAL (awayState(bool)),channel,SLOT (indicateAway(bool)) );

  return channel;
}

Query* KonversationMainWindow::addQuery(Server* server, const QString& name, bool weinitiated)
{
#ifdef USE_MDI
  Query* query=new Query(name);
#else
  Query* query=new Query(getViewContainer());
#endif

  query->setServer(server);
  query->setName(name);

#ifdef USE_MDI
  addMdiView(query,0, true, weinitiated);
#else
  addView(query,0,name, true, weinitiated);
#endif

  connect(query,SIGNAL (newText(QWidget*,const QString&,bool)),this,SLOT (newText(QWidget*,const QString&,bool)) );
  connect(server,SIGNAL (awayState(bool)),query,SLOT (indicateAway(bool)) );

  return query;
}

RawLog* KonversationMainWindow::addRawLog(Server* server)
{
#ifdef USE_MDI
  RawLog* rawLog=new RawLog(i18n("Raw Log"));
#else
  RawLog* rawLog=new RawLog(getViewContainer());
#endif

  rawLog->setServer(server);
  rawLog->setLog(false);

#ifdef USE_MDI
  addMdiView(rawLog,2,false);
#else
  addView(rawLog,2,i18n("Raw Log"),false);
#endif

  return rawLog;
}

ChannelListPanel* KonversationMainWindow::addChannelListPanel(Server* server)
{
#ifdef USE_MDI
  ChannelListPanel* channelListPanel=new ChannelListPanel(i18n("Channel List"));
#else
  ChannelListPanel* channelListPanel=new ChannelListPanel(getViewContainer());
#endif

  channelListPanel->setServer(server);

#ifdef USE_MDI
  addMdiView(channelListPanel,2);
#else
  addView(channelListPanel,2,i18n("Channel List"));
#endif

  return channelListPanel;
}

void KonversationMainWindow::newText(QWidget* widget,const QString& highlightColor,bool important)
{
  ChatWindow* view=static_cast<ChatWindow*>(widget);

#ifdef USE_MDI
  if(view!=activeWindow())
#else
  if(view!=getViewContainer()->currentPage())
#endif
  {
#ifdef USE_MDI
    view->setOn(true,important);
    if(!highlightColor.isNull()) view->setLabelColor(highlightColor);
#else
    getViewContainer()->changeTabState(view,true,important,highlightColor);
#endif

    emit startNotification(view);
  }
  else if(!isActiveWindow() && view->getServer() && view->getServer()->connected())
  {
    emit startNotification(view);
  }
}

void KonversationMainWindow::updateFrontView()
{
#ifdef USE_MDI
  ChatWindow* view=static_cast<ChatWindow*>(activeWindow());
#else
  ChatWindow* view=static_cast<ChatWindow*>(getViewContainer()->currentPage());
#endif
  if(view)
  {
    // Make sure that only views with info output get to be the frontView
    if(frontView)
        previousFrontView=frontView;
    if(view->frontView()) frontView=view;
    // Make sure that only text views get to be the searchView
    if(view->searchView()) searchView=view;
  }
}
#ifdef USE_MDI
void KonversationMainWindow::changeToView(KMdiChildView* viewToChange) // USE_MDI
{
  ChatWindow* view=static_cast<ChatWindow*>(viewToChange);
  if(frontView)
    previousFrontView = frontView;
  frontView=0;
  searchView=0;


  // display this server's lag time
  frontServer=view->getServer();
  if(frontServer) updateLag(frontServer,frontServer->getLag());
  updateFrontView();

  view->setOn(false);
  view->setLabelColor(QString::null);
  emit endNotification(viewToChange);
}
#else
void KonversationMainWindow::changeToView(KMdiChildView* /*viewToChange*/)
{
}
#endif
// this function will not be used in USE_MDI mode but moc will complain if it's not there
void KonversationMainWindow::changeView(QWidget* viewToChange)
{
#ifndef USE_MDI
  ChatWindow* view=static_cast<ChatWindow*>(viewToChange);
  if(frontView)
    previousFrontView = frontView;
  frontView=0;
  searchView=0;


  // display this server's lag time
  frontServer=view->getServer();
  if(frontServer) updateLag(frontServer,frontServer->getLag());
  updateFrontView();

  viewContainer->changeTabState(view,false,false,QString::null);
  emit endNotification(viewToChange);
#endif
}

bool KonversationMainWindow::queryClose()
{
  KonversationApplication* konv_app = static_cast<KonversationApplication*>(KApplication::kApplication());

  if(konv_app->sessionSaving()) {
    m_closeApp = true;
  }

  if(KonversationApplication::preferences.getShowTrayIcon() && !m_closeApp) {
    // Message copied from kopete...
    KMessageBox::information(this,
      i18n( "<qt>Closing the main window will keep Konversation running in the "
      "system tray. Use 'Quit' from the 'File' menu to quit the application.</qt>" ),
      i18n( "Docking in System Tray" ), "hideOnCloseInfo");

    hide();
    return false;
  }

  // send quit to all servers
  emit quitServer();

  return true;
}

void KonversationMainWindow::quitProgram()
{
  // will call queryClose()
  m_closeApp = true;
  close();
}

void KonversationMainWindow::openNicksOnlinePanel()
{
  if(!nicksOnlinePanel)
  {
#ifdef USE_MDI
    nicksOnlinePanel=new NicksOnline( i18n("Nicks Online"));
    addMdiView(nicksOnlinePanel,2,true);
#else
    nicksOnlinePanel=new NicksOnline(getViewContainer());
    addView(nicksOnlinePanel, 2, i18n("Nicks Online"), true);
#endif

    connect(nicksOnlinePanel,SIGNAL (editClicked()),this,SLOT (openNotify()) );

    connect(nicksOnlinePanel,SIGNAL (doubleClicked(const QString&,const QString&)),this,SLOT (notifyAction(const QString&,const QString&)) );

    connect(this,SIGNAL (nicksNowOnline(const QString&,const QStringList&,bool)),nicksOnlinePanel,SLOT (setOnlineList(const QString&,const QStringList&,bool)) );
  }
}

void KonversationMainWindow::closeNicksOnlinePanel()
{
  if(nicksOnlinePanel)
  {
    delete nicksOnlinePanel;
    nicksOnlinePanel=0;
  }
}

void KonversationMainWindow::openServerList()
{
  //emit openPrefsDialog(Preferences::ServerListPage);
  Konversation::ServerListDialog dialog;
  KonversationApplication *konvApp = static_cast<KonversationApplication *>(KApplication::kApplication());
  connect(&dialog, SIGNAL(connectToServer(int)), konvApp, SLOT(connectToAnotherServer(int)));

  dialog.exec();
}

void KonversationMainWindow::openQuickConnectDialog()
{
	emit showQuickConnectDialog();
}

void KonversationMainWindow::openNotify()
{
  emit openPrefsDialog(Preferences::NotifyPage);
}

// TODO: Let an own class handle notify things
void KonversationMainWindow::setOnlineList(Server* notifyServer,const QStringList& list, bool changed)
{
  emit nicksNowOnline(notifyServer->getServerName(),list,changed);
  if (changed && nicksOnlinePanel) newText(nicksOnlinePanel, QString::null, true);
}

void KonversationMainWindow::notifyAction(const QString& serverName,const QString& nick)
{
  KonversationApplication* konv_app=static_cast<KonversationApplication*>(KApplication::kApplication());
  Server* server=konv_app->getServerByName(serverName);
  server->notifyAction(nick);
}

void KonversationMainWindow::channelPrefsChanged()
{
  emit prefsChanged();
}

#ifndef USE_MDI
LedTabWidget* KonversationMainWindow::getViewContainer()
{
  return viewContainer;
}
#endif

void KonversationMainWindow::updateFonts()
{
#ifndef USE_MDI
  getViewContainer()->updateTabs();
#endif
}

void KonversationMainWindow::updateLag(Server* lagServer,int msec)
{
  // show lag only of actual server
  if(lagServer==frontServer)
  {
    statusBar()->changeItem(i18n("Ready."),StatusText);

    QString lagString(i18n("Lag: %1 ms").arg(msec));
    statusBar()->changeItem(lagString,LagOMeter);
  }
}

void KonversationMainWindow::tooLongLag(Server* lagServer,int msec)
{

  if((msec % 5000)==0)
  {
    int secsleft = msec/1000;
    QStringList timeString;
    
    if(secsleft%60 == 1) {
	    timeString.push_front(i18n("1 second"));
	    secsleft -= 1;
    } else if(secsleft %60) {
	    timeString.push_front(i18n("%1 seconds").arg(secsleft%60));
	    secsleft -= secsleft%60;
    }
    if(secsleft % 360 == 60) {
	    timeString.push_front(i18n("1 minute"));
	    secsleft -= 60;
    } else if(secsleft % 360) {
	    timeString.push_front(i18n("%1 minutes").arg((secsleft%360)/60));
	    secsleft -= secsleft%360;
    }
    if(secsleft % (360*24) == 360){
	    timeString.push_front(i18n("1 hour"));
            secsleft -= 360;
    } else if(secsleft % (360*24)) {
            timeString.push_front(i18n("%1 hours").arg((secsleft%360*24)/360));
            secsleft -= secsleft%(360*24);
    }
    if(secsleft == (360*24)) {
            timeString.push_front(i18n("1 day"));
            secsleft -= 360*24;
    } else if(secsleft) {
            timeString.push_front(i18n("%1 days").arg(secsleft/(360*24)));
    }
    Q_ASSERT(!timeString.empty()); if(timeString.empty()) return;
    QString timestring = timeString.first();
    timeString.pop_front();
    if(!timeString.empty()) {
	    timestring += " and " + timeString.join(", ");
    }
    QString lagString(i18n("No answer from server %1 for more than %2").arg(lagServer->getServerName(), timestring));
    statusBar()->changeItem(lagString,StatusText);
  }

  QString lagString(i18n("Lag: %1 s").arg(msec/1000));
  statusBar()->changeItem(lagString,LagOMeter);
}

// TODO: Make this server dependant
void KonversationMainWindow::resetLag()
{
  statusBar()->changeItem(i18n("Lag: not known"),LagOMeter);
}

void KonversationMainWindow::closeTab()
{
  // -1 = close currently visible tab
  emit closeTab(-1);
}

void KonversationMainWindow::nextTab()
{
#ifdef USE_MDI
  activateNextWin();
#else
  goToTab(getViewContainer()->currentPageIndex()+1);
#endif
}

void KonversationMainWindow::previousTab()
{
#ifdef USE_MDI
  activatePrevWin();
#else
  goToTab(getViewContainer()->currentPageIndex()-1);
#endif
}

void KonversationMainWindow::goToTab(int page)
{
#ifdef USE_MDI
  if(tabWidget())
  {
    if(page>=0 && page<tabWidget()->count()) activateView(page);
  }
#else
    if ( page >= getViewContainer()->count() )
        page = 0;
    else if ( page < 0 )
        page = getViewContainer()->count() - 1;

    if(page>=0)
    {
        getViewContainer()->setCurrentPage(page);
        ChatWindow* newPage=static_cast<ChatWindow*>(getViewContainer()->page(page));
        newPage->adjustFocus();
    }
#endif
}

void KonversationMainWindow::findText()
{
  if(searchView==0)
  {
    KMessageBox::sorry(this,
                       i18n("You can only search in text fields."),
                       i18n("Find Text Information"));
  }
  else
  {
    searchView->getTextView()->search();
  }
}

void KonversationMainWindow::findNextText()
{
  if(searchView)
  {
    searchView->getTextView()->searchAgain();
  }
}

void KonversationMainWindow::clearWindow()
{
  if (frontView)
    frontView->getTextView()->clear();
}

void KonversationMainWindow::openNotifications()
{
#ifdef USE_KNOTIFY
  (void) KNotifyDialog::configure(this);
#endif
}

void KonversationMainWindow::updateTrayIcon()
{
  if(KonversationApplication::preferences.getShowTrayIcon())
    tray->show();
  else
    tray->hide();

  tray->setNotificationEnabled(KonversationApplication::preferences.getTrayNotify());
}

void KonversationMainWindow::addIRCColor()
{
  IRCColorChooser dlg(this, &(KonversationApplication::preferences));

  if(dlg.exec() == QDialog::Accepted) {
    frontView->appendInputText(dlg.color());
  }
}

void KonversationMainWindow::insertRememberLine()
{
#ifndef USE_MDI
  if(KonversationApplication::preferences.getShowRememberLineInAllWindows())
  {
    int total = getViewContainer()->count()-1;
    ChatWindow* nextPage;

    for(int i = 0; i <= total; ++i)
    {
      nextPage = static_cast<ChatWindow*>(getViewContainer()->page(i));
      if(nextPage->getType() == ChatWindow::Channel ||
          nextPage->getType() == ChatWindow::Query)
      {
         nextPage->insertRememberLine();
      }
    }
  }

  else
  {
    if(frontView->getType() == ChatWindow::Channel ||
        frontView->getType() == ChatWindow::Query)
    {
      frontView->insertRememberLine();
    }
  }
#endif
}

void KonversationMainWindow::closeQueries()
{
#ifndef USE_MDI
  int total=getViewContainer()->count()-1;
  int operations=0;
  ChatWindow* nextPage;

  for(int i=0;i<=total;i++)
  {
    if (operations > total)
      break;

    nextPage=static_cast<ChatWindow*>(getViewContainer()->page(i));

    if(nextPage && nextPage->getType()==ChatWindow::Query) {
      nextPage->closeYourself();
      --i; /* Tab indexes changed */
    }
    ++operations;
  }
#endif
}

bool KonversationMainWindow::event(QEvent* e)
{
#ifndef USE_MDI
  if(e->type() == QEvent::WindowActivate) {
    emit endNotification(getViewContainer()->currentPage());
  }
#endif

  return KMainWindow::event(e);
}

void KonversationMainWindow::serverQuit(Server* server)
{
  if(server == frontServer) {
    frontServer = 0;
  }

  if(frontView->getServer() == server) {
    frontView = 0;
  }

  tray->removeServer(server);
  delete server->getStatusView();
  delete server;
}

void KonversationMainWindow::openToolbars()
{
  KEditToolbar dlg(actionCollection());

  if(dlg.exec())
  {
#ifdef USE_MDI
    createGUI(0);
#else
    createGUI();
#endif
  }
}

#if QT_VERSION >= 0x030200
void KonversationMainWindow::setShowTabBarCloseButton(bool s)
{
#ifdef USE_MDI
  // looks really strange ...
  if(tabWidget())
  {
    if(s) tabWidget()->cornerWidget()->show();
    else  tabWidget()->cornerWidget()->hide();
  }
#else
  if(s) {
    viewContainer->cornerWidget()->show();
  } else {
    viewContainer->cornerWidget()->hide();
  }
#endif
}
#else
void KonversationMainWindow::setShowTabBarCloseButton(bool) {}
#endif

void KonversationMainWindow::resizeEvent(QResizeEvent* ev)
{
#ifdef USE_MDI
  setSysButtonsAtMenuPosition();
#endif
  QWidget::resizeEvent(ev);
}

void KonversationMainWindow::insertCharacter()
{
  if(!m_insertCharDialog) {
    m_insertCharDialog = new Konversation::InsertCharDialog(frontView->getTextView()->font().family(), this);
    connect(m_insertCharDialog, SIGNAL(insertChar(const QChar&)), this, SLOT(insertChar(const QChar&)));
  }
  
  m_insertCharDialog->show();
}

void KonversationMainWindow::insertChar(const QChar& chr)
{
  frontView->appendInputText(chr);
}

#include "konversationmainwindow.moc"
