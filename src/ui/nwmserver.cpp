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
 * Neuros Window Manager for Qtopia-core , implementation
 *
 * REVISION:
 *
 * 3) Inserted ProcessHandler, Window management, App Show and Hide 2008-01-23 Tom Bruno tebruno99@gmail.com
 * 2) Moved to its own class.------------------------------ 2008-01-16 nerochiaro
 * 1) Initial creation. ----------------------------------- 2007-10 tebruno99@gmail.com
 *
 */

#include <QColor>
#include <QBrush>
#include <QWSServer>
#include <QFile>
#include <QFileInfo>
#include <QScreen>

#include "nwmserver.h"

#define VIDEO_SYSINIT_CONFIG "/etc/default/outputformat"
#define DEFAULT_LANG_STRING "en"


NWMServer::NWMServer()
: QObject (),busyWidget(NULL), 
settings(NULL), currentLang(DEFAULT_LANG_STRING)
{
	server = QWSServer::instance();

	QScreen *screen = QScreen::instance();

	initBusyWidget();
	
	settings = new QSettings;
	currentLang = settings->value("regional/language",DEFAULT_LANG_STRING).toString();

	setupConnections();
	processStartupConfig();

	server->setMaxWindowRect(QRect(0,0,screen->width(),screen->height()));
}

void NWMServer :: initBusyWidget( )
{
	if (NULL !=busyWidget)
	{
		delete busyWidget;
	}

	busyWidget = new BusyWidget();
	busyWidget->setObjectName("BusyHint");
	busyWidget->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	QSize size = busyWidget->sizeHint();
	busyWidget->resize(size);
	size = QApplication::desktop()->size() - size;
	busyWidget->move( size.width()/2 ,size.height()/2 );
	busyWidget->hide();
	busyWidget->lower();
}

NWMServer::~NWMServer()
{
	if (busyWidget != NULL) delete busyWidget;
	if (settings != NULL) delete settings;
}

void NWMServer::setupConnections()
{
	connect(server,SIGNAL(windowEvent ( QWSWindow*,QWSServer::WindowEvent ) ),this,SLOT(windowEventHandler( QWSWindow*,QWSServer::WindowEvent )));

	// NAppChannel Connection
	// connect(nappChannel,SIGNAL(received(const QString&, const QByteArray&)),this,SLOT(nappchannelReceived(const QString&, const QByteArray&)));

	connect(&pman,SIGNAL(busy(const QString&)),this,SLOT(startBusyHint(const QString&)));
	connect(&pman,SIGNAL(idle()),this,SLOT(stopBusyHint()));
	connect(&pman,SIGNAL(alreadyRunning(const QString&)),this,SLOT(appAlreadyRunning(const QString&)));
	connect(&pman,SIGNAL(closeAllComplete()),this,SLOT(sendCloseAllComplete()));
	connect(&pman,SIGNAL(applicationStarted(const QString&, const QByteArray&)),this,SLOT(sendApplicationStarted(const QString&, const QByteArray&)));
}



void NWMServer :: init ()
{
	QWSServer::setBackground(QBrush("black"));

}

void NWMServer::windowCreated()
{

}

void NWMServer::windowDestroyed(QWSWindow *window)
{
	if(server->clientWindows().size() > 0)
	{
		if(window != server->clientWindows().at(0))
		server->clientWindows().at(0)->show();
	}
}

void NWMServer::windowRaised()
{

}

void NWMServer::windowActivated(QWSWindow *pwin)
{
}


void NWMServer::windowLowered()
{

}

void NWMServer::windowHidden(QWSWindow *pwin)
{

	// else, restart Main Menu application

}

void NWMServer::windowShown(QWSWindow *pwin)
{

}


void NWMServer::windowEventHandler(QWSWindow *window, QWSServer::WindowEvent eventType)
{
	if (window->name() == "BusyHint")
		return;
	
	switch(eventType)
	{
		case QWSServer::Create:
			qDebug() << "Window Created";
			break;
		case QWSServer::Destroy:
//			windowDestroyed(window);
			break;
		case QWSServer::Hide:
//			windowHidden(window);
			break;
		case QWSServer::Show:
			qDebug() << "Window Shown";
//			windowShown(window);
			break;
		case QWSServer::Lower:
			break;
		case QWSServer::Raise:
			break;
		case QWSServer::Geometry:
			break;
		case QWSServer::Active:
			qDebug() << "Window Active";
			windowActivated(window);
			stopBusyHint();
			break;
		case QWSServer::Name:
			break;
		default:

			break;
	}
}


void NWMServer::nappchannelReceived(const QString &msg, const QByteArray &data)
{
/*
	QDataStream in(data);
	if (msg == SEND_CMD_CLOSEALL)
	{
		pman.closeall();
	} 
	else if (msg == SEND_CMD_CLOSEOTHERS)
	{
		QStringList list;
		in >> list;
		pman.closeotherapps(list);
	}
	else if (msg == SEND_CMD_RUN)
	{

		QString p;
		in >> p;
		pman.runapp(p,false);
	} 
	else if (msg == SEND_CMD_RUNARGS)
	{
		QString p;
		in >> p;
		QStringList a;
		in >> a;
		pman.runapp(p,a,false);
	}
	else if (msg == SEND_CMD_PRINTALL)
	{
		foreach (QWSWindow *window,server->clientWindows())
			qDebug() << "WinID:" << window->winId()  <<  "ClientID:" << window->client()->clientId() << window->name() << "Client Name:" << window->client()->identity();
	}
	else if(msg == NAppChannel::sendHideAppByIdMsg())
	{
		int id;
		in >> id;

		hideAllWindows(id);
	}
	else if(msg == NAppChannel::sendHideAppByNameMsg())
	{
		QString appname;
		in >> appname;

		hideAllWindows(appname);

	}
	else if(msg == NAppChannel::sendShowAppByIdMsg())
	{
		int id;
		in >> id;

		showAllWindows(id);
	}
	else if(msg == NAppChannel::sendShowAppByNameMsg())
	{
		QString appname;
		in >> appname;
		showAllWindows(appname);
	}
	else if(msg == NAppChannel::sendSwitchTVOutputModeMsg())
	{
		switchTVOutputMode(); // Shutdown all systems and start in new mode.
	}

*/
}

void NWMServer::showAllWindows(const QString& appname)
{
		foreach(QWSWindow *window,server->clientWindows())
		{
			if(window->client()->identity() == appname)
				window->show();
		}
}


void NWMServer::showAllWindows(const int id)
{
		foreach(QWSWindow *window,server->clientWindows())
		{
			if(window->client()->clientId() == id)
				window->show();
		}
}

void NWMServer::hideAllWindows(const QString& appname)
{
		foreach(QWSWindow *window,server->clientWindows())
		{
			if(window->client()->identity() == appname)
				window->hide();
		}
}

void NWMServer::hideAllWindows(const int id)
{
		foreach(QWSWindow *window,server->clientWindows())
		{
			if(window->client()->clientId() == id)
				window->hide();
		}
}

void NWMServer::startBusyHint(const QString& appName)
{
	QWSServer::instance()->closeKeyboard();

	settings->sync();
	QString newLang = settings->value("regional/language",DEFAULT_LANG_STRING).toString();
	if (newLang != currentLang)
	{
//		lang.changeLanguage();
		currentLang = newLang;
		initBusyWidget();
	}

   	if(busyWidget->isHidden())
	{
		busyWidget->setTitle (  tr("Starting application") );
		busyWidget->show();
		busyWidget->raise();
		busyWidget->setActive( true );
	}
}

void NWMServer::stopBusyHint()
{
	busyWidget->setActive( false );
	busyWidget->hide();
	busyWidget->lower();

	QWSServer::instance()->openKeyboard();
}

void NWMServer::appAlreadyRunning(const QString& app)
{

	QFileInfo file(app);
	showAllWindows(file.fileName());
	qDebug() << "ProcessHandler said application" << file.fileName() << "is already running";

}

void NWMServer::processStartupConfig(const QString& configFile)
{
	qDebug() << "Processing Configuration File" << configFile;

	QFile file(configFile);
	if(file.open(QFile::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString line = in.readLine();
		while (!line.isNull())
		{
			line.remove(QChar('\n'),Qt::CaseInsensitive);
			QFileInfo appinfo(line);
			if(appinfo.exists() && appinfo.isFile() && appinfo.isExecutable())
			{
				pman.runapp(line,true);
				qDebug() << "Autostart Application:" << line;
			}

			line = in.readLine();

		}

	file.close();
	}

	qDebug() << "Processing Configuration File Ended";

}


void NWMServer::sendCloseAllComplete()
{
//        NAppChannel::sendCloseCompleted();
}

void NWMServer::sendApplicationStarted(const QString& app, const QByteArray& data)
{
//        NAppChannel::sendApplicationStarted(app, data);
}


