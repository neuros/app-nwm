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
 * BusyWidget implement
 *
 * REVISION:
 * 
 * 1) Initial creation. ----------------------------------- 2008-02-27 GL
 *
 *
 */


#include "busywidget.h"
#define BW_HEIGHT    156  //busy widget height
#define EG_WIDTH     4    //edge width
#define MARGIN       20   //the margin of the text
#define FONT_MSG     22
#define FONT_TITLE   24
#define COLOR_MSG    QColor(178,178,178)
#define FLICK_COLOR  QColor(42, 170, 225)
#define COLOR_TITLE  Qt::white
#define FLICK_WIDTH  21
#define FLICK_HEIGHT 7
#define TIMER_INTERVAL (400)  //ms


BusyWidget::BusyWidget(QWidget *parent)
: QWidget(parent)
{
	flickOffset = 0;
	direction = 1;

	timer = new QTimer( this );
	timer -> setInterval( TIMER_INTERVAL );

	connect( timer, SIGNAL( timeout( ) ), this, SLOT( onTimer( ) ) );
}


void BusyWidget::showEvent(QShowEvent *event)
{
	QPixmap bgtile(":/boxes/Busy-exec_Tile.png");
	QPixmap bgedge(":/boxes/Busy-exec_Edge.png");
	QPixmap pixmap(size());
	QPainter painter(&pixmap);
	QRect rct(0, EG_WIDTH, width(), height() - EG_WIDTH*2);
	
	painter.drawTiledPixmap(rct, bgtile, QPoint(0, 0));
	painter.drawPixmap(0, EG_WIDTH, bgedge);
	painter.drawPixmap(0, height() - EG_WIDTH, bgedge);

	QPalette p = palette();
	QBrush	 b = QBrush(pixmap);
	p.setBrush(backgroundRole(), b);
	setPalette( p );
	setAutoFillBackground(true);
	
	rectFlick.setX( (width() - FLICK_WIDTH)/2 );
	rectFlick.setY( height()/2 - FLICK_HEIGHT*2 ); 
	rectFlick.setSize( QSize(FLICK_WIDTH ,FLICK_HEIGHT) );
}

void BusyWidget::onTimer()
{
	if(flickOffset == 0) 
		direction = 1;
	if(flickOffset >= FLICK_HEIGHT*2)
		direction = -1;
	flickOffset += direction * FLICK_HEIGHT;
	update(rectFlick);	
}

void BusyWidget::setActive(bool active)
{
	if(active)
		timer->start( );
	else
		timer->stop( );
}



void BusyWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QBrush brush;
	QRect ret = rectFlick;
	painter.fillRect(ret,COLOR_MSG);
	
	ret.setWidth(FLICK_HEIGHT);
	ret.translate(flickOffset,0);
	painter.fillRect(ret,FLICK_COLOR);
	
	if( event->rect() == rectFlick )
		return;

	//draw title
	QFont font = painter.font();
	if ( !strTitle.isEmpty())
	{
		font.setPointSize(FONT_TITLE);
		painter.setFont(font);
		painter.setPen(COLOR_TITLE);
		QRect retTitle(MARGIN, MARGIN, width() - MARGIN*2, (height() - MARGIN)/4);
		painter.drawText(retTitle, Qt::AlignCenter | Qt::AlignVCenter, strTitle);
	}

	//draw message quip
	font.setPointSize(FONT_MSG);
	painter.setFont(font);
	painter.setPen(COLOR_MSG);
	QRect retMsg(MARGIN, (height())/2, width() - MARGIN*2, (height() - MARGIN)/2);

	if ( !strMsg.isEmpty())
	{
		painter.drawText(retMsg, Qt::AlignCenter | Qt::AlignVCenter, strMsg);
	}
	else
	{
		painter.drawText(retMsg, Qt::AlignCenter | Qt::AlignVCenter,
			tr("One moment please ..."));
	}

}

QSize BusyWidget::sizeHint() const
{
	QSize psize = QApplication::desktop()->size();
	psize.setHeight(BW_HEIGHT);
	return psize;
}


QSize BusyWidget::minimumSizeHint() const
{
	QSize psize = QApplication::desktop()->size();
	psize.setHeight(BW_HEIGHT);
	return psize;
}


