/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
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
 ***************************************************************************/

#ifndef COMMON_DBUS_DEFINES_H
#define COMMON_DBUS_DEFINES_H

#define COMMON_DBUS_INTERFACE_PROPERTIES_STRING                  "org.freedesktop.DBus.Properties"

#define COMMON_DBUS_METHOD_PROPERTIES_GET_STRING                 "Get"
#define COMMON_DBUS_METHOD_PROPERTIES_GET_IN_SIGNATURE_STRING    "s" // property name

#if defined (SD_BUS_SUPPORT)
#define COMMON_SD_BUS_SERVICE_STRING                             "org.freedesktop.systemd1"

#define COMMON_SD_BUS_OBJECT_PATH_STRING                         "/org/freedesktop/systemd1"

#define COMMON_SD_BUS_INTERFACE_MANAGER_STRING                   "org.freedesktop.systemd1.Manager"
#define COMMON_SD_BUS_INTERFACE_UNIT_STRING                      "org.freedesktop.systemd1.Unit"

#define COMMON_SD_BUS_METHOD_UNIT_GET_STRING                     "GetUnit"
#define COMMON_SD_BUS_METHOD_UNIT_GET_IN_SIGNATURE_STRING        "s" // unit name
#define COMMON_SD_BUS_METHOD_UNIT_GET_OUT_SIGNATURE_STRING       "o" // unit object path

#define COMMON_SD_BUS_METHOD_UNIT_START_STRING                   "StartUnit"
#define COMMON_SD_BUS_METHOD_UNIT_START_IN_SIGNATURE_STRING      "ss" // unit name/mode
#define COMMON_SD_BUS_METHOD_UNIT_START_OUT_SIGNATURE_STRING     "o" // job object path

#define COMMON_SD_BUS_METHOD_UNIT_STOP_STRING                    "StopUnit"
#define COMMON_SD_BUS_METHOD_UNIT_STOP_IN_SIGNATURE_STRING       "ss" // unit name/mode
#define COMMON_SD_BUS_METHOD_UNIT_STOP_OUT_SIGNATURE_STRING      "o" // job object path
// *NOTE*: "...The mode needs to be one of replace, fail, isolate,
//         ignore-dependencies, ignore-requirements. ..." (see also:
//         https://www.freedesktop.org/wiki/Software/systemd/dbus)"
#define COMMON_SD_BUS_METHOD_UNIT_MODE_REPLACE_STRING            "replace"

#define COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_STRING           "ActiveState"
#define COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_SIGNATURE_STRING "s" // property type
#define COMMON_SD_BUS_PROPERTY_UNIT_ACTIVESTATE_ACTIVE_STRING    "active"
#define COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_STRING              "SubState"
#define COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_SIGNATURE_STRING    "s" // property type
#define COMMON_SD_BUS_PROPERTY_UNIT_SUBSTATE_RUNNING_STRING      "running"

#define COMMON_SD_BUS_FILTER_SIGNAL_MANAGER_STRING               "type='signal',sender='org.freedesktop.systemd1.Manager'"

#define COMMON_SD_BUS_PROPERTY_JOB_ID_STRING                     "Id"
#define COMMON_SD_BUS_PROPERTY_JOB_ID_SIGNATURE_STRING           "u" // property type
#define COMMON_SD_BUS_PROPERTY_JOB_RESULT_DONE_STRING            "done"

#define COMMON_SD_BUS_SIGNAL_JOB_REMOVED_SIGNATURE_STRING        "uoss" // id,object,unit,result

#endif // SD_BUS_SUPPORT

#endif
