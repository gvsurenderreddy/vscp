// canallogger.cpp : Defines the initialization routines for the DLL.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
// 
// This file is part of the VSCP (http://www.vscp.org) 
//
// Copyright (C) 2000-2013 Ake Hedman, 
//      eurosource, <akhe@eurosource.se>
// 
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this file see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
//
// Linux
// =====
// device1 = logger,/tmp/canal_log,txt,/usr/local/lib/canallogger.so,64,64,1
//
// WIN32
// =====
// device1 = logger,c:\canal_log,txt,d:\winnr\system32\canallogger.dll,64,64,1

#include "vscpl2logger.h"
#include "stdio.h"
#include "stdlib.h"


void _init() __attribute__((constructor));
void _fini() __attribute__((destructor));

void _init() {
    printf("initializing\n");
}

void _fini() {
    printf("finishing\n");
}


//CLoggerdllApp *gtheapp;

/////////////////////////////////////////////////////////////////////////////
// CLoggerdllApp

////////////////////////////////////////////////////////////////////////////
// CLoggerdllApp construction

CLoggerdllApp::CLoggerdllApp() {
    m_instanceCounter = 0;
    pthread_mutex_init(&m_objMutex, NULL);

    // Init the driver array
    for (int i = 0; i < CANAL_LOGGER_DRIVER_MAX_OPEN; i++) {
        m_logArray[ i ] = NULL;
    }

    UNLOCK_MUTEX(m_objMutex);
}

CLoggerdllApp::~CLoggerdllApp() {
    LOCK_MUTEX(m_objMutex);

    for (int i = 0; i < CANAL_LOGGER_DRIVER_MAX_OPEN; i++) {

        if (NULL == m_logArray[ i ]) {

            CVSCPLog *pLog = getDriverObject(i);
            if (NULL != pLog) {
                pLog->close();
                delete m_logArray[ i ];
                m_logArray[ i ] = NULL;
            }
        }
    }

    UNLOCK_MUTEX(m_objMutex);
    pthread_mutex_destroy(&m_objMutex);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLoggerdllApp object

CLoggerdllApp theApp;

///////////////////////////////////////////////////////////////////////////////
// CreateObject

//extern "C" CLoggerdllApp* CreateObject( void ) {
//	CLoggerdllApp *theapp = new CLoggerdllApp;
//	return ( ( CLoggerdllApp * ) theapp );
//}

///////////////////////////////////////////////////////////////////////////////
// addDriverObject
//

long CLoggerdllApp::addDriverObject( CVSCPLog *plog )
{
	long h = 0;

	LOCK_MUTEX( m_objMutex );
	for ( int i=0; i<CANAL_LOGGER_DRIVER_MAX_OPEN; i++ ) {
	
		if ( NULL == m_logArray[ i ] ) {
		
			m_logArray[ i ] = plog;	
			h = i + 1681; 
			break;

		}

	}

	UNLOCK_MUTEX( m_objMutex );

	return h;
}


///////////////////////////////////////////////////////////////////////////////
// getDriverObject
//

CVSCPLog * CLoggerdllApp::getDriverObject( long h )
{
	long idx = h - 1681;

	// Check if valid handle
	if ( idx < 0 ) return NULL;
	if ( idx >= CANAL_LOGGER_DRIVER_MAX_OPEN ) return NULL;
	return m_logArray[ idx ];
}


///////////////////////////////////////////////////////////////////////////////
// removeDriverObject
//

void CLoggerdllApp::removeDriverObject( long h )
{
	long idx = h - 1681;

	// Check if valid handle
	if ( idx < 0 ) return;
	if ( idx >= CANAL_LOGGER_DRIVER_MAX_OPEN ) return;

	LOCK_MUTEX( m_objMutex );
	if ( NULL != m_logArray[ idx ] ) delete m_logArray[ idx ];
	m_logArray[ idx ] = NULL;
	UNLOCK_MUTEX( m_objMutex );
}

///////////////////////////////////////////////////////////////////////////////
// InitInstance

BOOL CLoggerdllApp::InitInstance() 
{
	m_instanceCounter++;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//                         V S C P   D R I V E R -  A P I
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// VSCPOpen
//

extern "C" long VSCPOpen(const char *pUsername,
        const char *pPassword,
        const char *pHost,
        short port,
        const char *pPrefix,
        const char *pParameter,
        unsigned long flags) {
    long h = 0;

    CVSCPLog *pdrvObj = new CVSCPLog();
    if (NULL != pdrvObj) {

        if (pdrvObj->open(pUsername, pPassword, pHost, port, pPrefix, pParameter, flags)) {

            if (!(h = theApp.addDriverObject(pdrvObj))) {
                delete pdrvObj;
            }

        } else {
            delete pdrvObj;
        }

    }

    return h;
}


///////////////////////////////////////////////////////////////////////////////
//  VSCPClose
// 

extern "C" int VSCPClose(long handle) {
    int rv = 0;

    CVSCPLog *pdrvObj = theApp.getDriverObject(handle);
    if (NULL == pdrvObj) return 0;
    pdrvObj->close();
    theApp.removeDriverObject(handle);
    rv = 1;
    return CANAL_ERROR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//  VSCPGetLevel
// 

extern "C" unsigned long VSCPGetLevel(void) {
    return CANAL_LEVEL_USES_TCPIP;
}


///////////////////////////////////////////////////////////////////////////////
// VSCPGetDllVersion
//

extern "C" unsigned long VSCPGetDllVersion(void) {
    return VSCP_DLL_VERSION;
}


///////////////////////////////////////////////////////////////////////////////
// VSCPGetVendorString
//

extern "C" const char * VSCPGetVendorString(void) {
    return VSCP_DLL_VENDOR;
}


///////////////////////////////////////////////////////////////////////////////
// VSCPGetDriverInfo
//

extern "C" const char * VSCPGetDriverInfo(void) {
    return VSCP_LOGGER_DRIVERINFO;
}