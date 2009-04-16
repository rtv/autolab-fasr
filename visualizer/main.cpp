/***************************************************************************
 *   Copyright (C) 2009 by Jens
 *   jwawerla@sfu.ca
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 * $Log: main.cpp,v $
 * Revision 1.2  2009-03-23 06:24:22  jwawerla
 * RobotVis works now for data, maps still need more work
 *
 * Revision 1.1  2009-03-16 14:27:19  jwawerla
 * robots still get stuck
 *
 *
 *
 ***************************************************************************/

#include <QApplication>
#include <QtCore>
#include "mainwindow.h"
#include "cmdlineparser.h"
#include <QtDBus/QtDBus>

//-----------------------------------------------------------------------------
int main ( int argc, char* argv[] )
{
  char hostname[100];
  int port = 7000;

  strcpy ( hostname, "localhost" );

  CCmdLineParser cmdLineParser ( "RobotVis Client", false );

  cmdLineParser.registerArgument ( &hostname, CCmdLineParser::STRING, "-h", 1,
                                   "hostname" );
  cmdLineParser.registerArgument ( &port, CCmdLineParser::INT, "-p", 1, "port" );
  cmdLineParser.parse ( argc, argv );

  QApplication qtApp ( argc, argv );
  CMainWindow mainWindow(hostname, port);

  mainWindow.show();
  return qtApp.exec();
}
//-----------------------------------------------------------------------------

