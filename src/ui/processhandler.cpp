/*
 *  Copyright(C) 2007 Neuros Technology International LLC. 
 *               <www.neurostechnology.com>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 *  
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
 * NAppchannel implement routies.
 *
 * REVISION:
 * 
 * 1) Initial creation. ----------------------------------- 2007-12-12 EY
 * 2) Moved from NAppServer to ProcessHandler. ------------ 2008-01-23 Tom Bruno tebruno99@gmail.com
 * 3) Completely replaced with new handler that takes into account
 *     if process already started and emits signals. Closes apps
 *     correctly, And fixes QMap memory leak system. Usage of
 *     correct Iterators                                    2008-01-23 Tom Bruno tebruno99@gmail.com
 *
 */


#include <QtGui>
#include <QMapIterator>
#include <unistd.h>
#include "processhandler.h"
#include <QFile>
#include <QFileInfo>


ProcessHandler::ProcessHandler( QObject *parent)
: QObject(parent)
{
}

ProcessHandler::~ProcessHandler()
{

}

void ProcessHandler::application_error( QProcess::ProcessError error )
{
    if (error == QProcess::FailedToStart)
    {

        emit idle();
        QMessageBox::critical(0, tr("Error"),tr("Can't Run This Application!"), 
                              QMessageBox::Close,
                              QMessageBox::Close,
                              0);

	QMapIterator<QString,QProcess*> i(standardProcessMap);
	while(i.hasNext())
	{
		i.next();
		if(i.value()->state() == QProcess::NotRunning && (i.value()->error() == error ))
		{
			QFileInfo appFile(i.key());
			QFile file("/var/tmp/" + appFile.fileName() + ".pid");
			file.remove();

			disconnect(i.value());
			delete i.value();
			qDebug()<<"Application Errored::" << i.key() << "and has Failed to start";	
			standardProcessMap.remove(i.key());
		}
	}
    }
}

void ProcessHandler::application_finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	Q_UNUSED(exitCode);
	Q_UNUSED(exitStatus);

	QMapIterator<QString,QProcess*> i(standardProcessMap);
	while(i.hasNext())
	{
		i.next();
		if(i.value()->state() == QProcess::NotRunning && (i.value()->exitStatus() == QProcess::NormalExit || i.value()->exitStatus() == QProcess::CrashExit) )
		{
			disconnect(i.value());
			QFileInfo appFile(i.key());
			QFile file("/var/tmp/" + appFile.fileName() + ".pid");
			file.remove();
			delete i.value();
			qDebug()<<"Application::" << i.key() << "has exited";	
			standardProcessMap.remove(i.key());
		}
	}

	if (standardProcessMap.size() <=  1)
	{
		emit idle();
		emit closeAllComplete();
	}
}

void ProcessHandler::application_finished_restart( int exitCode, QProcess::ExitStatus exitStatus )
{
	Q_UNUSED(exitCode);
	Q_UNUSED(exitStatus);
	QString appname;

	QMapIterator<QString,QProcess*> i(restartProcessMap);
	while(i.hasNext())
	{
		i.next();
		if(i.value()->state() == QProcess::NotRunning && (i.value()->exitStatus() == QProcess::NormalExit || i.value()->exitStatus() == QProcess::CrashExit) )
		{

			disconnect(i.value());
			appname = i.key();
			QFileInfo appFile(appname);
			QFile file("/var/tmp/" + appFile.fileName() + ".pid");
			file.remove();
			delete i.value();
			qDebug()<<"Application::" << i.key() << "has exited";	
			restartProcessMap.remove(i.key());
			runapp(appname,true);
		}
	}
    
}

void ProcessHandler::setupconnections(QProcess *myProcess, bool restart)
{
	connect( myProcess, SIGNAL(error( QProcess::ProcessError)),this, SLOT(application_error( QProcess::ProcessError)) );
	connect( myProcess, SIGNAL(started()),this, SLOT(application_started()));
	if (restart) 
		connect( myProcess, SIGNAL(finished ( int, QProcess::ExitStatus)),this, SLOT(application_finished_restart ( int, QProcess::ExitStatus)) );
	else connect( myProcess, SIGNAL(finished ( int, QProcess::ExitStatus)),this, SLOT(application_finished ( int, QProcess::ExitStatus)) );
}

void ProcessHandler::application_started()
{
	emit applicationStarted(current_appname, QByteArray());
}

void ProcessHandler::runapp(const QString &appname,bool restart)
{
    emit busy(appname);
	current_appname = appname;
	

	qDebug()<< "Application:" << appname << " attempting to start";

	if(!isAppRunning(appname))
	{

		QProcess *myProcess = new QProcess();
		
		qDebug()<< "Application:" << appname << " attempted to start with pid" << myProcess->pid();

		QFileInfo appFile(appname);
		QFile pidFile("/var/tmp/" + appFile.fileName() + ".pid");
		qDebug() << "Pid File is:" << "/var/tmp/" + appFile.fileName() + ".pid";
		
		if(pidFile.open(QIODevice::WriteOnly | QIODevice::Text))
			qDebug() << "PID FILE OPEN SUCCESS";

		QTextStream pidout(&pidFile);
		pidout << myProcess->pid();
		pidout.flush();
		pidFile.close();

		myProcess->setProcessChannelMode(QProcess::ForwardedChannels);
		myProcess->start(appname);

		setupconnections(myProcess, restart);
		if(restart == false)
		{
			standardProcessMap.insert(appname,myProcess);
		}
		else
		{
			myProcess->waitForStarted();
			restartProcessMap.insert(appname,myProcess);
		}
	}
	else
	{
		emit idle();
		emit alreadyRunning(appname);

	}


}

void ProcessHandler::runapp(const QString &appname,const QStringList &arguments, bool restart)
{
	emit busy(appname);
	current_appname = appname;

	qDebug()<< "Application:" << appname << " attempting to start";

	if(!isAppRunning(appname))
	{
		QProcess *myProcess = new QProcess();
		
		qDebug()<< "Application:" << appname << " attempted to start with pid" << myProcess->pid();

		QFileInfo appFile(appname);
		QFile pidFile("/var/tmp/" + appFile.fileName() + ".pid");
		qDebug() << "Pid File is:" << "/var/tmp/" + appFile.fileName() + ".pid";

		if(pidFile.open(QIODevice::WriteOnly | QIODevice::Text))
			qDebug() << "PID FILE OPEN SUCCESS";
		QTextStream pidout(&pidFile);
		pidout << myProcess->pid();
		pidout.flush();
		pidFile.close();

		myProcess->setProcessChannelMode(QProcess::ForwardedChannels);
		myProcess->start(appname,arguments);

		setupconnections(myProcess, restart);
		if(restart == false)
		{
			standardProcessMap.insert(appname,myProcess);
		}
		else
		{
			myProcess->waitForStarted();
			restartProcessMap.insert(appname,myProcess);
		}
	}
	else
	{
		emit idle();
		emit alreadyRunning(appname);
	}

}

void ProcessHandler::closeotherapps(const QStringList &except)
{
	int count = 0;
	QMapIterator<QString,QProcess*> i(standardProcessMap);
	while(i.hasNext())
	{
		i.next();
       		if (!except.contains(i.key()))
       		{
		i.value()->write("quit");
		i.value()->closeWriteChannel();
			count++;
        	}
	}

	qDebug()<< "Close Applications Except:" << except << "completed and killed" << count << "processes";
	if (standardProcessMap.size() <=  1)
	{
		emit closeAllComplete();
	}
}

void ProcessHandler::closeall()
{
	int count = 0;
	QMapIterator<QString,QProcess*> i(standardProcessMap);
	while(i.hasNext())
	{
		i.next();
		i.value()->write("quit");
		i.value()->closeWriteChannel();
		count++;
	}

	qDebug()<< "Close Applications completed and killed" << count << "processes";
	if (standardProcessMap.size() <=  1)
	{
		emit closeAllComplete();
	}
}


// Needs to be fixed to wait for all to shutdown
// and to shutdown private nodes cleanly.
void ProcessHandler::shutdown()
{
        int count = 0;
        QMapIterator<QString,QProcess*> i(standardProcessMap);
        QMapIterator<QString,QProcess*> p(restartProcessMap);
        while(i.hasNext())
        {
                i.next();
                i.value()->write("quit");
                i.value()->closeWriteChannel();
                count++;
        }

	// disconnect the signal used to trigger restart of the process.
	// this terminate (fixme)

        while(p.hasNext())
        {
                p.next();
		disconnect(p.value());

                p.value()->terminate();
                count++;
        }


	// FIXME: should wait until all apps are closed.
        qDebug()<< "Close Applications completed and killed" << count << "processes";
        if (standardProcessMap.size() <=  1)
        {
                emit closeAllComplete();
        }
}


bool ProcessHandler::isAppRunning(const QString& appname)
{
	bool status = false;

	QMapIterator<QString,QProcess*> i(standardProcessMap);
	while(i.hasNext() && status == false)
	{
		i.next();
		if(i.key() == appname)
		{
// 			qDebug()<< "Standard Application Conflict:" << appname << "Key" << i.key();
			status = true;
		}
	}

	if(status != true)
	{

		QMapIterator<QString,QProcess*> rest(restartProcessMap);
		while(rest.hasNext() && status == false)
		{
			rest.next();
			if(rest.key() == appname)
			{
// 				qDebug()<< "Reset Application Conflict:" << appname << "Key" << rest.key();
				status = true;
			}
		}
	}

	return status;

}

