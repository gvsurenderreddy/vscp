// devicethread.h
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
// 
// This file is part of the VSCP (http://www.vscp.org) 
//
// Copyright (C) 2000-2016 
// Ake Hedman, Grodans Paradis AB, <akhe@grodansparadis.com>
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

#if !defined(DEVICETHREAD_H__7D80016B_5EFD_40D5_94E3_6FD9C324CC7B__INCLUDED_)
#define DEVICETHREAD_H__7D80016B_5EFD_40D5_94E3_6FD9C324CC7B__INCLUDED_

#ifdef WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#endif

#ifdef WIN32
#include <winsock2.h>
#endif

#include "wx/wx.h"
#include <wx/thread.h>
#include <wx/socket.h>
#include <wx/dynlib.h>

class CControlObject;
class CDeviceItem;
class deviceThread;

/*!
    This class implement a thread that write data
    to a blocking CANAL driver
*/

class deviceCanalWriteThread : public wxThread
{

public:
    
    /// Constructor
    deviceCanalWriteThread();

    /// Destructor
    virtual ~deviceCanalWriteThread();

    /*!
        Thread code entry point
    */
    virtual void *Entry();

    /*! 
        called when the thread exits - whether it terminates normally or is
        stopped with Delete() (but not when it is Kill()ed!)
    */
    virtual void OnExit();

    /*!
        Pointer to master thread.
    */
    deviceThread *m_pMainThreadObj;

  /*!
        Termination control
  */
  bool m_bQuit;


};


/*!
    This class implement a thread that read data
    from a blocking CANAL driver
*/

class deviceCanalReceiveThread : public wxThread
{

public:

    /// Constructor
    deviceCanalReceiveThread();

    /// Destructor
    virtual ~deviceCanalReceiveThread();

    /*!
        Thread code entry point
    */
    virtual void *Entry();


    /*! 
        called when the thread exits - whether it terminates normally or is
        stopped with Delete() (but not when it is Kill()ed!)
    */
    virtual void OnExit();

    /*!
        Pointer to master thread.
    */
    deviceThread *m_pMainThreadObj;

  /*!
        Termination control
  */
  bool m_bQuit;

};


/*!
    This class implement a thread that write data
    to a blocking VSCP Level II driver
*/

class deviceLevel2WriteThread : public wxThread
{

public:

    /// Constructor
    deviceLevel2WriteThread();

    /// Destructor
    virtual ~deviceLevel2WriteThread();

    /*!
        Thread code entry point
    */
    virtual void *Entry();

    /*! 
        called when the thread exits - whether it terminates normally or is
        stopped with Delete() (but not when it is Kill()ed!)
    */
    virtual void OnExit();

    /*!
        Pointer to master thread.
    */
    deviceThread *m_pMainThreadObj;

  /*!
    Termination control
  */
  bool m_bQuit;


};


/*!
    This class implement a thread that read data
    from a blocking VSCP Level II driver
*/

class deviceLevel2ReceiveThread : public wxThread
{

public:
    
    /// Constructor
    deviceLevel2ReceiveThread();

    /// Destructor
    virtual ~deviceLevel2ReceiveThread();

    /*!
        Thread code entry point
    */
    virtual void *Entry();


    /*! 
        called when the thread exits - whether it terminates normally or is
        stopped with Delete() (but not when it is Kill()ed!)
    */
    virtual void OnExit();

    /*!
        Pointer to master thread.
    */
    deviceThread *m_pMainThreadObj;

  /*!
        Termination control
  */
  bool m_bQuit;

};


/*!
    This class implement a one of thread that look
    for specific events and react on them appropriately .
*/

class deviceThread : public wxThread
{

public:

    /// Constructor
    deviceThread();

    /// Destructor
    virtual ~deviceThread();

    /*!
        Thread code entry point
    */
    virtual void *Entry();


    /*! 
        called when the thread exits - whether it terminates normally or is
        stopped with Delete() (but not when it is Kill()ed!)
    */
    virtual void OnExit();


    /// dl/dll handler
    wxDynamicLibrary m_wxdll;

    /*!
        DeviceItem for device to control	
    */
    CDeviceItem *m_pDeviceItem;

    /*!
        Control object that invoked thread
    */
    CControlObject *m_pCtrlObject;

    /*!
        Holder for CANAL receive thread
    */
    deviceCanalReceiveThread *m_preceiveThread;

    /*!
        Holder for CANAL write thread
    */
    deviceCanalWriteThread *m_pwriteThread;
    
    
    /*!
        Holder for VSCP Level II receive thread
    */
    deviceLevel2ReceiveThread *m_preceiveLevel2Thread;

    /*!
        Holder for VSCP Level II write thread
    */
    deviceLevel2WriteThread *m_pwriteLevel2Thread;

    /*!
        Check filter/mask to see if filter should be delivered

        The filter have the following matrix

        mask bit n	|	filter bit n	| msg id bit	|	result
        ===========================================================
            0               X                   X           Accept
            1               0                   0           Accept
            1               0                   1           Reject
            1               1                   0           Reject
            1               1                   1           Accept

            Formula is !( ( filter �d ) & mask )

        @param pclientItem Pointer to client item
        @param pcanalMsg Pointer to can message
        @return True if message is accepted false if rejected
        TODO
    */
        
};




#endif



