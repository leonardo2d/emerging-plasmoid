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

#include "emergingplasmoid.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <KLocale>
#include <QTimer>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QProcess>

#include <plasma/svg.h>
#include <plasma/theme.h>
#include <KStandardDirs>
#include <KColorUtils>

EmergingPlasmoid::EmergingPlasmoid(QObject * parent, const QVariantList & args)
  : Plasma::Applet(parent, args)
  , gentooLogoLabel(this)
  , currentJobMeter(this)
  , totalJobMeter(this)
  , currentJobName("Nothing being emerged")
  , targetJobCount(0)
  , currentJobCount(0)
  , secondsUntilGiveUp(0) {
  setBackgroundHints(DefaultBackground);
  gentooLogoLabel.setImage(KStandardDirs::locate("data", "emerging-plasmoid/glogo-small.png"));
  setHasConfigurationInterface(true);
  connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
  updateTimer.start(1000); // every 1sec
  connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));
  setAspectRatioMode(Plasma::IgnoreAspectRatio);
}


EmergingPlasmoid::~EmergingPlasmoid() {
}

void EmergingPlasmoid::init() {
  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout(this);
  layout->setOrientation(Qt::Vertical); //so widgets will be stacked up/down

  gentooLogoLabel.setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
  gentooLogoLabel.setOpacity(0.7f);

  currentJobMeter.setMeterType(Plasma::Meter::BarMeterHorizontal);
  currentJobMeter.setMaximum(100);
  currentJobMeter.setMaximumHeight(40.0f);
  currentJobMeter.setMinimumHeight(40.0f);
  currentJobMeter.setLabelAlignment(0, Qt::AlignJustify | Qt::AlignVCenter);
  currentJobMeter.setLabelAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
  currentJobMeter.setLabelAlignment(2, Qt::AlignRight | Qt::AlignVCenter);


  totalJobMeter.setMeterType(Plasma::Meter::BarMeterHorizontal);
  totalJobMeter.setMaximum(100);
  totalJobMeter.setMaximumHeight(40.0f);
  totalJobMeter.setMinimumHeight(40.0f);
  totalJobMeter.setLabelAlignment(0, Qt::AlignJustify | Qt::AlignVCenter);
  totalJobMeter.setLabelAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
  totalJobMeter.setLabelAlignment(2, Qt::AlignRight | Qt::AlignVCenter);

  layout->addItem(&gentooLogoLabel);
  layout->addItem(&currentJobMeter);
  layout->addItem(&totalJobMeter);

  clear();
}



void EmergingPlasmoid::updateStatus() {

  /* Get the getcurrent program */
  QString getcurrentPath = KStandardDirs::locate("data", "emerging-plasmoid/getcurrent");

  /* Run perl with getcurrent */
  QProcess proc(this);
  proc.start("perl", QStringList() << getcurrentPath);
  proc.waitForFinished();
  QString result = proc.readAllStandardOutput();

  /* Parse stdout from getcurrent */
  if (result[0] == '!') {
    /* ! means nothing is running */
    if (targetJobCount > 0) {
      /* targetJobCount > 0 means we have more work to do */
      currentJobMeter.setLabel(0, "Done!");
    }

    /* if we think we ran all the jobs, reset meter state */
    if (currentJobCount == targetJobCount) {
      totalJobMeter.setValue(0);
    }

    if (secondsUntilGiveUp > 0) {
      /* Wait 'secondsUntilGiveUp' seconds until getcurrent says theres something else running */
      totalJobMeter.setLabel(0, "Waiting " + QString::number(secondsUntilGiveUp) + "s");
      secondsUntilGiveUp--;
    } else {
      /* time-out while running emerge job, something went wrong with the emerge process */
      if (targetJobCount > 0) clear();
    }

    /* update and return because nothing is running */
    update();
    return;
  }

  /* Actual update if there is something running */
  secondsUntilGiveUp = 10;
  QStringList v = result.split(' ');
  QString jobname = v[0];
  int targettime = v[1].toFloat();
  int starttime = v[2].toInt();
  int runningtime = v[3].toInt();
  currentJobCount = v[4].toInt();
  targetJobCount = v[5].toInt();

  /* Update for this job */
  if (currentJobName == v[0]) {

    currentJobMeter.setValue(runningtime - starttime);
    float currentpercent = (runningtime - starttime) / (targettime * 1.0f);
    if (currentpercent > 1.0f) currentpercent = 1.0f;

    QTime time(0, 0, 0);
    time = time.addSecs(targettime - targettime * currentpercent);

    currentJobMeter.setLabel(1, QString("%1").arg((int)(currentpercent * 100)) + "%");
    currentJobMeter.setLabel(2, time.toString());
    float percent = ((currentJobCount - 1) + currentpercent) / (targetJobCount * 1.0f) * 100;
    totalJobMeter.setValue(percent);
    totalJobMeter.setLabel(0, "Emerging...");
    totalJobMeter.setLabel(1, QString::number((int)percent) + "%");
    totalJobMeter.setLabel(2, QString::number(currentJobCount) + "/" + QString::number(targetJobCount));
  } else {
    /* job changed */
    currentJobName = jobname;
    currentJobMeter.setToolTip(jobname);
    currentJobMeter.setMaximum(targettime);
    currentJobMeter.setLabel(0, jobname);
    totalJobMeter.setMaximum(100);
    currentJobMeter.setValue(runningtime - starttime);
  }

  update();
}

void EmergingPlasmoid::themeChanged() {
  Plasma::Theme * theme = Plasma::Theme::defaultTheme();
  QColor text = theme->color(Plasma::Theme::TextColor);
  QColor bg = theme->color(Plasma::Theme::BackgroundColor);
  QColor darkerText = KColorUtils::tint(text, bg, 0.4);

  totalJobMeter.setLabelColor(0, text);
  totalJobMeter.setLabelColor(1, darkerText);

  currentJobMeter.setLabelColor(0, text);
  currentJobMeter.setLabelColor(1, darkerText);

  QFont font = theme->font(Plasma::Theme::DefaultFont);
  font.setPointSizeF(9.0);
  totalJobMeter.setLabelFont(0, font);
  currentJobMeter.setLabelFont(0, font);
  font.setPointSizeF(7.5);
  totalJobMeter.setLabelFont(1, font);
  currentJobMeter.setLabelFont(1, font);
}

void EmergingPlasmoid::clear() {
  themeChanged();
  currentJobCount = 0;
  targetJobCount = 0;
  currentJobName = "Nothing being emerged";
  currentJobMeter.setValue(0);
  currentJobMeter.setLabel(0, currentJobName);
  currentJobMeter.setLabel(1, "");
  currentJobMeter.setLabel(2, "");
  totalJobMeter.setLabel(0, "Done!");
  totalJobMeter.setLabel(1, "");
  totalJobMeter.setLabel(2, "");
}


#include "emergingplasmoid.moc"
