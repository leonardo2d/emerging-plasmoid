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

// Define our plasma Applet
class emerging : public Plasma::Applet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        emerging(QObject *parent, const QVariantList &args);
        ~emerging();

        void init();
		
	public slots:
			void updateStatus();
			void themeChanged();
			
	private:
			void clear();

    private:
        Plasma::Label label;
		Plasma::Meter currentjob;
		Plasma::Meter totaljob;
		QString currentjobname;
		int targetjobn;
		int currentjobn;
		QTimer timer;
		int wtf;
		
    QString cout;
		
		
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(emerging, emerging)
#endif
