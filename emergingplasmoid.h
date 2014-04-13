/***************************************************************************
*   Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>                            *
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

// Here we avoid loading the header multiple times
#ifndef EMERGING_HEADER
#define EMERGING_HEADER

#include <KIcon>
// We need the Plasma Applet headers
#include <Plasma/Applet>
#include <Plasma/Svg>
#include <Plasma/Meter>
#include <Plasma/Label>
#include <QTimer>

class QSizeF;
class QWidget;
class QGraphicsGridLayout;

#include "ui_config.h"

/* EmerginPlasmoid shows information about the current emerging process:
 * - Total packages to build
 * - Current package progress
 * - Nice gentoo logo :)
 * 
 * It is based on genlop (a modified version of it, that is). */
class EmergingPlasmoid : public Plasma::Applet {
  
  enum LogoPosition {
    LogoHidden,
    LogoInTop,
    LogoInBottom,
    LogoInLeft,
    LogoInRight
  };
  
  Q_OBJECT
  public:
    /* Constructs an instance of the emerging plasmoid */
    EmergingPlasmoid(QObject * parent, const QVariantList & args);
    ~EmergingPlasmoid();

    /* Called when plasmoid gets initialized */
    void init();
    
    /* Called when we should create the config interface */
    virtual void createConfigurationInterface(KConfigDialog * parent);

  public slots:
    void updateStatus(); /* called every second */
    void themeChanged();
    void configAccepted();
    
  private:
    void clear();
    void setupLayout();

  private:
    /* Ui stuff */
    Plasma::Label gentooLogoLabel; /* the logo label */
    Plasma::Meter currentJobMeter; /* meter for current job completeness */
    Plasma::Meter totalJobMeter;   /* meter for total jobs completeness */
    QGraphicsGridLayout * layout;  /* the grid layout used */
    
    /* stuff from getcurrent */
    QString currentJobName;        /* current package name */
    int targetJobCount;            /* how many emerge jobs there is */
    int currentJobCount;           /* which job is the current one */
    
    /* time stuff */
    int secondsUntilGiveUp;        /* how many seconds until we give up */
    QTimer updateTimer;            /* updateStatus() timer */
    
    /* the config dialog */
    Ui::ConfigWidget configWidget;
    KConfigDialog * configDialog;
    
    /* config stuff */
    int timeout;
    LogoPosition logoPosition;
    
  private slots:
    void configModified();
};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(emerging, EmergingPlasmoid)
#endif
