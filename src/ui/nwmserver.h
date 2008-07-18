#ifndef NWMSERVER__H
#define NWMSERVER__H

/*
 *  Copyright(C) 2006 Neuros Technology International LLC.
 *               <www.neurostechnology.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 *
 *  This program is distributed in the hope that, in addition to its
 *  original purpose to support Neuros hardware, it will be useful
 *  otherwise, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 ****************************************************************************
 *
 * Neuros Window Manager for Qtopia-core
 *
 * REVISION:
 * 3) Inserted ProcessHandler, Window management, App Show and Hide 2008-01-23 Tom Bruno tebruno99@gmail.com
 * 2) Moved into its own class. --------------------------- 2008-01-16 nerochiaro
 * 1) Initial creation. ----------------------------------- 2007-10 tebruno99@gmail.com
 *
 */

#include <QString>
#include <QByteArray>
#include <QWSServer>
#include <QWSClient>
#include <QWSWindow>

#include "processhandler.h"
#include "busywidget.h"

class NWMServer : public QObject
{
	Q_OBJECT

public:
	NWMServer();
	~NWMServer();
	void init();

public slots:

protected:

private:
	ProcessHandler pman;
	QWSServer* server;
	BusyWidget *busyWidget;
	QSettings* settings;
	QString currentLang;

	void setupConnections();
	void processStartupConfig(const QString& path = "/etc/nwm/startup");
	void initBusyWidget();

private slots:
	void windowCreated();
	void windowDestroyed(QWSWindow *);
	void windowActivated(QWSWindow *);
	void windowShown(QWSWindow *);
	void windowRaised();
	void windowLowered();
	void windowHidden(QWSWindow *);
	void windowEventHandler(QWSWindow *window, QWSServer::WindowEvent eventType);

	void hideAllWindows(const int id);
	void hideAllWindows(const QString& appname);
	void showAllWindows(const int id);
	void showAllWindows(const QString& appname);


	void nappchannelReceived(const QString& message, const QByteArray& data);
	void appAlreadyRunning(const QString& app);
	void sendCloseAllComplete();
	void sendApplicationStarted(const QString& app, const QByteArray& data);


	void startBusyHint(const QString&);
	void stopBusyHint();

};

#endif //NWMSERVER__H
