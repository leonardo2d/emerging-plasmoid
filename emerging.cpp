/***************************************************************************
 *   Copyright (C) 2011 by Leonardo Guilherme de Freitas                   *
 *   leonardo.guilherme@gmail.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

/*
 * This program is not part of the Gentoo project and it is not directly
 * managed by any member of the Gentoo Foundation, Inc
 */

#include "emerging.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <KLocale>
#include <QTimer>
#include <QFile>
#include <QGraphicsLinearLayout>

#include <plasma/svg.h>
#include <plasma/theme.h>
#include <KStandardDirs>
#include <KColorUtils>

emerging::emerging(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
    , label(this)
    , currentjob(this)
    , totaljob(this)
	, currentjobname("Nothing being emerged")
	, targetjobn(0)
	, currentjobn(0)
	, wtf(0)
{
    setBackgroundHints(DefaultBackground);
	label.setImage(KStandardDirs::locate("data", "emerging-plasmoid/glogo-small.png"));
    setHasConfigurationInterface(true);  
	connect(&timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	timer.start(1000);
	connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));
	setAspectRatioMode(Plasma::IgnoreAspectRatio);
}


emerging::~emerging() {
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
    }
}

void emerging::init()
{
    // A small demonstration of the setFailedToLaunch function
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(this);
	layout->setOrientation(Qt::Vertical); //so widgets will be stacked up/down
	
// 	label.setMinimumSize(30, 30);
// 	label.setMaximumSize(50, 50);
	label.setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	label.setOpacity(0.7f);
// 	icon.adjustSize();
	
    currentjob.setMeterType(Plasma::Meter::BarMeterHorizontal);
	currentjob.setMaximum(100);
	currentjob.setMaximumHeight(40.0f);
	currentjob.setMinimumHeight(40.0f);
	currentjob.setLabelAlignment(0, Qt::AlignJustify | Qt::AlignVCenter);
	currentjob.setLabelAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
	currentjob.setLabelAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
	
	
	totaljob.setMeterType(Plasma::Meter::BarMeterHorizontal);
	totaljob.setMaximum(100);
	totaljob.setMaximumHeight(40.0f);
	totaljob.setMinimumHeight(40.0f);
	totaljob.setLabelAlignment(0, Qt::AlignJustify | Qt::AlignVCenter);
	totaljob.setLabelAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
	totaljob.setLabelAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
	
	layout->addItem(&label);
	layout->addItem(&currentjob);
	layout->addItem(&totaljob);
	
	clear();
	resize(400, 200);
}



void emerging::updateStatus() {
	
	/* gets a file suitable for running getcurrent */
	QString tmpfn = KStandardDirs::locateLocal("tmp", "emerging-plasmoid/.tmp");
	
	/* starts building the command to run getcurrent */
	QString cmd = "perl ";
	cmd += KStandardDirs::locate("data", "emerging-plasmoid/getcurrent");
	cmd += " > " + tmpfn;
	
	/* run it */
	int status = system(cmd.toAscii().data());
	if (status != 0) return;
	
	/* now lets open the resulting file to parse */
	QFile tmpf(tmpfn);
	if (!tmpf.open(QFile::ReadOnly)) return;
	QTextStream stream(&tmpf);
	QString result = stream.readAll();
	if (result[0] == '!') {
		if (targetjobn > 0) {
			currentjob.setLabel(0, "Done!");
		}
		/* only reset the value if we reached the maximum */
		/* TODO: what if something bad happened? */
		if (currentjobn == targetjobn) {
			totaljob.setValue(0);
		}
		if (wtf > 0) {
			totaljob.setLabel(0, "Waiting " + QString::number(wtf) + "s");
			wtf--;
		} else {
			if (targetjobn > 0) clear();
		}
		update();
		return;
	}
	/* wtf is the what the fuck marker. when it reaches zero,
	 * we do reset the plasmoid */
	wtf = 10;
	QStringList v = result.split(' ');
	QString jobname = v[0];
	int targettime = v[1].toFloat();
	int starttime = v[2].toInt();
	int runningtime = v[3].toInt();
	currentjobn = v[4].toInt();
	targetjobn = v[5].toInt();
	
	/* Update for this job */
	if (currentjobname == v[0]) {
		
		currentjob.setValue(runningtime - starttime);
		float currentpercent = (runningtime - starttime)/(targettime*1.0f);
		if (currentpercent > 1.0f) currentpercent = 1.0f;
		
		QTime time(0, 0, 0);
		time = time.addSecs(targettime - targettime*currentpercent);
		
		currentjob.setLabel(1, QString("%1").arg((int)(currentpercent * 100)) + "%");
		currentjob.setLabel(2, time.toString());
		float percent = ((currentjobn-1) + currentpercent)/(targetjobn*1.0f)*100;
		totaljob.setValue(percent);
		totaljob.setLabel(0, "Emerging...");
		totaljob.setLabel(1, QString::number((int)percent) + "%");
		totaljob.setLabel(2, QString::number(currentjobn) + "/" + QString::number(targetjobn));
// 		qDebug() << jobname << targettime << starttime << runningtime << targetjobn << currentjobn << currentpercent << percent;	
		update();
	} else {
		currentjobname = jobname;
// 		currentjob.setLabel(1, jobname);
		currentjob.setToolTip(jobname);
		currentjob.setMaximum(targettime);
		currentjob.setLabel(0, jobname);
		totaljob.setMaximum(100);
		currentjob.setValue(runningtime - starttime);
		update();
	}
	
}

void emerging::themeChanged() {
	Plasma::Theme* theme = Plasma::Theme::defaultTheme();
	QColor text = theme->color(Plasma::Theme::TextColor);
	QColor bg = theme->color(Plasma::Theme::BackgroundColor);
	QColor darkerText = KColorUtils::tint(text, bg, 0.4);
	
	totaljob.setLabelColor(0, text);
	totaljob.setLabelColor(1, darkerText);
	
	currentjob.setLabelColor(0, text);
	currentjob.setLabelColor(1, darkerText);
	
	QFont font = theme->font(Plasma::Theme::DefaultFont);
	font.setPointSizeF(9.0);
	totaljob.setLabelFont(0, font);
	currentjob.setLabelFont(0, font);
	font.setPointSizeF(7.5);
	totaljob.setLabelFont(1, font);
	currentjob.setLabelFont(1, font);
}

void emerging::clear() {
	themeChanged();
	currentjobn = 0;
	targetjobn = 0;
	currentjobname = "Nothing being emerged";
	currentjob.setValue(0);
	currentjob.setLabel(0, currentjobname);
	currentjob.setLabel(1, "");
	currentjob.setLabel(2, "");
	totaljob.setLabel(0, "Done!");
	totaljob.setLabel(1, "");
	totaljob.setLabel(2, "");
}


#include "emerging.moc"
