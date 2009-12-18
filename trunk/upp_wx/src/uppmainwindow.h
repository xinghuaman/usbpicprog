/***************************************************************************
*   Copyright (C) 2008 by Frans Schreuder                                 *
*   usbpicprog.sourceforge.net                                            *
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

#ifndef UPPMAINWINDOW_H
#define UPPMAINWINDOW_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/docview.h>
#include <wx/thread.h>
#include <wx/progdlg.h>
#include <wx/log.h>
#include <wx/aboutdlg.h>

#include <iostream>
using namespace std;

#include "uppmainwindow_base.h"
#include "hardware.h"
#include "pictype.h"
#include "hexfile.h"
#include "preferences.h"

#ifdef __WXMAC__
    #define EVENT_FIX
#else //__WXMAC__
    #define EVENT_FIX event.Skip();
#endif //__WXMAC__


/// More readable names for the various fields of the status bar
enum UppMainWindowStatusBar
{
    STATUS_FIELD_MENU,
    STATUS_FIELD_OTHER = STATUS_FIELD_MENU,
    STATUS_FIELD_HARDWARE
};

/// Defines the order in which the pages related to the loaded HEX file are shown
enum UppMainWindowNotebookPages
{
    PAGE_CODE,
    PAGE_DATA
};

/// When UppMainWindow starts a new operation, to avoid blocking the GUI
/// it uses a secondary thread; the values of this enumeration specify
/// the task the secondary thread performs.
enum UppMainWindowThreadMode
{
    THREAD_PROGRAM,
    THREAD_READ,
    THREAD_VERIFY,
    THREAD_ERASE,
    THREAD_BLANKCHECK
};

typedef std::vector<time_t> wxArrayTime;

/// The prefix used with the messages passed to the wxProgressDialog.
#define PROGRESS_MESSAGE_PREFIX    (wxString::FromUTF8("\xC2\xBB "))


/**
    The main frame class of the application.

    This class derives from the UppMainWindowBase class which is automatically
    generated by wxFormBuilder.

    This class derives from wxThreadHelper since it's used to manage two threads:
    the primary thread is used to handle all GUI events; the secondary thread
    is used to do long operations (like read/program/erase/verify, etc) without
    blocking the GUI.
*/
class UppMainWindow : public UppMainWindowBase, public wxThreadHelper
{
public:
    UppMainWindow(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~UppMainWindow();

    /** Open a hexfile by filename */
    bool upp_open_file(const wxString& path);

    /** Clear the hexfile and updates the GUI */
    void upp_new();

    /** Update the progress bar; this callback function is called by the Hardware class */
    void updateProgress(int value);

    /** 
        The user touched one of the code/data grids or one of the config values.
        This callback function is called by UppHexViewGrid and by UppConfigViewBook.
    */
    void upp_hex_changed();

protected:      // internal helpers

    /** Transfers data from the m_hexFile and m_picType members to the various GUI controls */
    void UpdatePicInfo();

    /** Updates the title of this frame */
    void UpdateTitle();

    /** Resets the m_hexFile member and then calls UpdatePicInfo() to update the GUI */
    void Reset();

    /** Returns a bitmap suitable for UppMainWindow menu items */
    wxBitmap GetMenuBitmap(const char* xpm_data[]);

    /** Completes UppMainWindow GUI creation started by wxFormBuilder-generated code */
    void CompleteGUICreation();

    /** Returns a pointer to the grid currently open or NULL */
    UppHexViewGrid* GetCurrentGrid() const;

    /** 
        In case the user modified some of the code/config/data memory of m_hexFile,
        warns him that he's going to loose his modification if he doesn't save.

        Call this function whenever m_hexFile is going to be modified.
    */
    bool ShouldContinueIfUnsaved();


protected:      // internal thread-related functions

    // functions which are executed in the secondary thread context:
    wxThread::ExitCode Entry();
    void LogFromThread(wxLogLevel level, const wxString& str);
    bool upp_thread_program();
    bool upp_thread_read();
    bool upp_thread_verify();
    bool upp_thread_erase();
    bool upp_thread_blankcheck();

    // functions which are executed in the primary thread context:
    bool RunThread(UppMainWindowThreadMode mode);
    /** Handles an update from the secondary thread */
    void OnThreadUpdate(wxThreadEvent& evt);

    /** Handles the notification about the secondary thread just finishing */
    void OnThreadCompleted(wxThreadEvent& evt);

protected:     // event handlers
    void on_mru( wxCommandEvent& event );
    void on_close( wxCloseEvent& event );
    void on_new( wxCommandEvent& event ){upp_new(); EVENT_FIX}
    void on_open( wxCommandEvent& event ){upp_open(); EVENT_FIX}
    void on_refresh( wxCommandEvent& event ){upp_refresh(); EVENT_FIX}
    void on_save( wxCommandEvent& event ){upp_save(); EVENT_FIX}
    void on_save_as( wxCommandEvent& event ){upp_save_as(); EVENT_FIX}
    void on_exit( wxCommandEvent& event ){upp_exit(); EVENT_FIX}
    void on_copy( wxCommandEvent& event ){upp_copy(); EVENT_FIX}
    void on_selectall( wxCommandEvent& event ){upp_selectall(); EVENT_FIX}
    void on_program( wxCommandEvent& event ){upp_program(); EVENT_FIX}
    void on_read( wxCommandEvent& event ){upp_read(); EVENT_FIX}
    void on_verify( wxCommandEvent& event ){upp_verify(); EVENT_FIX}
    void on_erase( wxCommandEvent& event ){upp_erase(); EVENT_FIX}
    void on_restore( wxCommandEvent& event ){upp_restore(); EVENT_FIX}
    void on_blankcheck( wxCommandEvent& event ){upp_blankcheck(); EVENT_FIX}
    void on_autodetect( wxCommandEvent& event ){upp_autodetect(); EVENT_FIX}
    void on_connect( wxCommandEvent& event){upp_connect(); EVENT_FIX}
    void on_disconnect( wxCommandEvent& event ){upp_disconnect(); EVENT_FIX}
    void on_preferences( wxCommandEvent& event ){upp_preferences(); EVENT_FIX}
    void on_io_test( wxCommandEvent& event ){upp_io_test(); EVENT_FIX}
    void on_help( wxCommandEvent& event ){upp_help(); EVENT_FIX}
    void on_about( wxCommandEvent& event ){upp_about(); EVENT_FIX}
    void on_pic_choice_changed( wxCommandEvent& event ){upp_pic_choice_changed(); EVENT_FIX}
    void on_pic_choice_changed_bymenu( wxCommandEvent& event )
        {upp_pic_choice_changed_bymenu(event.GetId()-wxID_PIC_CHOICE_MENU); EVENT_FIX}
    void on_package_variant_changed( wxCommandEvent& event ) 
        {upp_package_variant_changed(); EVENT_FIX}
    void on_size( wxSizeEvent& event ) 
        {upp_size_changed(); EVENT_FIX}

private:    // real event handlers

    /** Open a hexfile using a file dialog */
    void upp_open();

    /** Re-open the hexfile */
    void upp_refresh();

    /** Save the hexfile when already open, else perform a save_as */
    void upp_save();

    /** Save the hex file with a file dialog */
    void upp_save_as();

    /** The user is exiting the app */
    void upp_exit();

    /** The user clicked copy */
    void upp_copy();

    /** The user clicked select all */
    void upp_selectall();

    /** Show preferences dialog */
    void upp_preferences();

	/** Show the IO Test dialog */
	void upp_io_test();
    
    /** Load a browser with the usbpicprog website */
    void upp_help();
    
    /** Show an about box */
    void upp_about();

    /**
        @name Functions to keep internal structs and the GUI synchronized

        See also the public upp_hex_changed().
    */
    //@{
    
    /** Size of the window was changed */
    void upp_size_changed();
    
    /** If the PIC was changed from the wxChoice in the toolbar, also change it in the m_hardware */
    void upp_pic_choice_changed();
    
    /** If the PIC was changed from the menu, also change it in the m_hardware */
    void upp_pic_choice_changed_bymenu(int id);
    
    /** The package of the current PIC was changed by the choice box in the misc info panel */
    void upp_package_variant_changed();
    
    //@}


    /**
        @name Functions which issue commands to the hardware class.
    */
    //@{
    
    /** Write everything to the device */
    void upp_program();
    
    /** Read everything from the device */
    void upp_read();
    
    /** Verify the device with the open hexfile */
    void upp_verify();
    
    /** Perform a bulk-erase on the current PIC */
    void upp_erase();

	/** Restore OSCCAL and BANDGAP registers of PIC12F629 and similar devices */
    void upp_restore();
    
    /** Check if the device is erased successfully */
    void upp_blankcheck();
    
    /** Detect which PIC is connected and select it in the choicebox and the m_hardware */
    bool upp_autodetect();

	void checkFirmwareVersion(FirmwareVersion firmwareVersion);
	
    /** Connect upp_wx to the upp programmer */
    bool upp_connect();
    
    /** Disconnect the m_hardware */
    void upp_disconnect();
    
    //@}


private:    // member variables

    /** Contains the current code, data, config bytes shown in this window. */
    HexFile m_hexFile;

    /** Contains all the info about the current PIC model selected in this window. */
    PicType m_picType;

    /** Manages communication to the real hardware. */
    Hardware* m_hardware;

    /** The names of all supported PICs. */
    wxArrayString m_arrPICName;

    /** The current global preferences (they can be modified through UppPreferencesDialog). */
    UppPreferences m_cfg;

    /** The last path used by the user in the load/save dialogs. */
    wxString m_defaultPath;

    /** The control showing the currently selected PIC. */
    wxChoice* m_pPICChoice;

    /** The list of most recently used files. */
    wxFileHistory m_history;


private:   // variables related to the threaded operations:

    /// The progress dialog: this variable is accessed only by the main/primary thread.
    wxProgressDialog* m_dlgProgress;

    //@{
    /// The messages recorded during the thread activity
    wxCriticalSection m_arrLogCS;
    wxArrayString m_arrLog;
    wxArrayInt m_arrLogLevel;
    wxArrayTime m_arrLogTimes;
    //@}

    /// Tells the thread which operation should be performed
    /// @note Since the main thread reads/writes this var only when the
    ///       secondary thread has ended, there's no need for critical section.
    UppMainWindowThreadMode m_mode;

	wxMenuItem* m_pMenuRestoreCal;
};

#endif   // UPPMAINWINDOW_H
