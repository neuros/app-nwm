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
 * NAppchannel implement header.
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


#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <QCopChannel>
#include <QtGlobal>
#include <QProcess>
#include <QMap>
#include <QtGui>

class ProcessHandler : public QObject
{
    Q_OBJECT

    public:
	ProcessHandler(QObject *parent=0);
	~ProcessHandler();
	bool isAppRunning(const QString&);

public slots:

	void runapp(const QString &appname,const QStringList &arguments, bool restart);
	void runapp(const QString &appname, bool restart);
	void closeall();
	void closeotherapps(const QStringList &except);
	void shutdown();
	
private slots:
	void application_error( QProcess::ProcessError error );
	void application_finished( int exitCode, QProcess::ExitStatus exitStatus );
	void application_finished_restart( int exitCode, QProcess::ExitStatus exitStatus );
	void application_started();

private:
	QMap<QString,QProcess *> restartProcessMap;
	QMap<QString,QProcess *> standardProcessMap;

	void setupconnections(QProcess *myProcess, bool restart);
	QString current_appname;

signals:
	void busy(const QString &);
	void idle();
	void alreadyRunning(const QString&);
	void closeAllComplete();
	void applicationStarted(const QString&,  const QByteArray&);

};
#endif // NAPPCHANNEL_H
