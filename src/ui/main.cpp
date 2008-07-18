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
 * Neuros Window Manager application entry point.
 *
 * REVISION:
 *
 * 2) Split the server off into a class ------------------- 2008-01-16 nerochiaro
 * 1) Initial creation. ----------------------------------- 2007-10 tebruno99@gmail.com
 *
 */

#include <QApplication>
#include "nwmserver.h"

int main(int argc,char *argv[])
{
	QApplication app(argc, argv, QApplication::GuiServer);
	app.setQuitOnLastWindowClosed(false);

	QCoreApplication::setOrganizationName("Neuros");
	QCoreApplication::setOrganizationDomain("neurostechnology.com");
	QCoreApplication::setApplicationName("nwm");

	NWMServer windowmanager;
	windowmanager.init();

	QApplication::processEvents();
	return app.exec();
}

