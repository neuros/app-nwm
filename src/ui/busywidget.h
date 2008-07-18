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
 * NBusyWidget implement header.
 *
 * REVISION:
 * 
 * 1) Initial creation. ----------------------------------- 2008-02-27 GL
 *
 */

#ifndef BUSYWIDGET_H
#define BUSYWIDGET_H


#include <QtGui>

class BusyWidget : public QWidget
{
	Q_OBJECT

	public:
		BusyWidget(QWidget *parent = 0);
		QSize sizeHint() const;
		QSize minimumSizeHint() const;
		void setTitle ( const QString &str ){strTitle = str;}
		void setMsg ( const QString &str ){strMsg = str;}
		void setActive(bool active);
	
	private slots:
		void onTimer();

	protected:
		void showEvent(QShowEvent *event);
		void paintEvent(QPaintEvent *e);
		
	private:
		QRect rectFlick;
		QString strTitle;
		QString strMsg;
		QTimer* timer;
		int flickOffset;
		int direction; // 1 left to right, -1 right to left
};

#endif // BUSYWIDGET_H

