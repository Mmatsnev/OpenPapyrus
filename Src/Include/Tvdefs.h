// TVDEFS.H
// Copyright (c) Sobolev A. 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2010, 2011, 2012, 2013, 2014, 2016
// @codepage windows-1251
//
#ifndef __TVDEFS_H
#define __TVDEFS_H
/* @v9.1.12
#ifdef RC_INVOKED
	#ifndef __SLIB_H
		#include <slib.h>
	#endif
#endif
*/
//
//  Control keys
//
//  NOTE: these Control key definitions are intended only to provide
//  mnemonic names for the ASCII control codes.  They cannot be used
//  to define menu hotkeys, etc., which require scan codes.
//
#define kbCtrlA          0x0001
#define kbCtrlB          0x0002
#define kbCtrlC          0x0003
#define kbCtrlD          0x0004
#define kbCtrlE          0x0005
#define kbCtrlF          0x0006
#define kbCtrlG          0x0007
#define kbCtrlH          0x0008
#define kbCtrlI          0x0009
#define kbCtrlJ          0x000a
#define kbCtrlK          0x000b
#define kbCtrlL          0x000c
#define kbCtrlM          0x000d
#define kbCtrlN          0x000e
#define kbCtrlO          0x000f
#define kbCtrlP          0x0010
#define kbCtrlQ          0x0011
#define kbCtrlR          0x0012
#define kbCtrlS          0x0013
#define kbCtrlT          0x0014
#define kbCtrlU          0x0015
#define kbCtrlV          0x0016
#define kbCtrlW          0x0017
#define kbCtrlX          0x0018
#define kbCtrlY          0x0019
#define kbCtrlZ          0x001a
//
// Extended key codes
//
#define kbSpace          0x0020
#define kbEsc            0x011b
#define kbAltSpace       0x0200
#define kbCtrlIns        0x0400
#define kbShiftIns       0x0500
#define kbCtrlDel        0x0600
#define kbShiftDel       0x0700
#define kbBack           0x0e08
#define kbCtrlBack       0x0e7f
#define kbShiftTab       0x0f00
#define kbTab            0x0f09
#define kbAltQ           0x1000
#define kbAltW           0x1100
#define kbAltE           0x1200
#define kbAltR           0x1300
#define kbAltT           0x1400
#define kbAltY           0x1500
#define kbAltU           0x1600
#define kbAltI           0x1700
#define kbAltO           0x1800
#define kbAltP           0x1900
#define kbCtrlEnter      0x1c0a
#define kbEnter          0x1c0d
#define kbAltA           0x1e00
#define kbAltS           0x1f00
#define kbAltD           0x2000
#define kbAltF           0x2100
#define kbAltG           0x2200
#define kbAltH           0x2300
#define kbAltJ           0x2400
#define kbAltK           0x2500
#define kbAltL           0x2600
#define kbAltZ           0x2c00
#define kbAltX           0x2d00
#define kbAltC           0x2e00
#define kbAltV           0x2f00
#define kbAltB           0x3000
#define kbAltN           0x3100
#define kbAltM           0x3200
#define kbF1             0x3b00
#define kbF2             0x3c00
#define kbF3             0x3d00
#define kbF4             0x3e00
#define kbF5             0x3f00
#define kbF6             0x4000
#define kbF7             0x4100
#define kbF8             0x4200
#define kbF9             0x4300
#define kbF10            0x4400
#define kbF11            0x4500
#define kbF12            0x4600
#define kbHome           0x4700
#define kbUp             0x4800
#define kbPgUp           0x4900
#define kbGrayMinus      0x4a2d
#define kbLeft           0x4b00
#define kbRight          0x4d00
#define kbGrayPlus       0x4e2b
#define kbEnd            0x4f00
#define kbDown           0x5000
#define kbPgDn           0x5100
#define kbIns            0x5200
#define kbDel            0x5300
#define kbShiftF1        0x5400
#define kbShiftF2        0x5500
#define kbShiftF3        0x5600
#define kbShiftF4        0x5700
#define kbShiftF5        0x5800
#define kbShiftF6        0x5900
#define kbShiftF7        0x5a00
#define kbShiftF8        0x5b00
#define kbShiftF9        0x5c00
#define kbShiftF10       0x5d00
#define kbCtrlF1         0x5e00
#define kbCtrlF2         0x5f00
#define kbCtrlF3         0x6000
#define kbCtrlF4         0x6100
#define kbCtrlF5         0x6200
#define kbCtrlF6         0x6300
#define kbCtrlF7         0x6400
#define kbCtrlF8         0x6500
#define kbCtrlF9         0x6600
#define kbCtrlF10        0x6700
#define kbAltF1          0x6800
#define kbAltF2          0x6900
#define kbAltF3          0x6a00
#define kbAltF4          0x6b00
#define kbAltF5          0x6c00
#define kbAltF6          0x6d00
#define kbAltF7          0x6e00
#define kbAltF8          0x6f00
#define kbAltF9          0x7000
#define kbAltF10         0x7100
#define kbCtrlPrtSc      0x7200
#define kbCtrlLeft       0x7300
#define kbCtrlRight      0x7400
#define kbCtrlEnd        0x7500
#define kbCtrlPgDn       0x7600
#define kbCtrlHome       0x7700
#define kbAlt1           0x7800
#define kbAlt2           0x7900
#define kbAlt3           0x7a00
#define kbAlt4           0x7b00
#define kbAlt5           0x7c00
#define kbAlt6           0x7d00
#define kbAlt7           0x7e00
#define kbAlt8           0x7f00
#define kbAlt9           0x8000
#define kbAlt0           0x8100
#define kbAltMinus       0x8200
#define kbAltEqual       0x8300
#define kbCtrlPgUp       0x8400
#define kbAlt            0xf000
#define kbAltIns         0xf100
#define kbNoKey          0x0000
//
//  Keyboard state and shift masks
//
#define kbRightShift     0x0001
#define kbLeftShift      0x0002
#define kbCtrlShift      0x0004
#define kbAltShift       0x0008
#define kbScrollState    0x0010
#define kbNumState       0x0020
#define kbCapsState      0x0040
#define kbInsState       0x0080
//
//  Standard command codes
//
#define cmValid               0
#define cmNone                0
#define cmQuit                1
#define cmError               2
#define cmMenu                3
#define cmClose               4
#define cmZoom                5
#define cmResize              6
#define cmNext                7
#define cmPrev                8
#define cmHelp                9
//
//  TDialog standard commands
//
#define cmOK                 10
#define cmCancel             11
#define cmYes                12
#define cmNo                 13
#define cmDefault            14
#define cmInit               15 // Initialization command
#define cmUtil               16
#define cmRightClick         17 // (Win32 only) Right mouse button clicked on window
#define cmCommitInput        18 // Commiting input data in TInputLine.
	// event.what = evBroadcast, event.mesage.infoView = &TInputLine
#define cmInputUpdated       19 // TInputLine updated by user
	// event.what = evCommand, event.message.infoView = &TInputLine
//
//  Application command codes
//
#define cmCut                20
#define cmCopy               21
#define cmPaste              22
#define cmUndo               23
#define cmClear              24
#define cmTile               25
#define cmCascade            26
//
// Action commands (Application level)
//
#define cmaDefaultAction     27
#define cmaInsert            28
#define cmaAltInsert         29
#define cmaDelete            30
#define cmaEdit              31
#define cmaSearch            32
// #define cmaAltSearch         33 // ������������ � pplistdialog, ��� �������� ������� ������ �� email
#define cmaSendByMail        33 // ������������ � pplistdialog, ��� �������� ������� ������ �� email
#define cmaSort              34
#define cmaTransfer          35
#define cmaCalculate         36
#define cmaAltCalculate      37
#define cmaSelect            38
#define cmaAltSelect         39
#define cmaMore              40
#define cmaLevelDown         41
#define cmaLevelUp           42
#define cmaSkip              43
#define cmaAll               44
#define cmaBack              45
#define cmaDesktop           46
#define cmaRename            47
//
// Standard messages
//
#define cmIdle               48 // (evBroadcast) ���������� �������� TProgram::idle
//
// Message: cmChangedFocus
// Descr: ���������� �������� TGroup::setCurrent � ��������� �������
//   ����� ���������� ���� ���������, � ����� �������� TDialog � ����� ��
//   ��������� WINDOWS WM_KILLFOCUS (function DialogProc).
// Parameters:
//   ��������� �� TView, ���������� �����
//
#define cmChangedFocus       49
#define cmReceivedFocus      50
#define cmReleasedFocus      51
#define cmCommandSetChanged  52
//
// TScrollBar messages
//
#define cmScrollBarChanged   53
#define cmScrollBarClicked   54
//
// TWindow select messages
//
#define cmSelectWindowNum    55
//
//  Added command codes
//
#define cmRequestID          56
#define cmSearchLabel        57
#define cmSearchVirtButton   58 // evBroadcast
#define cmInputUpdatedByBtn  59 // ��������� ����� � ������ �����, ����������� ������� �����-���� �������
#define cmInputDblClk        60 // �� ���� ����� ������������ ������ ������� ����� ������� ����
//
//
//
#define cmDrawItem           61 // evCommand ��������� ���������� ���������� TDialog � ����� �� ���������
	// ��������� WM_DRAWITEM. � �������������� ���������� ���������� ��������� �� ��������� TDrawItemData
#define cmWinKeyDown         62 // @v5.4.7 AHTOXA ���������� ���������, �������������� TVKEYDOWN, �� � WIN ������ ������
#define cmCtlColor           63 // @v5.5.8 evCommand ��������� ���������� ���������� TDialog � ����� �� ���������
	// ��������� WM_CTLCOLORSTATIC, WM_CTLCOLOREDIT, WM_CTLCOLORSCROLLBAR. � �������������� ���������� ���������� ��������� ��
	// ��������� TDrawCtrlData
#define cmSearchButton       64 // evBroadcast ��������� ���������� ������� ��� ������ ������, ������������ �������
	// event.message.infoWord. ������, ������� ���������� ����� ������� ������ ������� clearEvent � ����� �� ���������.
#define cmLocalMenu          65 // evCommand @v5.7.8 �������� ���������� ��������� ���� ������������ ��������
#define cmSetupTooltip       66 // evCommand @v5.6.7 ���������� ����, ��� ����, ����� ��� ������ ����������������
	// ��������� (tooltip) //
#define cmSetupResizeParams  67 // evCommand @v5.6.7 ���������� ����, ��� ����, ����� ��� ���������� ���������
	// ��������� �������� ��� ����������� ���������
#define cmMouseHover         68 // evBroadcast @v6.0.2 ���������� ����, ����� ������ ���� ������������� �� ������������ �����.
	// � infoPtr ���������� ���������� ������� ���� (TPoint).
#define cmTimeChange         69 // evCommand ���������� ������ TProgram,
	// ����� �����-���� ���������� �������� ��������� ����� //
//
// ComboBox messages
//
#define cmCBSelected         70
#define cmWSSelected         71 // evCommand ���������� ��� ��������� ������ ����� ������ ������������� ������ � WordSelector.
	// �������������� �������� - ��������� �� TView � ������� ��������� ������� (������ TInputLine).
#define cmCBActivate        300
//
// ���������, ���������� ������ ����� ��� ��������� �������� ����� ���
// ������. ����� ���� ������������ � ��������� �������� ����� ������������.
//
#define cmGetFocusedNumber  301 // double * infoPtr;
#define cmGetFocusedText    302 // char * infoPtr;
#define cmGroupInserted     303 // evCommand ��������� ���������� �������� ������ CtrlGroup ����� ������� �� � ���� ������
//
#define cmPrint             141 // evCommand
//
//  TListViewer messages
//
#define cmLBItemSelected     71
#define cmLBItemUnselected   72
#define cmLBItemFocused      73
#define cmLBDblClk           74
#define cmLBEmpty            75
#define cmClusterClk         76 // evCommand TCluster message
#define cmLBLoadDef          77 //
#define cmGetHelpContext     78 //
#define cmBrwsSrchPreprocess 81 // evCommand Browser message
#define cmSetFont            82 // ���������� ���������� TWindowBase ��� ��������� ������.
	// � infoPtr ���������� ��������� �� ��������� SetFontEvent
#define cmSize               83 // ���������� ���������� TWindowBase ����� ��������� �������
	// ����. � infoPtr ���������� ��������� �� ��������� SizeEvent
#define cmPaint              84 // ���������� ���������� TWindowBase � ����� �� ���� ��
	// ��������� ��������� Windows: WM_PAINT, WM_NCPAINT, WM_ERASEBKGND.
#define cmMouse              85 // ���������� ���������� TWindowBase � ����� �� ���� ��
	// ��������� ��������� Windows:
	// WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK,
	// WM_MBUTTONDOWN, WM_MBUTTONUP, WM_MBUTTONDBLCLK, WM_MOUSEMOVE, WM_MOUSEWHEEL, WM_MOUSEHOVER
#define cmScroll             86 // ���������� ���������� TWindowBase � ����� �� ���� ��
	// ��������� ��������� Windows: WM_HSCROLL, WM_VSCROLL
#define cmMove               87 // ���������� ���������� TWindowBase � ����� ��������� WM_MOVE
#define cmSetBounds          88 // ������������ ���������. ���������� ���������� TView ��� ����, ����� ��
	// ������� ���� ���������� � (���) ������. TEvent.message.infoPtr ��������� �� TRect.
#define cmDragndropObj       89 // ���������� �����, ���������� � ������������ ������
	// � ��������������� ��������. ������ � ��������� ���������� ��������� DragndropEvent
#define cmEndModal           90 // @construction @v6.9.9 evCommand ���������� ��� ��������� ������ ����������� ����.
	// ������������ ��� ��������� ����������� ������� TView::endModal.
	// TEvent::message::infoWord �������� �������, �� ������� �������������� ���������� ������.
#define cmExecute            91 // @v9.0.4 evCommand �������, ���������� ����� ����������� ������� PPView::execute
	// TEvent::message::infoLong ���������� ��� �������, ������� ���������� ��������� ����
//
// StdDlg commands & messages
//
#define cmFileOpen         1001   // Returned from TFileDialog when Open pressed
#define cmFileReplace      1002   // Returned from TFileDialog when Replace pressed
#define cmFileClear        1003   // Returned from TFileDialog when Clear pressed
#define cmFileInit         1004   // Used by TFileDialog internally
#define cmChangeDir        1005   // Used by TChDirDialog internally
#define cmRevert           1006   // Used by TChDirDialog internally
#define cmFileSave         1007   // @v6.6.1
#define cmFileSaveAs       1008   // @v6.6.1
#define cmBringToFront     1009   // @v6.6.12 ����������� ������ �� �������� ����
#define cmSendToBack       1010   // @v6.6.12 ����������� ������ �� ������ ����
#define cmProperties       1011   // @v6.7.1  ��������
#define cmUp               1012   // @v6.7.1
#define cmDown             1013   // @v6.7.1
#define cmGrouping         1014   // @v6.7.1
//
// SBrowser command & messages
//
#define cmBroDataChanged    110   // (evCommand) Send to BrowserView
//
//
//
#define WM_USER_COMBO_ACTIVATEBYCHAR WM_USER+10 // ������� ����������� ������
	// �����-����� �� ������� ���������� �������
#define WM_USER_COMBO_CLEAR          WM_USER+11 // ������� ������� �����-����� (������� Delete)
#define WM_USER_MAINWND_MOVE_SIZE    WM_USER+12 // ������� ��������� �������� ��� ��������� �������� ����, ���������� �������� �����
#define WM_USER_CLOSE_TOOLTIPMSGWIN  WM_USER+13 // ������� ������������� ������ ��� ������� DLG_TOOLTIP, ��������� ������ ������.
#define WM_USER_GETLIST              WM_USER+14 // ��������� ������ StrAssocArray �� ListWindow, SmartListBox �� ���������
#define WM_USER_SHOWTREEWNDITEM       WM_USER+15 // �������� � ���������� ���� �� ������� �������� ����
#define WM_USER_CLOSETREEWNDITEM     WM_USER+16 // ��������� ���� �� ������� �������� ����
#define WM_USER_NOTIFYBRWFRAME       WM_USER+0  // ��������� ������ �� ��������� ����� ����
#define WM_USER_CLOSEBROWSER         WM_USER+1  // ������� �������� ������������ ��������.
	// ������������ ��������� ����� CloseWndProc (TProgram.cpp)

//
// Button types
//
#define bfNormal           0x00
#define bfDefault          0x01
#define bfLeftJust         0x02
#define bfBroadcast        0x04
//
//  TView State masks
//
#define sfVisible         0x00000001
#define sfActive          0x00000010
#define sfSelected        0x00000020
#define sfFocused         0x00000040
#define sfDisabled        0x00000100
#define sfModal           0x00000200
#define sfReadOnly        0x00001000 // Work for TInputLine only
#define sfCmdSetChanged   0x00002000 // @v6.6.8
#define sfMsgToParent     0x00004000 // @v6.6.8 // ���� ����������, �� Win-��������� ������������ �������, ����� - � handleWindowsMessage
#define sfEventBarrier    0x00008000 // @v7.7.7 ������ TView ��������� � �������� ���������� ������� handleEvent.
	// ����� ���������� ���������� ��� ���������� �������� handleEvent � �������� ������ '��������� �������.
#define sfOnDestroy       0x00010000 // @v7.7.7 ������ ��������� � ��������� ����������. ���� ��������������� ������� ����������
	// ��� ��������� ��������� WM_DESTROY � ��������� ��� �������������� ������������ ��� ������� ������� ��������� ���������
	// ������ TView ����� ���������� TView �� ������� � ���� ������� ��������� ����.
//
// TView Option masks
//
#define ofSelectable      0x0001
#define ofFirstClick      0x0004
#define ofFramed          0x0008
#define ofPreProcess      0x0010
#define ofPostProcess     0x0020
#define ofCenterX         0x0100
#define ofCenterY         0x0200
#define ofCentered        0x0300
//
// TView GrowMode masks
//
#define gfGrowLoX          0x01
#define gfGrowLoY          0x02
#define gfGrowHiX          0x04
#define gfGrowHiY          0x08
#define gfGrowAll          0x0f
#define gfGrowRel          0x10
//
// TView DragMode masks
//
#define dmDragMove         0x01
#define dmDragGrow         0x02
#define dmLimitLoX         0x10
#define dmLimitLoY         0x20
#define dmLimitHiX         0x40
#define dmLimitHiY         0x80
#define dmLimitAll         (dmLimitLoX | dmLimitLoY | dmLimitHiX | dmLimitHiY)
//
// TView Help context codes
//
#define hcNoContext           0
#define hcDragging            1
//
// TScrollBar part codes
//
#define sbLeftArrow           0
#define sbRightArrow          1
#define sbPageLeft            2
#define sbPageRight           3
#define sbUpArrow             4
#define sbDownArrow           5
#define sbPageUp              6
#define sbPageDown            7
#define sbIndicator           8
#define sbTop                 9
#define sbBottom             10
//
// TScrollBar options for TWindow.StandardScrollBar
//
#define sbHorizontal      0x000
#define sbVertical        0x001
#define sbHandleKeyboard  0x002
//
//  TView inhibit flags
//
#define noMenuBar        0x0001
#define noDeskTop        0x0002
#define noStatusLine     0x0004
#define noBackground     0x0008
#define noFrame          0x0010
#define noViewer         0x0020
#define noHistory        0x0040
//
// TWindow number constants
//
#define wnNoNumber            0
//
// TWindow palette entries
//
#define wpBlueWindow          0
#define wpCyanWindow          1
#define wpGrayWindow          2
//
//  Event masks
//
#define positionalEvents    evMouse
#define focusedEvents       (evKeyboard | evCommand)
//
// MsgBox constants
//
#define mfWarn           0x0000        // Display a Warning box
#define mfError          0x0001        // Dispaly a Error box
#define mfInfo           0x0002        // Display an Information Box
#define mfConf           0x0003        // Display a Confirmation Box
#define mfCritWarn       0x0004        // Display a Critical Warning box

#define mfYes            0x0100        // Put a Yes button into the dialog
#define mfNo             0x0200        // Put a No button into the dialog
#define mfOK             0x0400        // Put an OK button into the dialog
#define mfCancel         0x0800        // Put a Cancel button into the dialog
#define mfAll            0x1000
#define mfDefaultYes     0x2000        // (Win32 mfConf only) Yes button by default
#define mfLargeBox       0x4000        // Show large MsgBox
#define mfYesNo          (mfYes|mfNo)
#define mfYesNoCancel    (mfYes|mfNo|mfCancel)
#define mfOKCancel       (mfOK|mfCancel)
//
// ListViewer types
//
#define lbtExtSel        0x0001
#define lbtDblClkNotify  0x0002
#define lbtFocNotify     0x0004
#define lbtSelNotify     0x0008
#define lbtNoData        0x0010
#define lbtSorted        0x0020
#define lbtDisposeData   0x0040
#define lbtAutoID        0x0080
#define lbtWordID        0x0100 // If set, then id is word, otherwise dword
#define lbtVScrollBar    0x0200 // For resource definition only !
#define lbtHScrollBar    0x0400 // For function CreateListWindow()
#define lbtHSizeAlreadyDef 0x0800 // For combobox listwindow
#define lbtTransferData  0x2000 // Else transfer ID (Don't use with cbxNoData)
#define lbtOwnerDraw     0x4000 //
#define lbtExtMenu       0x8000 // ����������� ���� ��������������. �������� ����� ��������� �� email
//
// ComboBox flags
//
#define cbxDblClkNotify  lbtDblClkNotify
#define cbxFocNotify     lbtFocNotify
#define cbxNoData        lbtNoData
#define cbxSorted        lbtSorted
#define cbxDisposeData   lbtDisposeData
#define cbxAutoID        lbtAutoID
#define cbxWordID        lbtWordID
#define cbxListOnly      0x1000
#define cbxNoActivate    0x0800 // User can't directly select item from list
#define cbxTransferData  lbtTransferData
#define cbxSimple        0x4000
#define cbxAllowEmpty    0x8000
//
// OwnerDrawCtrls definition
//
enum CtrlTypes {
	ctListBox  = 1,
	ctComboBox
};

struct ODC {
	long   CtrlType; // ctXXX
	long   CtrlID;
	long   ExtraParam;
};

extern struct ODC OwnerDrawCtrls[32]; // @defined(TDIALOG.CPP)
//
// File Dialog flags
//
#define fdOKButton       0x0001       // Put an OK button in the dialog
#define fdOpenButton     0x0002       // Put an Open button in the dialog
#define fdReplaceButton  0x0004       // Put a Replace button in the dialog
#define fdClearButton    0x0008       // Put a Clear button in the dialog
#define fdHelpButton     0x0010       // Put a Help button in the dialog
#define fdNoLoadDir      0x0100       // Do not load the current directory
//                                       contents into the dialog at Init.
//                                       This means you intend to change the
//                                       WildCard by using SetData or store
//                                       the dialog on a stream.
//
// Change Dir Dialog flags
//
#define cdNormal         0x0000 // Option to use dialog immediately
#define cdNoLoadDir      0x0001 // Option to init the dialog to store on a stream
#define cdHelpButton     0x0002 // Put a help button in the dialog
//
// Browse columns options
//
//#define CALC_AVERAGE        1
//#define CALC_SUMM           2
//#define CALC_SIGMA          3
//#define CALC_MIN            4
//#define CALC_MAX            5
#define BCO_EDIT         0x00000001
//#define BCO_LEFT       0x00000002
//#define BCO_RIGHT      0x00000004
//#define BCO_CENTER     0x00000008
#define BCO_CAPLEFT      0x00000010
#define BCO_CAPRIGHT     0x00000020
#define BCO_CAPCENTER    0x00000040
#define BCO_CAPTOP       0x00000080
#define BCO_CAPBOTTOM    0x00000100
#define BCO_CAPVCENTER   0x00000200
#define BCO_USERPROC     0x00001000
//#define BCO_SKIP       0x00002000
#define BCO_DEFOPT       (BCO_CAPBOTTOM | BCO_CAPCENTER)
#define BCO_DONTSHOWDUPL 0x00004000
#define BCO_RESIZEABLE   0x00008000
#define BCO_SIZESET      0x00010000 // @v6.2.10 ������ ������� �������������� �������� SutupColumnWidth
//
// Browse options
//
#define BRO_EDIT         0x0001
#define BRO_INS          0x0002
#define BRO_DEL          0x0004
#define BRO_GRID         0x0008
#define BRO_OWNER        0x0010
//
// TVRez definitions
//
#define TV_END           0xffff

#define TV_MENU             300
	#define TV_POPUP         0xff01
	#define TV_MENUITEM      0xff03
	#define TV_MENUSEPARATOR 0xff04
#define TV_STATUS           301
	#define TV_STATUSDEF     0xff01
	#define TV_STATUSITEM    0xff03
#define TV_DIALOG           302
	#define TV_INPUTLINE     0xff01
	#define TV_BUTTON        0xff02
	#define TV_CHECKBOXES    0xff03
	#define TV_RADIOBUTTONS  0xff04
	#define TV_LISTBOX       0xff05
	#define TV_STATIC        0xff06
	#define TV_LABEL         0xff07
	#define TV_INFOPANE      0xff08
	#define TV_HISTORY       0xff09
	#define TV_SCROLLBAR     0xff0A
	#define TV_MEMO          0xff0B
	#define TV_COMBO         0xff0C

	#define TV_TOOLBAR	0xff0D
	#define TV_LOCALMENU	0xff0E
	#define TV_IMPTOOLBAR	0xff0F

#define TV_PALETTE          303
#define TV_BROWSER          304
	#define TV_BROCOLUMN     0xff01
	#define TV_BROGROUP      0xff02
	#define TV_CROSSTAB      0xff03 // 24/08/2004
#define TV_REPORT           305
#define TV_EXPTOOLBAR       306
#define TV_GLBTOOLBAR       307
//
// Cursor
//
#define IDC_PPYWAIT               301
#define IDC_PPYPOINT              302
#define IDC_TWOARR_HORZ           303
#define IDC_TWOARR_VERT           304
#define IDC_ARR_ROZE              305
#define IDC_CALENDAR              306
#define IDC_MOVE                  307
#define IDC_SIZE_EW               308
#define IDC_SIZE_NS               309
#define IDC_SIZE_NESW             310
#define IDC_SIZE_NWSE             311
#define IDC_ODIOUS                312 // �������� �������� ��� ����� �������
//
//
//
#define STD_CUT                 0
#define STD_COPY                1
#define STD_PASTE               2
#define STD_UNDO                3
#define STD_REDOW               4
#define STD_DELETE              5
#define STD_FILENEW             6
#define STD_FILEOPEN            7
#define STD_FILESAVE            8
#define STD_PRINTPRE            9
#define STD_PROPERTIES          10
#define STD_HELP                11
#define STD_FIND                12
#define STD_REPLACE             13
#define STD_PRINT               14

#define DLGW_MSGBOX              4092
#define DLGW_MSGBOX_L            4109
	#define CTL_WMSGBOX_TEXT             200
	#define CTL_WMSGBOX_FIRSTBUTTON      201
	#define CTL_WMSGBOX_LASTBUTTON       207

#define DLGW_LBX                 4095
#define DLGW_LBX_L               4110
	#define CTL_LBX_LIST                 201


#define DLGW_TREELBX             4107
	#define CTL_TREELBX_TREELIST        1015
#define DLGW_OWNDRAWLBX          4108
#	define CTL_OWNDRAWLBX_LIST           201
//                               4109 // Occupied by DLGW_MSGBOX_L
//                               4110 // Occupied by DLGW_LBX_L
//                               4111 // occupied by DLGW_CALENDAR_L
#define DLGW_LOGGER              4112
	#define CTL_LOGGER_LIST              201
	#define CTL_LOGGER_LINE1             202
	#define CTL_LOGGER_LINE2             203
#define DLGW_LBXFLAT             4113
	// CTL_LBX_...
#define DLGW_TREELBXNM           4114
	// CTL_TREELBX
#define TB_ICON_USER	     100

#define TB_FIRST_TOOLBAR     301
	#define TB_ICON_EDIT             STD_PROPERTIES
	#define TB_ICON_DELETE	         STD_DELETE
	#define TB_ICON_PRINT	         STD_PRINT
	#define TB_ICON_BROWSE	         STD_FIND
	#define TB_ICON_NOICON           0+TB_ICON_USER
	#define TB_ICON_LINS             1+TB_ICON_USER
	#define TB_ICON_SIGM             2+TB_ICON_USER
	#define TB_ICON_BARCODE          3+TB_ICON_USER
	#define TB_ICON_HAMMER           4+TB_ICON_USER
	#define TB_ICON_PAYMENT          5+TB_ICON_USER
	#define TB_ICON_GOODSRET         6+TB_ICON_USER
	#define TB_ICON_CHGFLAGS         7+TB_ICON_USER
	#define TB_ICON_REFRESHVIEW      8+TB_ICON_USER
	#define TB_ICON_QCERT            9+TB_ICON_USER
	#define TB_ICON_LOTSOPER        10+TB_ICON_USER
	#define TB_ICON_CHNGQCERT       11+TB_ICON_USER
	#define TB_ICON_GOODS           12+TB_ICON_USER
	#define TB_ICON_GOODSSTRCT      13+TB_ICON_USER
	#define TB_ICON_PRNLABELS       14+TB_ICON_USER
	#define TB_ICON_GENGOODS        15+TB_ICON_USER
	#define TB_ICON_PERCENT         16+TB_ICON_USER
	#define TB_ICON_PRINTLIST       17+TB_ICON_USER
	#define TB_ICON_SYSTEMINFO      18+TB_ICON_USER
	#define TB_ICON_QUOTATION       19+TB_ICON_USER
	#define TB_ICON_RENTA           20+TB_ICON_USER
	#define TB_ICON_CHECKSTAT       21+TB_ICON_USER
	#define TB_ICON_COPYBYSAMPLE    22+TB_ICON_USER
	#define TB_ICON_CHNGPRICE       23+TB_ICON_USER
	#define TB_ICON_REFRESH         24+TB_ICON_USER
	#define TB_ICON_OPERMOVE        25+TB_ICON_USER
	#define TB_ICON_LOTS            26+TB_ICON_USER
	#define TB_ICON_ORDLOTS         27+TB_ICON_USER
	#define TB_ICON_DELETEALL       28+TB_ICON_USER
	#define TB_ICON_EXTADDTI        29+TB_ICON_USER
	#define TB_ICON_ADDTIFROMORD    30+TB_ICON_USER
	#define TB_ICON_DEBTCARD        31+TB_ICON_USER
	#define TB_ICON_UNCHECKED       32+TB_ICON_USER
	#define TB_ICON_CHECKED         33+TB_ICON_USER
	#define TB_ICON_SORT            34+TB_ICON_USER
	#define TB_ICON_PERSONS         35+TB_ICON_USER
	#define TB_ICON_GOLD1           36+TB_ICON_USER
	#define TB_ICON_GOLD2           37+TB_ICON_USER
	#define TB_ICON_TO_TRASH        38+TB_ICON_USER
	#define TB_ICON_HOUSE           39+TB_ICON_USER
	#define TB_ICON_BILLS            40+TB_ICON_USER
	#define TB_ICON_TAGS             41+TB_ICON_USER
	#define TB_ICON_CLOSECSESS       42+TB_ICON_USER
	#define TB_ICON_COMPLETECSESS    43+TB_ICON_USER
	#define TB_ICON_RECKON           44+TB_ICON_USER
	#define TB_ICON_RECKONPAYM       45+TB_ICON_USER
	#define TB_ICON_RECKONDEBT       46+TB_ICON_USER
	#define TB_ICON_OPGRPNG          47+TB_ICON_USER
	#define TB_ICON_ACCANLZ          48+TB_ICON_USER
	#define TB_ICON_CSESSEXC         49+TB_ICON_USER
	#define TB_ICON_CSESSEXCALL      50+TB_ICON_USER
	#define TB_ICON_CSEXCAG          51+TB_ICON_USER
	#define TB_ICON_ACCTURNSBYBILL   52+TB_ICON_USER
	#define TB_ICON_GOODSANLZ        53+TB_ICON_USER
	#define TB_ICON_SENDOBJ          54+TB_ICON_USER
	#define TB_ICON_SYSJ             55+TB_ICON_USER
	#define TB_ICON_PCKGNEW          56+TB_ICON_USER
	#define TB_ICON_PCKGLINES        57+TB_ICON_USER
	#define TB_ICON_BASKET 	         58+TB_ICON_USER
	#define TB_ICON_RECKONBILLS      59+TB_ICON_USER
	#define TB_ICON_PROGNOSIS        60+TB_ICON_USER
	#define TB_ICON_CCHECKS          61+TB_ICON_USER
	#define TB_ICON_CHARGE           62+TB_ICON_USER
	#define TB_ICON_ADDTISFROMBASKET 63+TB_ICON_USER
	#define TB_ICON_POSTALBATROS     64+TB_ICON_USER
	#define TB_ICON_TEST             65+TB_ICON_USER // �������������
	#define TB_ICON_FILTER           66+TB_ICON_USER
	#define TB_ICON_EXPORT           67+TB_ICON_USER
	#define TB_ICON_DIVISION         68+TB_ICON_USER
	#define TB_ICON_INSERT           69+TB_ICON_USER
	#define TB_ICON_ADDTISTOBASKET   70+TB_ICON_USER
	#define TB_ICON_TOGGLESHIPMTAG   71+TB_ICON_USER
	#define TB_ICON_EXTRAVIEW        72+TB_ICON_USER
	#define TB_ICON_MEMBERSHIP       73+TB_ICON_USER // ��������
	#define TB_ICON_NEWFOLDER        74+TB_ICON_USER //
	#define TB_ICON_COMPARE          75+TB_ICON_USER // ���������
	#define TB_ICON_REGISTER         76+TB_ICON_USER // �������� ����������
	#define TB_ICON_SELLTAB          77+TB_ICON_USER // ������� ������
	#define TB_ICON_PERSONRELLIST    78+TB_ICON_USER // ������ ��������� ����������
	#define TB_ICON_ADDPERSONREL     79+TB_ICON_USER // �������� ��������� ����������
	#define TB_ICON_TRFRANLZ         80+TB_ICON_USER // ������ �������� ��������
	#define TB_ICON_MRPTAB           81+TB_ICON_USER // MRP-�������
	#define TB_ICON_PRJTASK          82+TB_ICON_USER // ������� //
	#define TB_ICON_BARLABEL         83+TB_ICON_USER // ������� //
	#define TB_ICON_SUPPLQUOTS       84+TB_ICON_USER // ����������� ���� �����������
	#define TB_ICON_TECH             85+TB_ICON_USER // ����������
	#define TB_ICON_PRC              86+TB_ICON_USER // ���������
	#define TB_ICON_TSESS            87+TB_ICON_USER // ��������������� ������
	#define TB_ICON_DFCTBILLS        88+TB_ICON_USER // ��������� �������� ��������
	#define TB_ICON_TSESSANLZ        89+TB_ICON_USER // ������ ��������������� ������
	#define TB_ICON_FOLDER   	     90+TB_ICON_USER //
	#define TB_ICON_MAIL		     91+TB_ICON_USER //
	#define TB_ICON_ABC              92+TB_ICON_USER // ����� �� ������������
	#define TB_ICON_STAFFCAL         93+TB_ICON_USER // ������� ���������
	#define TB_ICON_POSPRINT         94+TB_ICON_USER // ������ �� POS-�������
	#define TB_ICON_REPLACE          95+TB_ICON_USER // ������ ������
	#define TB_ICON_LINK             96+TB_ICON_USER // �����
	#define TB_ICON_HANDSHAKE        97+TB_ICON_USER // �����������
	#define TB_ICON_PSNOPKIND        98+TB_ICON_USER // ��� ������������ ��������
	#define TB_ICON_PSNEVENTS        99+TB_ICON_USER // ������������ ��������
	#define TB_ICON_MARK            100+TB_ICON_USER // �������
	#define TB_ICON_UNITE           101+TB_ICON_USER // ����������� ��������
	#define TB_ICON_ADDBILLBYSCARD  102+TB_ICON_USER // ������� �������� �� �����
	#define TB_ICON_SWITCH          103+TB_ICON_USER // �������������
	#define TB_ICON_OPLIST          104+TB_ICON_USER // ���� ��������
	#define TB_ICON_TIMEGRAPH       105+TB_ICON_USER // ��������� ���������
	#define TB_ICON_MATRIX          106+TB_ICON_USER // ������� (� ���������, ��������)
	#define TB_ICON_ARCODE          107+TB_ICON_USER // ���� ������ �� �������
	#define TB_ICON_AMOUNTS         108+TB_ICON_USER // �����
	#define TB_ICON_SALARY          109+TB_ICON_USER // �������� ���������� ��������
	#define TB_ICON_PERSONRELLIST_R 110+TB_ICON_USER // ������ �������� ��������� ����������
	#define TB_ICON_PLOT            111+TB_ICON_USER // ������
	#define TB_ICON_BIZSCORE        112+TB_ICON_USER // �������� ������-�����������
	#define TB_ICON_CONFIG          113+TB_ICON_USER // ������������ //
	#define TB_ICON_TRANSPORT       114+TB_ICON_USER // ��������� //
	#define TB_ICON_CURPERSONATTR   115+TB_ICON_USER // �������������� �������� �������� ���������� //
	#define TB_ICON_WROFFDRAFT      116+TB_ICON_USER // �������� �����-����������
	#define TB_ICON_WHCELL          117+TB_ICON_USER // ��������� ������
	#define TB_ICON_DEVICE          118+TB_ICON_USER // ����������
	#define TB_ICON_SERIALGEN       119+TB_ICON_USER // ����������� �������� �������
	#define TB_ICON_TAGSALL         120+TB_ICON_USER // �������� ���� �� ���� ������� ��������
	#define TB_ICON_EXPORTUHTT      121+TB_ICON_USER // ������� � Universe-HTT
	#define TB_ICON_ARROWUP         122+TB_ICON_USER
	#define TB_ICON_ARROWDN         123+TB_ICON_USER
	#define TB_ICON_STAT            124+TB_ICON_USER
	#define TB_ICON_PANE            125+TB_ICON_USER
	#define TB_ICON_TEST2           126+TB_ICON_USER // �����
	#define TB_ICON_MEMO            127+TB_ICON_USER

#define TB_GLOBAL            302
	#define TB_GLOBAL_NOICON      0+TB_ICON_USER
	#define TB_GLOBAL_ACCBILLS    1+TB_ICON_USER
	#define TB_GLOBAL_GOODSBILLS  2+TB_ICON_USER
	#define TB_GLOBAL_ORDERBILLS  3+TB_ICON_USER
	#define TB_GLOBAL_INVENTORY   4+TB_ICON_USER
	#define TB_GLOBAL_ATURNLIST   5+TB_ICON_USER
	#define TB_GLOBAL_ATURNLIST2  6+TB_ICON_USER
	#define TB_GLOBAL_ACCANLZ     7+TB_ICON_USER
	#define TB_GLOBAL_LOTS        8+TB_ICON_USER
	#define TB_GLOBAL_ORDLOTS     9+TB_ICON_USER
	#define TB_GLOBAL_GOODSREST  10+TB_ICON_USER
	#define TB_GLOBAL_UNIFYPRICE 11+TB_ICON_USER
	#define TB_GLOBAL_UNCHECKED  12+TB_ICON_USER
	#define TB_GLOBAL_CHECKED    13+TB_ICON_USER
	#define TB_GLOBAL_OPGRPNG    14+TB_ICON_USER
	#define TB_GLOBAL_PRICELIST  15+TB_ICON_USER
	#define TB_GLOBAL_GOODSVIEW  16+TB_ICON_USER
	#define TB_GLOBAL_GOODSVIEW2 17+TB_ICON_USER
	#define TB_GLOBAL_GGRPVIEW   18+TB_ICON_USER
	#define TB_GLOBAL_PERSONS    19+TB_ICON_USER
	#define TB_GLOBAL_GOODSANLZ  20+TB_ICON_USER
	#define TB_GLOBAL_SENDOBJ    21+TB_ICON_USER
	#define TB_GLOBAL_BALANCE    22+TB_ICON_USER
	#define TB_GLOBAL_CASHBOOK   23+TB_ICON_USER
	#define TB_GLOBAL_VATBOOK    24+TB_ICON_USER
	#define TB_GLOBAL_BASKET     25+TB_ICON_USER
	#define TB_GLOBAL_SCARD      26+TB_ICON_USER // ����������� �����
	#define TB_GLOBAL_DRAFTBILLS 27+TB_ICON_USER // �����-���������
	#define TB_GLOBAL_OPLIST     28+TB_ICON_USER // ���� ��������
	#define TB_GLOBAL_TRFRANLZ   29+TB_ICON_USER // ������ �������� ��������
	#define TB_GLOBAL_STAFFCAL   30+TB_ICON_USER // ������� ���������
	#define TB_GLOBAL_PSNOPKIND  31+TB_ICON_USER // ���� ������������ ��������
	#define TB_GLOBAL_PSNEVENTS  32+TB_ICON_USER // ������������ ��������

#endif /* __TVDEFS_H */