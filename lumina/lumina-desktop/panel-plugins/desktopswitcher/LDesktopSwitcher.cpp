//===========================================
//  Lumina-DE source code
//  Copyright (c) 2014, Susanne Jaeckel
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "LDesktopSwitcher.h"

LDesktopSwitcher::LDesktopSwitcher(QWidget *parent) : LPPlugin(parent, "desktopswitcher") {
  iconOld = -1;

  //Setup the widget
  label = new LTBWidget(this);
  label->setIcon( LXDG::findIcon("preferences-desktop-display-color", ":/images/preferences-desktop-display-color.png") );
  label->setToolTip(QString("Workspace 1"));
  menu = new QMenu(this);
  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuActionTriggered(QAction*)));
  label->setMenu(menu);
  this->layout()->addWidget(label);

  // Maybe a timer should be set to set the toolTip of the button,
  // becasue the workspace could be switched via Keyboard-shortcuts ...

  createMenu();
}

LDesktopSwitcher::~LDesktopSwitcher(){
}

void LDesktopSwitcher::setNumberOfDesktops(int number) {
  Display *display = QX11Info::display();
  Window rootWindow = QX11Info::appRootWindow();

  Atom atom = XInternAtom(display, "_NET_NUMBER_OF_DESKTOPS", False);
  XEvent xevent;
  xevent.type                 = ClientMessage;
  xevent.xclient.type         = ClientMessage; 
  xevent.xclient.display      = display;
  xevent.xclient.window       = rootWindow;
  xevent.xclient.message_type = atom;
  xevent.xclient.format       = 32;
  xevent.xclient.data.l[0]    = number;
  xevent.xclient.data.l[1]    = CurrentTime;
  xevent.xclient.data.l[2]    = 0;
  xevent.xclient.data.l[3]    = 0;
  xevent.xclient.data.l[4]    = 0;
  XSendEvent(display, rootWindow, False, SubstructureNotifyMask | SubstructureRedirectMask, &xevent);

  XFlush(display);
}

void LDesktopSwitcher::setCurrentDesktop(int number) {
  Display *display = QX11Info::display();
  Window rootWindow = QX11Info::appRootWindow();

  Atom atom = XInternAtom(display, "_NET_CURRENT_DESKTOP", False);
  XEvent xevent;
  xevent.type                 = ClientMessage;
  xevent.xclient.type         = ClientMessage; 
  xevent.xclient.display      = display;
  xevent.xclient.window       = rootWindow;
  xevent.xclient.message_type = atom;
  xevent.xclient.format       = 32;
  xevent.xclient.data.l[0]    = number;
  xevent.xclient.data.l[1]    = CurrentTime;
  xevent.xclient.data.l[2]    = 0;
  xevent.xclient.data.l[3]    = 0;
  xevent.xclient.data.l[4]    = 0;
  XSendEvent(display, rootWindow, False, SubstructureNotifyMask | SubstructureRedirectMask, &xevent);

  XFlush(display);
}

int LDesktopSwitcher::getNumberOfDesktops() {
  int number = -1;
  Atom a = XInternAtom(QX11Info::display(), "_NET_NUMBER_OF_DESKTOPS", true);
  Atom realType;
  int format;
  unsigned long num, bytes;
  unsigned char *data = 0;
  int status = XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), a, 0L, (~0L),
             false, AnyPropertyType, &realType, &format, &num, &bytes, &data);
  if( (status >= Success) && (num > 0) ){
    number = *data;
    XFree(data);
  }
  return number;
}

int LDesktopSwitcher::getCurrentDesktop() {
  int number = -1;
  Atom a = XInternAtom(QX11Info::display(), "_NET_CURRENT_DESKTOP", true);
  Atom realType;
  int format;
  unsigned long num, bytes;
  unsigned char *data = 0;
  int status = XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), a, 0L, (~0L),
             false, AnyPropertyType, &realType, &format, &num, &bytes, &data);
  if( (status >= Success) && (num > 0) ){
    number = *data;
    XFree(data);
  }
  return number;
}

QAction* LDesktopSwitcher::newAction(int what, QString name) {
  QAction *act = new QAction(LXDG::findIcon("preferences-desktop-display-color", ":/images/preferences-desktop-display-color.png"), name, this);
  act->setWhatsThis(QString::number(what));
  return act;
}

void LDesktopSwitcher::createMenu() {
  qDebug() << "-- vor getCurrentDesktop SWITCH";
  qDebug() << getCurrentDesktop();
  menu->clear();
  for (int i = 0; i < getNumberOfDesktops(); i++) {
    menu->addAction(newAction(i, QString("Workspace %1").arg(i +1)));
  }
}

void LDesktopSwitcher::menuActionTriggered(QAction* act) {
  setCurrentDesktop(act->whatsThis().toInt());
  label->setToolTip(QString("Workspace %1").arg(act->whatsThis().toInt() +1));
}
