# File: main-window.tcl

# Purpose: the Main Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSMainWindow {

	variable Priv
	variable Progress
	
	variable tracking 0
	variable trackId 0
	variable trackStepping 0
	variable trackX
	variable trackY
	variable trackHPfrac

# namespace eval NSMainWindow
}

# NSMainWindow::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#
# Results:
#	What happened.

proc NSMainWindow::InitModule {} {

	global Display
	global PYPX

	MsgCatInit main
	
	NSModule::LoadIfNeeded NSMap
	NSModule::LoadIfNeeded NSWidget

	# The character's position
	set PYPX "0 0"

	# Keep track of active window (inventory, book, etc)
	set Display(window) none

	# Create the main window
	NSObject::New NSMainWindow

	return
}

# NSMainWindow::NSMainWindow --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::NSMainWindow {oop} {

	InitWindow $oop

	# Window positions
	Info $oop window,autosave [Value window,autosave]

	#
	# Global access
	#

	Window main [Info $oop win]
	Global main,oop $oop

InitAutobar $oop

	return
}

# NSMainWindow::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::Info {oop info args} {

	global NSMainWindow

	# Verify the object
	NSObject::CheckObject NSMainWindow $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSMainWindow($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			default {
				return $NSMainWindow($oop,$info)
			}
		}
	}

	return
}

# NSMainWindow::InitWindow --
#
#	Creates the Main Window.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::InitWindow {oop} {

	global Angband

	set win .main$oop
	toplevel $win
	wm title $win "[mc Main] - ZAngband"

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSMainWindow::Close $oop"

	# Start out withdrawn (hidden)
	wm withdraw $win

	# Remember the window
	Info $oop win $win

	# Create the menus
	InitMenus $oop

	set frame $win.divider2
	MakeDivider $frame x

	#
	# Statusbar
	# There is a level of tomfoolery with the statusbar to prevent
	# a really long message causing the Main Window to change size.
	# This is in spite of the fact that many other windows do not
	# change size with long statusbar labels. I thought gridded
	# geometry solved the problem, but not in this case...
	#
	# The hack involves pack'ing the label in a frame, and turning
	# off pack propagation for that frame. Oh well.
	#

	# Font for all statusbars
	set font [Value font,statusBar]

	frame $win.statusBar \
		-borderwidth 0
	frame $win.statusBar.frameLabel \
		-borderwidth 0
	label $win.statusBar.frameLabel.label \
		-anchor w -text "Hello world!" -relief sunken -padx 2 \
		-foreground [Value main,statusbar,color] -background Black -font $font
	label $win.statusBar.center \
		-text "C" -relief sunken -width 2 -padx 0 -foreground White \
		-background Black -font $font
	label $win.statusBar.depth \
		-relief sunken -width 12 -padx 2 \
		-foreground White -background Black -font $font

	bind $win.statusBar.frameLabel.label <ButtonPress-3> \
		"NSMainWindow::ContextMenu_StatusBar $win.context %X %Y"

	# Used in various places
	Global main,statusBar $win.statusBar.frameLabel.label
	
	# Hack
	pack $win.statusBar.frameLabel.label -fill x
	pack propagate $win.statusBar.frameLabel no

	grid columnconfigure $win.statusBar 0 -weight 1
	grid columnconfigure $win.statusBar 1 -weight 0
	grid columnconfigure $win.statusBar 2 -weight 0
	grid rowconfigure $win.statusBar 0 -weight 0
	
	grid $win.statusBar.frameLabel \
		-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid $win.statusBar.center \
		-row 0 -column 1 -rowspan 1 -columnspan 1
	grid $win.statusBar.depth \
		-row 0 -column 2 -rowspan 1 -columnspan 1

	# The small "C" in the status bar can be clicked to recenter the
	# Main widget and Micro Map widget on the character's location.
	bind $win.statusBar.center <ButtonPress-1> {
		Global main,widget,center [WidgetCenter [Global main,widget]]
		WidgetCenter [Global micromap,widget]
	}
	bind $win.statusBar.center <Enter> "
		%W configure -foreground gray60
		NSMainWindow::StatusText $oop {Click to recenter the display.}
	"
	bind $win.statusBar.center <Leave> "
		%W configure -foreground White
		NSMainWindow::StatusText $oop {}
	"

	# Update ourself when the font,statusBar value changes
	NSValueManager::AddClient font,statusBar \
		"NSMainWindow::ValueChanged_font_statusBar"

	#
	# Message line when Message Window is closed
	#

	set frame $win.message
	frame $frame -background black -borderwidth 1 -relief sunken

	#
	# Misc info when Misc Window is closed
	#

	set frame $win.misc
	frame $frame -background black -borderwidth 1 -relief sunken

	#
	# Main widget
	#

	# Black background affects border color
	frame $win.mainframe \
		-borderwidth 1 -relief sunken -background Black

	# Get the icon dimensions
	set gsize [icon size]

	# This is a large monitor
	if {[winfo screenwidth .] >= 800} {
		set width [expr {15 * 32}]
		set height [expr {11 * 32}]

	# This is a small monitor
	} else {
		set width [expr {13 * 32}]
		set height [expr {9 * 32}]
	}

	set widgetId [NSObject::New NSWidget $win.mainframe \
		$width $height $gsize $gsize]
	NSWidget::Info $widgetId leaveCmd NSMainWindow::Leave
	set widget [NSWidget::Info $widgetId widget]

	bind $widget <ButtonPress-1> "NSMainWindow::TrackPress $oop %x %y"
	bind $widget <Button1-Motion> "NSMainWindow::TrackMotion $oop %x %y"
	bind $widget <ButtonRelease-1> "NSMainWindow::TrackRelease $oop"

	bind $widget <Control-ButtonPress-1> "NSMainWindow::MouseCommand $oop %x %y +"
	bind $widget <Shift-ButtonPress-1> "NSMainWindow::MouseCommand $oop %x %y ."

	bind $win <Control-KeyPress-Shift_L> \
		"$widget configure -cursor fleur"
	bind $win <Control-KeyRelease-Shift_L> \
		"$widget configure -cursor {}"
	bind $win <Shift-KeyPress-Control_L> \
		"$widget configure -cursor fleur"
	bind $win <Shift-KeyRelease-Control_L> \
		"$widget configure -cursor {}"
	bind $widget <Control-Shift-ButtonPress-1> "
		NSWidget::Info $widgetId track,x %x
		NSWidget::Info $widgetId track,y %y
		NSWidget::Info $widgetId track,mouseMoved 0
	"
	bind $widget <Control-Shift-Button1-Motion> \
		"NSWidget::TrackOnce $widgetId %x %y"

	bind $widget <ButtonPress-3> \
		"NSMainWindow::ButtonPress3 $oop %x %y %X %Y"
	bind $widget <Control-ButtonPress-3> \
		"NSRecall::PopupSelect_Use $win.context %X %Y"

	# When the pointer leaves the Main Window Widget, we clear the
	# statusbar text, in addition to the behaviour defined by the
	# NSWidget module.
#	bind $widget <Leave> "+NSMainWindow::StatusText $oop {}"

	# Remember the center of the Main Window Widget.
	Global main,widget,center [angband player position]
	
# Debug: draw 1 spot
proc ::wipespot {y x} {
	scan [[Global main,widget] caveyx $x $y] "%d %d" y x
	[Global main,widget] wipespot $y $x
}
bind $widget <Shift-ButtonPress-3> "wipespot %y %x"


	variable HT ""

	# The "big map", the map of the entire cave with scroll bars.
	# The user can change the scale via a popup menu, so we save
	# the desired scale.
	set scale [Value bigmap,scale]
	set width [expr {[$widget cget -width] - 16}]
	set height [expr {[$widget cget -height] - 16}]
	set mapId [NSObject::New NSMap $widget $width $height $scale $scale]
	set widget2 [NSMap::Info $mapId widget]

	NSMap::Info $mapId scaleCmd \
		"Value bigmap,scale \[NSWidget::Info [NSMap::Info $mapId widgetId] scale]"

	bind $widget2 <Leave> {+
		[Global mapdetail,widget] center -100 -100
		NSMainWindow::StatusText [Global main,oop] {}
	}

	# Hide the Big Map when clicked (but not dragged)
	bind $widget2 <ButtonRelease-1> {
		if {![NSWidget::Info [Global bigmap,widgetId] track,mouseMoved]} {
			angband keypress \033
		}
	}

	# Each NSMap widget has Left/Right etc bindings. Need this to
	# hide the map.
	bind $widget2 <KeyPress-Escape> {
		angband keypress \033
	}

	# Global access
	Global main,widgetId $widgetId
	Global main,widget $widget
	Global bigmap,mapId $mapId
	Global bigmap,widgetId [NSMap::Info $mapId widgetId]
	Global bigmap,widget [NSMap::Info $mapId widget]

	# This binding is called whenever the Main Window is resized
	# by the user.
	bind $widget <Configure> \
		"NSMainWindow::Configure $oop %w %h"

	pack $widget -expand yes -fill both

	#
	# Geometry
	#

	grid rowconfigure $win 0 -weight 0
	grid rowconfigure $win 1 -weight 0
	grid rowconfigure $win 2 -weight 1
	grid rowconfigure $win 3 -weight 0
	grid columnconfigure $win 0 -weight 0
	grid columnconfigure $win 1 -weight 1

	grid $win.divider2 \
		-row 0 -column 0 -rowspan 1 -columnspan 2 -sticky ew
	grid $win.message \
		-row 1 -column 0 -columnspan 2 -sticky we
	grid $win.misc \
		-row 2 -column 0 -sticky ns
	grid $win.mainframe \
		-row 2 -column 1 -rowspan 1 -columnspan 1 -sticky news
	grid $win.statusBar \
		-row 3 -column 0 -rowspan 1 -columnspan 2 -sticky ew

	#
	# Context menu
	#

	menu $win.context -tearoff 0

	#
	# Feed Term when keys pressed
	#

	Term_KeyPress_Bind $win

	# Hack -- Visual feedback of whether the target is set or not.
	TargetSetup $oop

	# The monster health bar
	ProgressSetup $widget

	return
}

proc NSMainWindow::InitAutobar {oop} {

	set statusBar [Global main,statusBar]

	bind $statusBar <Enter> \
		"NSMainWindow::ShowAutobar $oop"

	return
}

proc NSMainWindow::ShowAutobar {oop} {

	# Allow easy rebooting of the module
	if {[NSModule::LoadIfNeeded NSAutobar]} {

		set autobarId [Global autobar,oop]
		set statusBar [Global main,statusBar]
	
		bind $statusBar <Leave> \
			"NSAutobar::Event $autobarId leave-status"
	}

	set autobarId [Global autobar,oop]
	NSAutobar::Event $autobarId enter-status

	return
}

# NSMainWindow::InitMenus --
#
#	Initialize the menus for the Main Window.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::InitMenus {oop} {

	global Angband

	set win [Info $oop win]

	set mbarId [NSObject::New NSMenu $win -tearoff 0 \
		-postcommand "NSMainWindow::SetupMenus $oop" -identifier MENUBAR]

	# Call our command when an entry is invoked
	NSMenu::Info $mbarId invokeCmd "NSMainWindow::MenuInvoke $oop"

	Info $oop mbarId $mbarId

	#
	# File Menu
	#

	NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_FILE
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_FILE -label [mc File] -underline 0 -identifier M_FILE

	set entries {}
	lappend entries [list -type command -label [mc Save] -identifier E_GAME_SAVE]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc "Quit With Save"] -identifier E_GAME_EXIT]
	lappend entries [list -type command -label [mc Quit] -identifier E_GAME_ABORT]

	NSMenu::MenuInsertEntries $mbarId -end MENU_FILE $entries

	#
	# Inven Menu
	#

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_INVEN]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_INVEN -label [mc Inven] -underline 0 -identifier M_INVEN

	# Magic Menu
	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_MAGIC]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	set entries {}
	lappend entries [list -type command -label [mc Activate] -identifier E_MAGIC_ACTIVATE]
	lappend entries [list -type command -label [mc "Aim Wand"] -identifier E_MAGIC_WAND]
	lappend entries [list -type command -label [mc "Drink Potion"] -identifier E_MAGIC_POTION]
	lappend entries [list -type command -label [mc "Read Scroll"] -identifier E_MAGIC_SCROLL]
	lappend entries [list -type command -label [mc "Use Staff"] -identifier E_MAGIC_STAFF]
	lappend entries [list -type command -label [mc "Zap Rod"] -identifier E_MAGIC_ROD]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Browse] -identifier E_MAGIC_BROWSE]
	lappend entries [list -type command -label [mc Study] -identifier E_MAGIC_STUDY]

	NSMenu::MenuInsertEntries $mbarId -end MENU_MAGIC $entries

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_USE]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	set entries {}
	lappend entries [list -type command -label [mc Destroy] -identifier E_USE_DESTROY]
	lappend entries [list -type command -label [mc Drop] -identifier E_USE_DROP]
	lappend entries [list -type command -label [mc "Pick Up"] -identifier E_USE_PICKUP]
	lappend entries [list -type command -label [mc "Take Off"] -identifier E_USE_TAKEOFF]
	lappend entries [list -type command -label [mc Wear/Wield] -identifier E_USE_WIELD]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc "Eat Food"] -identifier E_USE_FOOD]
	lappend entries [list -type command -label [mc "Fire Missle"] -identifier E_USE_MISSILE]
	lappend entries [list -type command -label [mc "Fuel Light"] -identifier E_USE_FUEL]
	lappend entries [list -type command -label [mc "Jam Spike"] -identifier E_USE_SPIKE]
	lappend entries [list -type command -label [mc Throw] -identifier E_USE_THROW]

	NSMenu::MenuInsertEntries $mbarId -end MENU_USE $entries

	set entries {}
	lappend entries [list -type command -label [mc Equipment] -identifier E_INVEN_EQUIPMENT]
	lappend entries [list -type command -label [mc Inventory] -identifier E_INVEN_INVENTORY]
	lappend entries [list -type separator]
 	lappend entries [list -type cascade -menu MENU_MAGIC -label [mc Magic] -identifier M_MAGIC]
	lappend entries [list -type cascade -menu MENU_USE -label [mc Use] -identifier M_USE]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Inspect] -identifier E_INVEN_INSPECT]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Inscribe] -identifier E_INVEN_INSCRIBE]
	lappend entries [list -type command -label [mc Uninscribe] -identifier E_INVEN_UNINSCRIBE]

	NSMenu::MenuInsertEntries $mbarId -end MENU_INVEN $entries

	NSModule::LoadIfNeeded NSMindcraftMenu
	NSObject::New NSMindcraftMenu $mbarId

	#
	# Action Menu
	#

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_ACTION]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_ACTION -label [mc Action] -underline 0 -identifier M_ACTION

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_ACTION_ALTER]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	set entries {}
	lappend entries [list -type command -label [mc Alter] -identifier E_ACTION_ALTER]
	lappend entries [list -type command -label [mc Bash] -identifier E_ACTION_BASH]
	lappend entries [list -type command -label [mc Close] -identifier E_ACTION_CLOSE]
	lappend entries [list -type command -label [mc Disarm] -identifier E_ACTION_DISARM]
	lappend entries [list -type command -label [mc Open] -identifier E_ACTION_OPEN]
	lappend entries [list -type command -label [mc Tunnel] -identifier E_ACTION_TUNNEL]
	NSMenu::MenuInsertEntries $mbarId -end MENU_ACTION_ALTER $entries

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_ACTION_LOOKING]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	set entries {}
	lappend entries [list -type command -label [mc Look] -identifier E_ACTION_LOOK]
	lappend entries [list -type command -label [mc Map] -identifier E_ACTION_MAP]
	NSMenu::MenuInsertEntries $mbarId -end MENU_ACTION_LOOKING $entries

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_ACTION_RESTING]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	set entries {}
	lappend entries [list -type command -label [mc Rest] -identifier E_ACTION_REST]
	lappend entries [list -type command -label [mc "Stay (With Pickup)"] -identifier E_ACTION_STAY]
	lappend entries [list -type command -label [mc "Stay"] -identifier E_ACTION_STAY_TOGGLE]
	NSMenu::MenuInsertEntries $mbarId -end MENU_ACTION_RESTING $entries

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_ACTION_SEARCHING]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	set entries {}
	lappend entries [list -type command -label [mc Search] -identifier E_ACTION_SEARCH]
	lappend entries [list -type command -label [mc "Search Mode"] -identifier E_ACTION_SEARCH_MODE]
	NSMenu::MenuInsertEntries $mbarId -end MENU_ACTION_SEARCHING $entries

	set menuId [NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_ACTION_MOVEMENT]
	NSMenu::Info $menuId setupCmd "NSMainWindow::MenuSetupCmd $oop"
	set entries {}
	lappend entries [list -type command -label [mc "Go Down"] -identifier E_ACTION_DOWN]
	lappend entries [list -type command -label [mc "Go Up"] -identifier E_ACTION_UP]
	lappend entries [list -type command -label [mc Run] -identifier E_ACTION_RUN]
	lappend entries [list -type command -label [mc "Walk (With Pickup)"] -identifier E_ACTION_WALK]
	lappend entries [list -type command -label [mc "Walk"] -identifier E_ACTION_WALK_TOGGLE]
	NSMenu::MenuInsertEntries $mbarId -end MENU_ACTION_MOVEMENT $entries

	set entries {}
 	lappend entries [list -type cascade -menu MENU_ACTION_ALTER -label [mc Alter] -identifier M_ACTION_ALTER]
 	lappend entries [list -type cascade -menu MENU_ACTION_LOOKING -label [mc Looking] -identifier M_ACTION_LOOKING]
 	lappend entries [list -type cascade -menu MENU_ACTION_MOVEMENT -label [mc Movement] -identifier M_ACTION_MOVEMENT]
 	lappend entries [list -type cascade -menu MENU_ACTION_RESTING -label [mc Resting] -identifier M_ACTION_RESTING]
 	lappend entries [list -type cascade -menu MENU_ACTION_SEARCHING -label [mc Searching] -identifier M_ACTION_SEARCHING]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Note] -identifier E_ACTION_NOTE]
	lappend entries [list -type command -label [mc Repeat] -identifier E_ACTION_REPEAT]
	lappend entries [list -type command -label [mc Target] -identifier E_ACTION_TARGET]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Pets] -identifier E_ACTION_PETS]
	lappend entries [list -type command -label [mc "Use Power"] -identifier E_ACTION_POWER]

	NSMenu::MenuInsertEntries $mbarId -end MENU_ACTION $entries

	#
	# Other Menu
	#

	NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_OTHER
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_OTHER -label [mc Other] -underline 0 -identifier M_OTHER

	NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_PREFERENCES
	set entries {}
	lappend entries [list -type command -label [mc Alternate] -identifier E_PREF_ALTERNATE]
	lappend entries [list -type command -label [mc Color] -identifier E_PREF_COLOR]
	lappend entries [list -type command -label [mc Font] -identifier E_PREF_FONT]
	lappend entries [list -type command -label [mc Keymap] -identifier E_PREF_KEYMAP]
	lappend entries [list -type command -label [mc Macros] -identifier E_PREF_MACROS]
	lappend entries [list -type command -label [mc Options] -identifier E_PREF_OPTIONS]
	lappend entries [list -type command -label [mc Sprite] -identifier E_PREF_SPRITE]
	NSMenu::MenuInsertEntries $mbarId -end MENU_PREFERENCES $entries

	set entries {}
	lappend entries [list -type command -label [mc "Character Info"] -identifier E_OTHER_INFO]
	lappend entries [list -type command -label [mc Feeling] -identifier E_OTHER_FEELING]
	lappend entries [list -type command -label [mc "File Character"] -identifier E_OTHER_FILE]
	lappend entries [list -type command -label [mc "High Scores"] -identifier E_OTHER_SCORE]
	lappend entries [list -type command -label [mc Knowledge] -identifier E_OTHER_KNOWLEDGE]
	lappend entries [list -type command -label [mc "Message History"] -identifier E_OTHER_MESSAGES]
	lappend entries [list -type cascade -menu MENU_PREFERENCES -label [mc Preferences] -identifier M_PREFERENCES]
	lappend entries [list -type command -label [mc "Quest Status"] -identifier E_OTHER_QUEST]
	lappend entries [list -type command -label [mc "Time Of Day"] -identifier E_OTHER_TIME]

	NSMenu::MenuInsertEntries $mbarId -end MENU_OTHER $entries

	#
	# Window Menu
	#

	NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_WINDOW
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_WINDOW -label [mc Window] -underline 0 -identifier M_WINDOW

	set entries {}
	lappend entries [list -type command -label [mc "Arrange Windows..."] -identifier E_WINDOW_DEFPOS]
	lappend entries [list -type command -label [mc "Maximize Windows..."] -identifier E_WINDOW_MAXIMIZE]
		lappend entries [list -type separator]
	lappend entries [list -type command -label [mc "Save Window Positions"] -identifier E_WINDOW_SAVEPOS]
	lappend entries [list -type command -label [mc "Load Window Positions"] -identifier E_WINDOW_LOADPOS]
	lappend entries [list -type checkbutton -label [mc "AutoSave Positions"] \
		-variable ::NSMainWindow($oop,window,autosave) -identifier E_WINDOW_AUTOSAVE]
	if {[file exists [PathTk choice-window.tcl]]} {
		Info $oop choiceWindow [Value choicewindow,show]
		lappend entries [list -type separator]
		lappend entries [list -type checkbutton -label [mc "Choice Window"] \
			-variable ::NSMainWindow($oop,choiceWindow) -identifier E_CHOICEWINDOW]
	}
	Info $oop messageWindow [Value message,float]
	lappend entries [list -type checkbutton -label [mc "Message Window"] \
		-variable ::NSMainWindow($oop,messageWindow) -identifier E_WINDOW_MESSAGE]
	Info $oop messagesWindow 0
	lappend entries [list -type checkbutton -label [mc "Messages Window"] \
		-variable ::NSMainWindow($oop,messagesWindow) -identifier E_WINDOW_MESSAGES]
	Info $oop mapWindow [Value micromap,float]
	lappend entries [list -type checkbutton -label [mc "Micro Map Window"] \
		-variable ::NSMainWindow($oop,mapWindow) -identifier E_WINDOW_MAP]
	Info $oop miscWindow [Value misc,float]
	lappend entries [list -type checkbutton -label [mc "Misc Window"] \
		-variable ::NSMainWindow($oop,miscWindow) -identifier E_WINDOW_MISC]
if 0 {
	lappend entries [list -type checkbutton -label [mc "Progress Window"] \
		-variable ::NSMainWindow($oop,progressWindow) \
		-identifier E_WINDOW_PROGRESS]
}
	Info $oop recallWindow [Value recall,show]
	lappend entries [list -type checkbutton -label [mc "Recall Window"] \
		-variable ::NSMainWindow($oop,recallWindow) -identifier E_WINDOW_RECALL]
	NSMenu::MenuInsertEntries $mbarId -end MENU_WINDOW $entries

	#
	# Help Menu
	#

	NSObject::New NSMenu $mbarId -tearoff 0 -identifier MENU_HELP
	NSMenu::MenuInsertEntry $mbarId -end MENUBAR -type cascade \
		-menu MENU_HELP -label [mc Help] -underline 0 -identifier M_HELP

	set entries {}
	lappend entries [list -type command -label [mc Help] -identifier E_HELP]
	lappend entries [list -type command -label [mc Tips] -identifier E_TIPS]
	lappend entries [list -type separator]
	lappend entries [list -type command \
		-label "About ZAngband..." -identifier E_ABOUT]

	NSMenu::MenuInsertEntries $mbarId -end MENU_HELP $entries
	
	NSMenu::SetIdentArray $mbarId

	return
}

# NSMainWindow::SetupMenus --
#
#	Called by NSMenus::_MenuPostCommand() to enable menu items before
#	posting a menu.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#	mbarId					OOP ID of NSMenu object (the menubar).
#
# Results:
#	What happened.

proc NSMainWindow::SetupMenus {oop mbarId} {

	global Windows

	lappend identList E_WINDOW_SAVEPOS E_WINDOW_DEFPOS \
		E_WINDOW_LOADPOS E_WINDOW_AUTOSAVE E_WINDOW_MAXIMIZE E_ABOUT E_TIPS
	lappend identList M_PREFERENCES E_PREF_COLOR \
		E_PREF_FONT E_PREF_KEYMAP E_PREF_SPRITE \
		E_PREF_ALTERNATE

	lappend identList E_CHOICEWINDOW E_WINDOW_MESSAGE E_WINDOW_MESSAGES \
		E_WINDOW_MAP E_WINDOW_MISC E_WINDOW_RECALL
	if {[info exists Windows(choice)]} {
		Info $oop choiceWindow [winfo ismapped [Window choice]]
	}
	Info $oop messageWindow [winfo ismapped [Window message]]
	if {[info exists Windows(message2)]} {
		Info $oop messagesWindow [winfo ismapped [Window message2]]
	}
	Info $oop mapWindow [winfo ismapped [Window micromap]]
	Info $oop miscWindow [winfo ismapped [Window misc]]
	Info $oop recallWindow [winfo ismapped [Window recall]]

	if {[string equal [angband inkey_flags] INKEY_CMD]} {
		lappend identList E_GAME_SAVE E_GAME_EXIT E_OTHER_FEELING \
			E_OTHER_INFO E_OTHER_FILE E_OTHER_KNOWLEDGE E_PREF_MACROS \
			E_OTHER_MESSAGES E_PREF_OPTIONS E_OTHER_SCORE E_HELP \
			E_OTHER_QUEST E_OTHER_TIME
	}

	lappend identList E_GAME_ABORT

	NSMenu::MenuEnable $mbarId $identList

	return
}

# NSMainWindow::MenuSetupCmd --
#
#	Called when a menu is about to be posted. We use this to change the
#	setupMode of a menu so we don't need to pass a huge list of identifiers
#	to the MenuEnable() command.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::MenuSetupCmd {oop menuId} {

	if {[string compare [angband inkey_flags] INKEY_CMD]} {
		NSMenu::Info $menuId setupMode disabled
	} else {
		NSMenu::Info $menuId setupMode normal
	}
	
	return
}

# NSMainWindow::MenuInvoke --
#
#	Called when a menu entry is invoked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::MenuInvoke {oop menuId ident} {

	switch -glob -- $ident {

		E_GAME_SAVE {DoUnderlyingCommand ^s}
		E_GAME_EXIT {DoUnderlyingCommand ^x}
		E_GAME_ABORT {QuitNoSave}

		E_MAGIC_ACTIVATE {DoUnderlyingCommand A}
		E_MAGIC_WAND {DoUnderlyingCommand a}
		E_MAGIC_POTION {DoUnderlyingCommand q}
		E_MAGIC_SCROLL {DoUnderlyingCommand r}
		E_MAGIC_STAFF {DoUnderlyingCommand u}
		E_MAGIC_ROD {DoUnderlyingCommand z}
		E_MAGIC_BROWSE {DoUnderlyingCommand b}
		E_MAGIC_STUDY {DoUnderlyingCommand G}

		E_USE_DESTROY {DoUnderlyingCommand k}
		E_USE_DROP {DoUnderlyingCommand d}
		E_USE_PICKUP {DoUnderlyingCommand g}
		E_USE_TAKEOFF {DoUnderlyingCommand t}
		E_USE_WIELD {DoUnderlyingCommand w}
		E_USE_FOOD {DoUnderlyingCommand E}
		E_USE_MISSILE {DoUnderlyingCommand f}
		E_USE_FUEL {DoUnderlyingCommand F}
		E_USE_SPIKE {DoUnderlyingCommand j}
		E_USE_THROW {DoUnderlyingCommand v}

		E_INVEN_EQUIPMENT {DoUnderlyingCommand e}
		E_INVEN_INVENTORY {DoUnderlyingCommand i}
		E_INVEN_INSPECT {DoUnderlyingCommand I}
		E_INVEN_INSCRIBE {DoUnderlyingCommand \{}
		E_INVEN_UNINSCRIBE {DoUnderlyingCommand \}}

		E_ACTION_ALTER {DoUnderlyingCommand +}
		E_ACTION_BASH {DoUnderlyingCommand B}
		E_ACTION_CLOSE {DoUnderlyingCommand c}
		E_ACTION_DISARM {DoUnderlyingCommand D}
		E_ACTION_DOWN {DoUnderlyingCommand >}
		E_ACTION_OPEN {DoUnderlyingCommand o}
		E_ACTION_LOOK {DoUnderlyingCommand l}
		E_ACTION_MAP {DoUnderlyingCommand M}
		E_ACTION_NOTE {DoUnderlyingCommand :}
		E_ACTION_SHAPE {DoUnderlyingCommand \]}
		E_ACTION_PETS {DoUnderlyingCommand p}
		E_ACTION_POWER {DoUnderlyingCommand U}
		E_ACTION_REPEAT {DoUnderlyingCommand n}
		E_ACTION_REST {DoUnderlyingCommand R}
		E_ACTION_RUN {DoUnderlyingCommand .}
		E_ACTION_SEARCH {DoUnderlyingCommand s}
		E_ACTION_SEARCH_MODE {DoUnderlyingCommand S}
		E_ACTION_STAY {DoUnderlyingCommand ,}
		E_ACTION_STAY_TOGGLE {DoUnderlyingCommand g}
		E_ACTION_TARGET {DoUnderlyingCommand *}
		E_ACTION_TUNNEL {DoUnderlyingCommand T}
		E_ACTION_UP {DoUnderlyingCommand <}
		E_ACTION_WALK {DoUnderlyingCommand ";"}
		E_ACTION_WALK_TOGGLE {DoUnderlyingCommand -}
		
		E_PREF_ALTERNATE {
			NSModule::LoadIfNeeded NSAlternate
			NSWindowManager::Display alternate
		}
		E_PREF_COLOR {
			NSModule::LoadIfNeeded NSColorPreferences
			NSWindowManager::Display color
		}
		E_PREF_FONT {
			NSModule::LoadIfNeeded NSFont
			NSWindowManager::Display font
		}
		E_PREF_KEYMAP {
			NSModule::LoadIfNeeded NSKeymap
			NSWindowManager::Display keymap
		}
		E_PREF_COLOR {
			NSModule::LoadIfNeeded NSColorPreferences
			NSWindowManager::Display color
		}
		E_PREF_MACROS {DoUnderlyingCommand @}
		E_PREF_OPTIONS {DoUnderlyingCommand =}
		E_PREF_SPRITE {
			NSModule::LoadIfNeeded NSSprite
			NSWindowManager::Display sprite
		}
		E_PREF_SQUELCH {
			NSModule::LoadIfNeeded NSSquelch
			NSWindowManager::Display squelch
		}

		E_OTHER_INFO {DoUnderlyingCommand C}
		E_OTHER_FEELING {DoUnderlyingCommand ^F}
		E_OTHER_FILE {FileCharacter [Info $oop win]}
		E_OTHER_SCORE {
			NSModule::LoadIfNeeded NSHighScore
			NSHighScore::Info [Global highscore,oop] interactive 0
			NSWindowManager::Display highscore
		}
		E_OTHER_KNOWLEDGE {DoUnderlyingCommand ~}
		E_OTHER_MESSAGES {DoUnderlyingCommand ^p}
		E_OTHER_QUEST {DoUnderlyingCommand ^Q}
		E_OTHER_TIME {DoUnderlyingCommand ^T}

		E_WINDOW_DEFPOS {
			set title [mc dialog-title-defpos]
			set message [mc dialog-msg-defpos]
			set answer [tk_messageBox -parent [Info $oop win] -type yesno \
				-icon question -title $title -message $message]
			if {[string equal $answer yes]} {
				HardcodeGeometry
			}
		}
		E_WINDOW_MAXIMIZE {
			set title [mc dialog-title-max]
			set message [mc dialog-msg-max]
			set answer [tk_messageBox -parent [Info $oop win] -type yesno \
				-icon question -title $title -message $message]
			if {[string equal $answer yes]} {
				MaximizeWindows
			}
		}
		E_WINDOW_SAVEPOS {WriteGeometryFile}
		E_WINDOW_LOADPOS {
			set title [mc dialog-title-loadpos]
			if {![file exists [PathTk config geometry]]} {
				set message [mc dialog-msg-loadpos-fail]
				tk_messageBox -parent [Info $oop win] \
					-title $title -message $message
				return
			}
			set message [mc dialog-msg-loadpos]
			set answer [tk_messageBox -parent [Info $oop win] -type yesno \
				-icon question -title $title -message $message]
			if {[string equal $answer yes]} {
				ReadGeometryFile
			}
		}
		E_WINDOW_AUTOSAVE {
			Value window,autosave [Info $oop window,autosave]
		}

		E_CHOICEWINDOW {
			if {[Info $oop choiceWindow]} {
				NSModule::LoadIfNeeded NSChoiceWindow
				NSWindowManager::Display choice
			} else {
				NSWindowManager::Undisplay choice
			}
		}
		E_WINDOW_MAP {
			if {[Info $oop mapWindow]} {
				wm deiconify [Window micromap]
			} else {
				wm withdraw [Window micromap]
			}
			Value micromap,float [Info $oop mapWindow]
		}
		E_WINDOW_MESSAGES {
			if {[Info $oop messagesWindow]} {
				NSModule::LoadIfNeeded NSMessageWindow
				NSWindowManager::Display message2
			} else {
				NSWindowManager::Undisplay message2
			}
		}
		E_WINDOW_MESSAGE {
			if {[Info $oop messageWindow]} {
				wm deiconify [Window message]
				grid remove [Window main].message
				Global message,message [Window message].message
			} else {
				wm withdraw [Window message]
				grid [Window main].message
				Global message,message [Window main].message.message
			}
			Value message,float [Info $oop messageWindow]
		}
		E_WINDOW_MISC {
			if {[Info $oop miscWindow]} {
				wm deiconify [Window misc]
				grid remove [Window main].misc
				Global misc,canvas [Window misc].misc
				if {[Value misc,layout] == "wide"} {
					wm deiconify [Window progress]
				}
			} else {
				Value misc,layout tall
				wm withdraw [Window misc]
				wm withdraw [Window progress]
				grid [Window main].misc
				Global misc,canvas [Window main].misc.misc
			}
			Value misc,float [Info $oop miscWindow]
			if {[Value misc,layout] == "wide"} {
				NSMiscWindow::MiscArrangeWide
			} else {
				NSMiscWindow::MiscArrangeTall
			}
		}
		E_WINDOW_RECALL {
			if {[Info $oop recallWindow]} {
				NSWindowManager::Display recall
			} else {
				NSWindowManager::Undisplay recall
			}
		}

		E_HELP {DoUnderlyingCommand ?}
		E_TIPS {
			NSModule::LoadIfNeeded NSTips 
			WindowBringToFront [Window tip]
		}
		E_ABOUT {AboutApplication}
		default {
			error "unhandled menu entry \"$ident\""
		}
	}

	return
}

# NSMainWindow::Close --
#
#	Called when the user attempts to close the window.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::Close {oop} {

	global Angband

	# Check if game is waiting for a command. If not, it isn't a
	# good time to quit.
	if {[string compare [angband inkey_flags] INKEY_CMD]} {
		bell
		return
	}

	# Ask the user to confirm quit with save
	set answer [tk_messageBox -icon question -type yesno \
		-title [format [mc dialog-title-quit] "ZAngband"] \
		-message [mc dialog-msg-quit]]
	if {[string equal $answer no]} return

	# Save and quit
	DoCommandIfAllowed ^x

	return
}

# NSMainWindow::Configure --
#
#	Called when the Main Window widget changes size.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::Configure {oop width height} {

	variable Progress

	set widgetId [Global main,widgetId]
	set widget [Global main,widget]

	# Resize the main widget
	if {[NSWidget::Resize $widgetId $width $height]} {
		
		# Move the Monster Health Bar
		set x [expr {$width / 2}]
		set y [expr {$height - 8}]
		$widget itemconfigure $Progress(barId) -x $x -y $y
		$widget itemconfigure $Progress(textId) -x $x -y [expr {$y - 6}]

		# Move the target 'T' item
		set x [expr {$width - 6}]
		set y [expr {$height - 6}]
		$widget itemconfigure [Global target,itemId] -x $x -y $y

		# Arrange "status" items
		NSStatus::Configure
	}

	return
}

proc NSMainWindow::DisplayPoints {oop itemId value} {

	set widget [Global main,widget]
	$widget itemconfigure $itemId -text $value

	return
}

# NSMainWindow::TargetSetup --
#
#	One-time initialization. When the target is set, display an
#	image in the lower-left corner. Remove the image when the target
#	is unset. Display the image differently when the target is
#	projectable/not-projectable.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::TargetSetup {oop} {

	variable Target

	set win [Info $oop win]

	set widget [Global main,widget]
	set width [$widget cget -width]
	set height [$widget cget -height]
	set x [expr {$width - 6}]
	set y [expr {$height - 6}]

	if {[Platform unix]} {
		set font {Times 14 bold}
	}
	if {[Platform windows]} {
		set font {Times 11 bold}
	}

	set itemId [$widget create text -x $x -y $y -visible no \
		-anchor se -text T -clipx no -width 20 -height 20 -bevel yes \
		-font $font -justify center \
		-fill [Value targetText] \
		-fill2 [Value targetText2] \
		-background [Value targetBG] \
		-background2 [Value targetBG2] \
		-bevellight [Value targetBL] \
		-bevellight2 [Value targetBL2] \
		-beveldark [Value targetBD] \
		-beveldark2 [Value targetBD2]]

	# Reserve the widget colors for the target image
	set data {
		-fill targetText 1
		-background targetBG 1
		-bevellight targetBL 1
		-beveldark targetBD 1
		-fill target2Text 0
		-background target2BG 0
		-bevellight target2BL 0
		-beveldark target2BD 0
	}
	foreach {option valueName visible} $data {
		set color [palette nearest [Value $valueName]]
		set opacity [Value ${valueName}2]
		$widget coloralloc $color $opacity
		set Target(alloc,$valueName) [list $color $opacity]

		NSValueManager::AddClient $valueName \
			"NSMainWindow::TargetSynch $valueName $option $visible"
		NSValueManager::AddClient ${valueName}2 \
			"NSMainWindow::TargetSynch ${valueName}2 ${option}2 $visible"
	}
	
	Global target,itemId $itemId
	Global target,visible 0

	# This "cursor" is displayed during targetting/looking
	set itemId [$widget create cursor -color yellow -linewidth 2 -visible no]

	if {$::DEBUG} {
		set ::debug_cursor 0
	}

	Global cursor,itemId $itemId
	Global cursor,visible 0

	return
}

# NSMainWindow::TargetSet --
#
#	Handle the <Target-set> quasi-event.
#
# Arguments:
#	widget					the main Widget
#	r_idx					r_info[] index, or zero
#
# Results:
#	What happened.

proc NSMainWindow::TargetSet {widget r_idx} {

	set itemId [Global target,itemId]

	if {$r_idx} {
		set text [angband r_info set $r_idx d_char]
	} else {
		set text ""
	}
	$widget itemconfigure $itemId -visible yes -text $text

	return
}

# NSMainWindow::SetTargetColors --
#
#	Sets the colors of the target indicator.
#
# Arguments:
#	visible						True if target in line-of-sight
#
# Results:
#	What happened.

proc NSMainWindow::SetTargetColors {visible} {

	variable Target

	set widget [Global main,widget]

	if {$visible} {
		set data {
			-fill targetText
			-background targetBG
			-bevellight targetBL
			-beveldark targetBD
		}
	} else {
		set data {
			-fill target2Text
			-background target2BG
			-bevellight target2BL
			-beveldark target2BD
		}
	}

	set command "$widget itemconfigure [Global target,itemId]"
	foreach {option valueName} $data {
		append command " $option [Value $valueName]"
		append command " ${option}2 [Value ${valueName}2]"
	}
	eval $command

	Global target,visible $visible

	return
}

# NSMainWindow::TargetSynch --
#
#	Called by NSValueManager when any of the target indicator colors change.
#
# Arguments:
#	valueName				The name of the value.
#	option					Configuration option for the widget item.
#
# Results:
#	What happened.

proc NSMainWindow::TargetSynch {valueName option visible} {

	variable Target

	set widget [Global main,widget]
	set itemId [Global target,itemId]

	set name [string trimright $valueName 2]
	set color [lindex $Target(alloc,$name) 0]
	set opacity [lindex $Target(alloc,$name) 1]
	$widget colorderef $color $opacity

	set value [Value $valueName]
	if {[string match *2 $valueName]} {
		set opacity $value
	} else {
		set color [palette nearest $value]
	}
	$widget coloralloc $color $opacity
	set Target(alloc,$name) [list $color $opacity]

	if {[Global target,visible] == $visible} {
		$widget itemconfigure $itemId $option $value
	}

	return
}

# NSMainWindow::ProgressSetup --
#
#	Creates a Widget text item (for the monster name) and progress item
#	(for the monster hit points) in the Main Window.
#
# Arguments:
#	widget					The Widget to create the monster bar in.
#
# Results:
#	What happened.

proc NSMainWindow::ProgressSetup {widget} {

	variable Progress

	# XXX Mega-Hack XXX
	# The monster health bar is displayed differently for friendly versus
	# non-friendly monsters. This is done by using a different set of
	# colors for each state. We don't want the progress item to repeatedly
	# allocate and deallocate the colors it uses, so we call the
	# "$widget coloralloc" command to preallocate each color used by
	# the progress item.
	#
	# When the user chooses new progress item colors via the Color
	# Preferences Window, we must deallocate those colors previously
	# pre-allocated, then pre-allocate the new colors.
	
	foreach name {BarDone BarToDo BarBL BarBD} {
	
		set color [palette nearest [Value health$name]]
		set opacity [Value health${name}2]
		$widget coloralloc $color $opacity
		set Progress(alloc,health$name) [list $color $opacity]
	
		set color [palette nearest [Value friend$name]]
		set opacity [Value friend${name}2]
		$widget coloralloc $color $opacity
		set Progress(alloc,friend$name) [list $color $opacity]
	}

	set width [$widget cget -width]
	set height [$widget cget -height]
	set x [expr {$width / 2}]
	set y [expr {$height - 8}]

	set data {
		healthBarDone done
		healthBarToDo todo
		healthBarBL bevellight
		healthBarBD beveldark
		healthBarDone2 done2
		healthBarToDo2 todo2
		healthBarBL2 bevellight2
		healthBarBD2 beveldark2
	}
	foreach {name varname} $data {
		set $varname [Value $name]
	}

	set Progress(barId) \
		[$widget create progressbar -x $x -y $y -width 150 -height 6 \
		-anchor s -done $done -done2 $done2 -todo $todo -todo2 $todo2 \
		-bevellight $bevellight -bevellight2 $bevellight2 \
		-beveldark $beveldark -beveldark2 $beveldark2 \
		-visible no]

	set data {
		healthNameText fill
		healthNameBG background
		healthNameBL bevellight
		healthNameBD beveldark
		healthNameText2 fill2
		healthNameBG2 background2
		healthNameBL2 bevellight2
		healthNameBD2 beveldark2
	}
	foreach {name varname} $data {
		set $varname [Value $name]
	}

	# Get the desired font
	set font [Value font,monster]

	# Calculate the height of a row
	set fontHeight [font metrics $font -linespace]

	# Fudge
	incr fontHeight 1

	# Initial width (will expand automatically)
	set nameWidth 150

	# Create a widget text item
	set Progress(textId) [$widget create text -x $x -y [expr {$y - 6}] \
		-visible no -anchor s -fill $fill -fill2 $fill2 \
		-background $background -background2 $background2 \
		-bevellight $bevellight -bevellight2 $bevellight2 \
		-beveldark $beveldark -beveldark2 $beveldark2 -font $font \
		-clipx yes -clipy yes -width $nameWidth -height $fontHeight \
		-bevel yes -expandx yes -expandy yes -padbottom 1]
	
	set data [list \
		healthBarDone -done \
		healthBarToDo -todo \
		healthBarBL -bevellight \
		healthBarBD -beveldark \
		healthBarDone2 -done2 \
		healthBarToDo2 -todo2 \
		healthBarBL2 -bevellight2 \
		healthBarBD2 -beveldark2 \
	]

	lappend data \
		healthNameText -fill \
		healthNameBG -background \
		healthNameBL -bevellight \
		healthNameBD -beveldark \
		healthNameText2 -fill2 \
		healthNameBG2 -background2 \
		healthNameBL2 -bevellight2 \
		healthNameBD2 -beveldark2 \
		friendBarDone -done \
		friendBarToDo -todo \
		friendBarBL -bevellight \
		friendBarBD -beveldark \
		friendBarDone2 -done2 \
		friendBarToDo2 -todo2 \
		friendBarBL2 -bevellight2 \
		friendBarBD2 -beveldark2

	foreach {name option} $data {
		NSValueManager::AddClient $name \
			"NSMainWindow::ProgressSynch $name $option"
	}

	set Progress(visible) 0
	set Progress(current) 0
	set Progress(r_idx) 0
	set Progress(friend) 0

	# Update ourself when the font,monster value changes
	NSValueManager::AddClient font,monster \
		"NSMainWindow::ValueChanged_font_monster"

	return
}

# NSMainWindow::ValueChanged_font_monster --
#
#	Called when the font,monster value changes.
#	Updates the Monster Health Bar.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::ValueChanged_font_monster {} {

	variable Progress
	
	# Get the desired font
	set font [Value font,monster]

	# Change the font. We rely on the -expandy option to resize the
	# bitmap for us.
	[Global main,widget] itemconfigure $Progress(textId) -font $font

	return
}

# NSMainWindow::ValueChanged_font_statusBar --
#
#	Called when the font,statusBar value changes.
#	Updates the Main Window statusbar.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::ValueChanged_font_statusBar {} {

	set statusBar [Window main].statusBar

	# Get the desired font
	set font [Value font,statusBar]

	# Update the font. Too bad there isn't a -fontvar font variable
	$statusBar.frameLabel.label configure -font $font
	$statusBar.center configure -font $font
	$statusBar.depth configure -font $font

	return
}

# NSMainWindow::ProgressSynch --
#
#	Called by NSValueManager when any of the healthName*, healthBar* or
#	friendBar* values change. Configures the monster health bar colors
#	as appropriate. Note the ugly song-and-dance number done to control
#	which colors are pre-allocated.
#
# Arguments:
#	name					The name of the value.
#	option					Configuration option for the widget item.
#
# Results:
#	What happened.

proc NSMainWindow::ProgressSynch {name option} {

	variable Progress

	set widget [Global main,widget]
	set value [Value $name]

	switch -glob $name {
		healthName* {
			$widget itemconfigure $Progress(textId) $option $value
		}
		healthBar*2 {
			set name2 [string trimright $name 2]
			eval $widget colorderef $Progress(alloc,$name2)
			set color [lindex $Progress(alloc,$name2) 0]
			set Progress(alloc,$name2) [list $color $value]
			$widget coloralloc $color $value
			if {!$Progress(friend)} {
				$widget itemconfigure $Progress(barId) $option $value
			}
		}
		healthBar* {
			eval $widget colorderef $Progress(alloc,$name)
			set opacity [lindex $Progress(alloc,$name) 1]
			set Progress(alloc,$name) [list [palette nearest $value] $opacity]
			$widget coloralloc [palette nearest $value] $opacity
			if {!$Progress(friend)} {
				$widget itemconfigure $Progress(barId) $option $value
			}
		}
		friendBar*2 {
			set name2 [string trimright $name 2]
			eval $widget colorderef $Progress(alloc,$name2)
			set color [lindex $Progress(alloc,$name2) 0]
			set Progress(alloc,$name2) [list $color $value]
			$widget coloralloc $color $value
			if {$Progress(friend)} {
				$widget itemconfigure $Progress(barId) $option $value
			}
		}
		friendBar* {
			eval $widget colorderef $Progress(alloc,$name)
			set opacity [lindex $Progress(alloc,$name) 1]
			set Progress(alloc,$name) [list [palette nearest $value] $opacity]
			$widget coloralloc [palette nearest $value] $opacity
			if {$Progress(friend)} {
				$widget itemconfigure $Progress(barId) $option $value
			}
		}
	}

	return
}

# NSMainWindow::DisplayCursor --
#
#	Shows the cursor in the main widget at the given cave
#	location. If the location is outside the visible area of the
#	widget, then the widget is centered on the location.
#
#	When hiding the cursor the widget is centered on the character
#	location if required.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#	show				1 to show the cursor, 0 to hide it.
#	x, y				Cave location to place cursor at.
#
# Results:
#	What happened.

proc NSMainWindow::DisplayCursor {oop show x y} {

	if {0} {
		set isoview [Global main,isoview]
		if {$show} {
			$isoview configure -hit 1 -hitx $x -hity $y
		} else {
			$isoview configure -hit -1
		}
		$isoview wipe
		Global cursor,visible $show
		return
	}

	set widget [Global main,widget]

	# Show the cursor
	if {$show} {

		$widget itemconfigure [Global cursor,itemId] \
			-visible yes -x $x -y $y

		if {![$widget visible $y $x]} {
			$widget center $y $x
		}

		Global cursor,visible 1

		# Remember the coordinates (could use "$widget itemcget -x")
		Global cursor,y $y
		Global cursor,x $x

	# Hide the cursor
	} else {

		$widget itemconfigure [Global cursor,itemId] \
			-visible no

		if {[string compare [$widget center] [Global main,widget,center]]} {
			eval $widget center [Global main,widget,center]
		}

		Global cursor,visible 0
	}

	return
}

# NSRecall::Fresh_Cursor --
#
#	Called as a <Term-fresh> quasi-event script.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::Fresh_Cursor {oop} {

	ASSERT {$::debug_cursor == 1} \
		"Fresh_Cursor called with debug_cursor=0!"

	DisplayCursor $oop 1 [Global cursor,x] [Global cursor,y]

	return
}

# NSMainWindow::SynchMenuAccel --
#
#	Sets the accelerator option for certain menu entries depending on
#	the current keymap.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::SynchMenuAccel {oop force} {

	global NSMenu
	variable Priv
	
	# Since many keymaps may change when a pref file is read in, delay
	# configuring the menu accelerators until idle time.
	if {!$force} {
	
		if {![string length $Priv(keymap,afterId)]} {
			set Priv(keymap,afterId) \
				[after idle NSMainWindow::SynchMenuAccel $oop 1]
		}

		# The idle task was scheduled by a previous call, but this
		# call isn't from the idle task.
		return
	}

	# Important: clear the after id.
	set Priv(keymap,afterId) ""

	set mbarId [Info $oop mbarId]

	lappend data E_GAME_SAVE ^S
	lappend data E_GAME_EXIT ^X

	lappend data E_MAGIC_ACTIVATE A
	lappend data E_MAGIC_WAND a
	lappend data E_MAGIC_POTION q
	lappend data E_MAGIC_SCROLL r
	lappend data E_MAGIC_STAFF u
	lappend data E_MAGIC_ROD z
	lappend data E_MAGIC_BROWSE b
	lappend data E_MAGIC_STUDY G

	lappend data E_USE_DESTROY k
	lappend data E_USE_DROP d
	lappend data E_USE_PICKUP g
	lappend data E_USE_TAKEOFF t
	lappend data E_USE_WIELD w
	lappend data E_USE_FOOD E
	lappend data E_USE_MISSILE f
	lappend data E_USE_FUEL F
	lappend data E_USE_SPIKE j
	lappend data E_USE_THROW v

	lappend data E_INVEN_EQUIPMENT e
	lappend data E_INVEN_INVENTORY i
	lappend data E_INVEN_INSPECT I
	lappend data E_INVEN_INSCRIBE \{
	lappend data E_INVEN_UNINSCRIBE \}

	lappend data E_ACTION_ALTER +
	lappend data E_ACTION_BASH B
	lappend data E_ACTION_CLOSE c
	lappend data E_ACTION_DISARM D
	lappend data E_ACTION_DOWN >
	lappend data E_ACTION_LOOK l
	lappend data E_ACTION_MAP M
	lappend data E_ACTION_NOTE :
	lappend data E_ACTION_OPEN o
	lappend data E_ACTION_REPEAT n

	lappend data E_ACTION_REST R
	lappend data E_ACTION_RUN .
	lappend data E_ACTION_SEARCH s
	lappend data E_ACTION_SEARCH_MODE S
	lappend data E_ACTION_STAY ,
	lappend data E_ACTION_STAY_TOGGLE g
	lappend data E_ACTION_TARGET *
	lappend data E_ACTION_TUNNEL T
	lappend data E_ACTION_UP <
	lappend data E_ACTION_WALK ";"
	lappend data E_ACTION_WALK_TOGGLE -

	lappend data E_ACTION_PETS p
	lappend data E_ACTION_POWER U

	lappend data E_HELP ?
	lappend data E_OTHER_FEELING ^F
	lappend data E_OTHER_INFO C
	lappend data E_OTHER_KNOWLEDGE ~
	lappend data E_OTHER_MESSAGES ^P
	lappend data E_OTHER_QUEST ^Q
	lappend data E_OTHER_TIME ^T

	lappend data E_PREF_MACROS @
	lappend data E_PREF_OPTIONS =
	
	foreach {ident key} $data {
		set entry [NSMenu::MenuFindEntry $mbarId $ident]
		if {$::DEBUG && ![llength $entry]} {
			error "can't find menu identifier \"$ident\""
		}
		set menuId [lindex $entry 0]
		set index [lindex $entry 1]
		set menu $NSMenu($menuId,menu)

		if 0 {
		
		set string [angband keymap find $key]
		regsub {\^} $string Ctrl+ string
		$menu entryconfigure $index -accelerator $string
		}
	}

	return
}


# NSMainWindow::MouseCmd --
#
#	Use to execute commands when a mouse button is pressed. The direction
#	is determined from the given widget coordinates.
#	Calls "angband keypress CMD DIR".
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#	x					x coordinate in Widget (as returned by event)
#	y					y coordinate in Widget (as returned by event)
#	cmd					Command to invoke.
#
# Results:
#	What happened.

proc NSMainWindow::MouseCommand {oop x y cmd} {

	set widgetId [Global main,widgetId]

	set coords [NSWidget::PointToCave $widgetId $x $y]
	scan $coords "%d %d" caveY caveX
	set dirInfo [CaveToDirection $caveY $caveX]
	set charDir [lindex $dirInfo 0]

	if {$charDir != 5} {
		angband keypress \\$cmd$charDir
	}

	return
}

# NSMainWindow::TrackPress --
#
#	Set up mouse tracking when <ButtonPress-1> occurs. See TrackMotion()
#	and TrackOnce() as well.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#	x					x coordinate in Widget (as returned by event)
#	y					y coordinate in Widget (as returned by event)
#
# Results:
#	What happened.

proc NSMainWindow::TrackPress {oop x y} {

	variable tracking
	variable track1st
	variable trackStepping
	variable trackX
	variable trackY

	set tracking 1
	set track1st 1
	set trackX $x
	set trackY $y

	# Hack -- Allow drag during targetting
	if {[string equal [angband inkey_flags] INKEY_TARGET]} {
		NSWidget::TrackPress [Global main,widgetId] $x $y
		return
	}

	variable trackHPfrac
	scan [angband player hitpoints] "%d %d %f" curhp maxhp hpfrac
	set trackHPfrac $hpfrac

	TrackOnce $oop

	set track1st 0

	set trackStepping 1
	after 200 set NSMainWindow::trackStepping 0

	return
}

# NSMainWindow::TrackMotion --
#
#	Called to remember the cursor position when <Button1-Motion> occurs.
#	See TrackOnce() below as well.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#	x					x coordinate in Widget (as returned by event)
#	y					y coordinate in Widget (as returned by event)
#
# Results:
#	What happened.

proc NSMainWindow::TrackMotion {oop x y} {

	variable trackX
	variable trackY

	# Hack -- Allow drag during targetting
	if {[string equal [angband inkey_flags] INKEY_TARGET]} {
		NSWidget::TrackOnce [Global main,widgetId] $x $y
		return
	}

	set trackX $x
	set trackY $y

	return
}

# NSMainWindow::TrackOnce --
#
#	This command examines the result of "angband inkey_flags" and
#	takes some action depending on the value. During INKEY_MORE and
#	INKEY_DISTURB it calls "angband keypress" with a single space
#	character. During INKEY_DIR it calls "angband keypress" with the
#	corresponding direction character (0-9).
#
#	During INKEY_CMD it calls "angband keypress" with a direction
#	key (to move the character), but only if the grid is not blocked.
#
#	This command is usually called when the <Inkey> binding is invoked,
#	but if the character is unable to move it calls itself again as
#	an "after" command.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::TrackOnce {oop} {

	variable tracking
	variable track1st
	variable trackX
	variable trackY
	variable trackId
	variable trackStepping

	# If the mouse isn't down, then do nothing. This command gets
	# called whenever the <Inkey> event is generated.
	if {!$tracking} return
if 0 {
	# Hack -- Allow drag during targetting
	if {[string equal [angband inkey_flags] INKEY_TARGET]} {
		NSWidget::TrackOnce [Global main,widgetId] $trackX $trackY
		return
	}
}
	# It is important to delay after taking the first step, otherwise
	# the character won't be able to navigate cleanly, and -more-
	# messages may go zipping by.
	if {$trackStepping} {
		set trackId [after 1 NSMainWindow::TrackOnce $oop]
		return
	}

	# (1) Walking into a door with always_repeat
	# (2) Walking through rubble/tree (OAngband)
	if {!$track1st && [angband player command_rep]} return

	# Get the inkey_flags
	set flags [angband inkey_flags]

	# If the game is displaying the "-more-" message, feed the Term
	# with a single space character. This only works if the "quick_messages"
	# option is set.
	if {[string equal $flags INKEY_MORE]} {
		angband keypress " "
		return
	}

	# If a repeated command is in progress, a mouse-click will disturb
	if {[string equal $flags INKEY_DISTURB]} {
		angband keypress " "
		return
	}

	set widgetId [Global main,widgetId]
	set widget [Global main,widget]

	set coords [NSWidget::PointToCave $widgetId $trackX $trackY]
	if {![string length $coords]} {
		set trackId [after 1 NSMainWindow::TrackOnce $oop]
		return
	}
	scan $coords "%d %d" caveY caveX
	set dirInfo [CaveToDirection $caveY $caveX]
	set dirKey [lindex $dirInfo 0]
	set y [lindex $dirInfo 1]
	set x [lindex $dirInfo 2]

	# If the game is waiting for the user to enter a direction, then
	# feed the direction key into the Term.
	if {[string equal $flags INKEY_DIR]} {
		angband keypress $dirKey
		return
	}
if 0 {
	# If the game is waiting for the user to choose a target, feed the
	# y,x location.
	# XXX Remove this if PROJECT_HINT stuff is used
	if {[string equal $flags INKEY_TARGET]} {
		angband keypress @$caveY\n$caveX\n
		return
	}
}
	# If the game is NOT asking for a command, then do nothing
	if {[string compare $flags INKEY_CMD]} {
		return
	}

	# If the grid in the desired direction is blocked, then we will
	# attempt to "slide" in that direction.
	if {[angband cave blocked $y $x]} {
if 1 {
		scan $::PYPX "%d %d" py px
		set xdiff [expr {$caveX - $px}]
		set ydiff [expr {$caveY - $py}]
} else {
		set xdiff [expr {$trackX - [NSWidget::Info $widgetId width] / 2}]
		set ydiff [expr {$trackY - [NSWidget::Info $widgetId height] / 2}]
}
		set xdiff [expr {abs($xdiff)}]
		set ydiff [expr {abs($ydiff)}]
		switch $dirKey {
			1 {
				if {$xdiff > $ydiff} {
					incr y -1
					set dirKey 4
				} else {
					incr x
					set dirKey 2
				}
			}
			3 {
				if {$xdiff > $ydiff} {
					incr y -1
					set dirKey 6
				} else {
					incr x -1
					set dirKey 2
				}
			}
			7 {
				if {$xdiff > $ydiff} {
					incr y 1
					set dirKey 4
				} else {
					incr x
					set dirKey 8
				}
			}
			9 {
				if {$xdiff > $ydiff} {
					incr y 1
					set dirKey 6
				} else {
					incr x -1
					set dirKey 8
				}
			}
		}
		if {[angband cave blocked $y $x]} {
			set trackId [after 1 NSMainWindow::TrackOnce $oop]
			return
		}
	}

	# If the mouse is over the player grid, only move if this is
	# the initial mouse click. Otherwise the user may accidentally
	# "run on the spot".
	if {!$track1st && ($dirKey == 5)} {
		set trackId [after 10 NSMainWindow::TrackOnce $oop]
		return
	}

	# If the spacebar is down, we may get any number of Inkey
	# events per turn. To prevent "mouse command overflow" we
	# never feed the Term with more than one key per turn.
	if {[angband keycount]} return

	# If the character's hit points have dropped more than 15%, then
	# display a warning message and stop tracking. This is to prevent
	# insta-death by running into a nasty monster. This could be an
	# option.
	if 1 {

		# TODO: compare old maxhp to current maxhp, to prevent
		# warnings when exp level goes up.

		# Alway process the initial mouse click
		if {!$track1st} {

			variable trackHPfrac

			set oldFrac $trackHPfrac
			scan [angband player hitpoints] "%d %d %f" curhp maxhp hpfrac
			set trackHPfrac $hpfrac

			# Hit points dropped by over 15%
			if {$hpfrac < ($oldFrac * 0.85)} {
	
				# Message
				NSStatus::SetStatusMessage [mc Danger!] Danger! bad
	
				# The user must release and click again to move.
				TrackRelease $oop
	
				# No rescheduled track.
				return
			}
		}
	}

	# Move the character
	angband keypress $dirKey

	return
}

# NSMainWindow::TrackRelease --
#
#	Cancels mouse tracking when the mouse button is released.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::TrackRelease {oop} {

	variable trackId
	variable tracking
	variable trackStepping
	variable trackX
	variable trackY

	# One time I selected a menu command and received an error after releasing
	# the mouse-button
	if {!$tracking} return

	set tracking 0
	set trackStepping 0

	after cancel $trackId

	# If the Widget wasn't dragged, then tell the game to target
	if {[string equal [angband inkey_flags] INKEY_TARGET]} {
		set widgetId [Global main,widgetId]
		if {![NSWidget::Info $widgetId track,mouseMoved]} {
			set coords [NSWidget::PointToCave $widgetId $trackX $trackY]
			scan $coords "%d %d" caveY caveX
			angband keypress @$caveY\n$caveX\n
			return
		}
	}

	return
}



# NSMainWindow::Leave --
#
#	Handle the mouse leaving the Widget. Called as NSWidget(OOP,leaveCmd).
#
# Arguments:
#	oop					OOP ID NSWidget.
#
# Results:
#	What happened.

proc NSMainWindow::Leave {oop} {

	# Unused: PROJECT_HINT
	if {0 && [string equal [angband inkey_flags] INKEY_TARGET]} {

		# Show target grids at the cursor
		set y [Global cursor,y]
		set x [Global cursor,x]
		angband keypress &$y\n$x\n
		return
	}

	# Clear the statusbar prompt
	StatusText $oop ""

	return
}

# NSMainWindow::CaveToDirection --
#
#	Given cave location y,x, determine the direction key relative
#	to the player location.
#
# Arguments:
#	y					y cave location.
#	x					x cave location.
#
# Results:
#	Return "dir y x", where dir is key to move, y/x is adjacent cave location
#	character would move to.

proc NSMainWindow::CaveToDirection {y x} {

	global PYPX

	scan $PYPX "%d %d" py px

	if {$y < $py} {
		set yyy 789
		incr py -1
	} elseif {$y > $py} {
		set yyy 123
		incr py
	} else {
		set yyy 456
	}

	if {$x < $px} {
		set dirKey [string index $yyy 0]
		incr px -1
	} elseif {$x > $px} {
		set dirKey [string index $yyy 2]
		incr px
	} else {
		set dirKey [string index $yyy 1]
	}

	return "$dirKey $py $px"
}

# NSMainWindow::StatusText --
#
#	Displays text in the status bar.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#
# Results:
#	What happened.

proc NSMainWindow::StatusText {oop text} {

	set label [Global main,statusBar]
	if {[string compare $text [$label cget -text]]} {
		$label configure -text $text
	}

	return
}

# NSMainWindow::DisplayDepth --
#
#	Displays the dungeon level in the Main Window's status bar.
#
# Arguments:
#	label					The label widget to display the depth in.
#	depth					Current depth.
#
# Results:
#	What happened.

proc NSMainWindow::DisplayDepth {label depth} {

	if {[angband player inside_arena]} {
		set depthStr [mc Arena]
	} elseif {[angband player inside_quest]} {
		set depthStr [mc Quest]
	} elseif {$depth == 0} {
		set depthStr [angband cave wild_name]
	} else
		set depthStr [format [mc "Level %d"] $depth]
	}
	$label configure -text $depthStr

	return
}

# NSMainWindow::Bind_Py_level --
#
#	Handle <Py-level> quasi-event.
#
# Arguments:
#	level					The new experience level.
#
# Results:
#	What happened.

proc NSMainWindow::Bind_Py_level {oop level} {

	if {$level != [Info $oop Py_level]} {
		NSStatus::SetStatusMessage [format [mc "Level %d"] $level] Level info
		Info $oop Py_level $level
	}

	return
}

# NSMainWindow::ButtonPress3 --
#
#	Do something when Button 3 is pressed in the main widget.
#
# Arguments:
#	oop					OOP ID of NSMainWindow object.
#	x y					Coords in Widget (as returned by event).
#	X Y					Global coords (as returned by event).
#
# Results:
#	What happened.

proc NSMainWindow::ButtonPress3 {oop x y X Y} {

	set win [Info $oop win]

	set flags [angband inkey_flags]

	# Run
	if {[string equal $flags INKEY_CMD]} {
		MouseCommand $oop $x $y .

	# Set target
	} elseif {[string equal $flags INKEY_DIR]} {
		scan [NSWidget::PointToCave [Global main,widgetId] $x $y] "%d %d" y2 x2
		angband keypress *@$y2\n$x2\n
	}

	# Popup pet commands
	if {[string equal $flags INKEY_CMD_PET]} {
		NSRecall::PopupSelect_CmdPet $win.context $X $Y
		
	# Popup Mindcraft power list
	} elseif {[string equal $flags INKEY_MINDCRAFT]} {
		NSMindcraftMenu::PopupSelect $win.context $X $Y
	
	# Popup racial/mutation power list
	} elseif {[string equal $flags INKEY_POWER]} {
		NSRecall::PopupSelect_Power $win.context $X $Y
	}

	return
}

# NSMainWindow::UpdateHealthWho --
#
#	Called as a qebind <Track-health> script. Hides/shows/updates the
#	Monster Health Bar.
#
# Arguments:
#	oop						OOP ID of NSMainWindow object.
#	m_idx					m_list[] index of the tracked monster.
#	friend					1 if m_idx if a pet.
#
# Results:
#	What happened.

proc NSMainWindow::UpdateHealthWho {oop m_idx friend} {

	variable Progress

	set widget [Global main,widget]
	
	# Hide the bar if visible and not tracking
	if {$m_idx == 0} {
		if {$Progress(visible)} {
			$widget itemconfigure $Progress(barId) -visible no
			$widget itemconfigure $Progress(textId) -visible no
			set Progress(visible) 0
		}
		return
	}

	# Show the bar if hidden
	if {!$Progress(visible)} {
		$widget itemconfigure $Progress(barId) -visible yes
		$widget itemconfigure $Progress(textId) -visible yes
		set Progress(visible) 1
	}

	# Change colors depending on friend status
	if {$friend != $Progress(friend)} {
		SetProgressColors $friend
		set Progress(friend) $friend
	}

	# Set the progress
	array set attrib [angband m_list set $m_idx]
	if {$attrib(ml)} {
		set curhp [expr {($attrib(hp) > 0) ? $attrib(hp) : 0}]
		set current [expr {int((double($curhp) / $attrib(maxhp)) * 100)}]
	} else {
		set current 0
	}
	if {$current != $Progress(current)} {
		$widget itemconfigure $Progress(barId) \
			-current $current -maximum 100
		set Progress(current) $current
	}

	# Set the name
	if {$attrib(r_idx) != $Progress(r_idx)} {
		set name [angband r_info info $attrib(r_idx) name]
		$widget itemconfigure $Progress(textId) -text $name
		set Progress(r_idx) $attrib(r_idx)
	}

	return
}


# NSMainWindow::SetProgressColors --
#
#	Called when the monster health bar goes from friend to non-friend mode.
#	Updates the monster health bar colors as appropriate.
#
# Arguments:
#	name1					See "trace" manual entry.
#	name2					See "trace" manual entry.
#	op						See "trace" manual entry.
#
# Results:
#	What happened.

proc NSMainWindow::SetProgressColors {friend} {

	variable Progress

	set widget [Global main,widget]

	if {$friend} {
		set data {
			friendBarDone done
			friendBarToDo todo
			friendBarBL bevellight
			friendBarBD beveldark
			friendBarDone2 done2
			friendBarToDo2 todo2
			friendBarBL2 bevellight2
			friendBarBD2 beveldark2
		}
	} else {
		set data {
			healthBarDone done
			healthBarToDo todo
			healthBarBL bevellight
			healthBarBD beveldark
			healthBarDone2 done2
			healthBarToDo2 todo2
			healthBarBL2 bevellight2
			healthBarBD2 beveldark2
		}
	}

	foreach {name varname} $data {
		set $varname [Value $name]
	}

	$widget itemconfigure $Progress(barId) \
		-done $done -done2 $done2 -todo $todo -todo2 $todo2 \
		-bevellight $bevellight -bevellight2 $bevellight2 \
		-beveldark $beveldark -beveldark2 $beveldark2

	return
}

# NSMainWindow::SelectWindow --
#
#	Make a window the frontmost active window.
#
# Arguments:
#	window				Index into Windows[] (inventory, book, etc)
#
# Results:
#	What happened.

proc NSMainWindow::SelectWindow {window} {

	if {[info exists NSWindowManager::Priv($window,win)]} {
		NSWindowManager::Display $window
		return
	}

	WindowBringToFront [Window $window]

	return
}

# NSMainWindow::WithdrawWindow --
#
#	Withdraw a window.
#
# Arguments:
#	window					Index into Windows[] (inventory, book, etc)
#
# Results:
#	What happened.

proc NSMainWindow::WithdrawWindow {window} {

	wm withdraw [Window $window]

	return
}

# NSMainWindow::Display --
#
#	Remove current window (if any), and select given window.
#
# Arguments:
#	window					Index into Windows[] (inventory, book, etc)
#
# Results:
#	What happened.

proc NSMainWindow::Display {window} {

	global Display

	if {[string compare $Display(window) none] &&
		[string compare $Display(window) $window]} {
		WithdrawWindow $Display(window)
	}

	SelectWindow $window

	set Display(window) $window

	return
}


# NSMainWindow::PositionChanged --
#
#	Called as a qebind <Position> script. Update the Main Window
#	when the character's position changes. Handles the "disturb_panel" option.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::PositionChanged {widget y x} {

	global PYPX

	# Keep character centered in the display
	$widget center $y $x
	Global main,widget,center "$y $x"

	# This global is read in various places
	set PYPX "$y $x"
	
	return
}

# FlashCanvasText --
#
#	Configure the fill color of a canvas item, then do it again later.
#
# Arguments:
#	canvas					Canvas widget the item is in.
#	tagOrId					The canvas item ID to manipulate.
#	color					The fill color.
#	num						Number of times to flash it.
#
# Results:
#	What happened.

global FlashCanvas

proc FlashCanvasTextAux {canvas tagOrId} {

	global FlashCanvas

	set num $FlashCanvas($canvas,$tagOrId,num)
	if {$num & 1} {
		set fill $FlashCanvas($canvas,$tagOrId,colorOff)
	} else {
		set fill $FlashCanvas($canvas,$tagOrId,colorOn)
	}
	$canvas itemconfigure $tagOrId -fill $fill

	incr num -1
	set FlashCanvas($canvas,$tagOrId,num) $num

	if {$num} {
		set id [after 250 "FlashCanvasTextAux $canvas $tagOrId"]
		set FlashCanvas($canvas,$tagOrId,afterId) $id
	} else {
		unset FlashCanvas($canvas,$tagOrId,afterId)
	}

	return
}

proc FlashCanvasText {canvas tagOrId colorOn colorOff num} {

	global FlashCanvas

	# Never set more than one "after" command for an item
	if {[info exists FlashCanvas($canvas,$tagOrId,afterId)]} {
		set id $FlashCanvas($canvas,$tagOrId,afterId)
		after cancel $id
	}

	set FlashCanvas($canvas,$tagOrId,colorOn) $colorOn
	set FlashCanvas($canvas,$tagOrId,colorOff) $colorOff
	set FlashCanvas($canvas,$tagOrId,num) $num

	FlashCanvasTextAux $canvas $tagOrId

	return
}

# FlashCanvasTextFill --
#
#	Returns the fill color for an canvas item. This routine should be
#	called if a canvas item may be "flashing".
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc FlashCanvasTextFill {canvas tagOrId} {

	global FlashCanvas

	if {[info exists FlashCanvas($canvas,$tagOrId,afterId)]} {
		return $FlashCanvas($canvas,$tagOrId,colorOff)
	} else {
		return [$canvas itemcget $tagOrId -fill]
	}
}

# DoCommandIfAllowed --
#
#	Feeds a string of bytes to the Term, but only if INKEY_CMD is set.
#
# Arguments:
#	string					String argument to "angband keypress"
#
# Results:
#	What happened.

proc DoCommandIfAllowed {string} {

	# Check if game is waiting for a command
	if {[string compare [angband inkey_flags] INKEY_CMD]} return

	# Feed the Term
	angband keypress $string

	return
}

# DoUnderlyingCommand --
#
#	Feeds the string to "angband keypress", but prepends a slash
#	to bypass keymaps. This only works if request_command() is being
#	called to handle the \ escape character. INKEY_CMD is actually set
# 	when examining the inventory or equipment, and when browsing a book,
#	in which case this cannot be used.
#
# Arguments:
#	string					String argument to "angband keypress"
#
# Results:
#	What happened.

proc DoUnderlyingCommand {string} {

	# Check if game is waiting for a command
	if {[string compare [angband inkey_flags] INKEY_CMD]} return

	# Feed the Term
	angband keypress \\$string

	return
}

# DoKeymapCmd --
#
#	Maps the given command char to the underlying command and calls
#	"angband keypress" with it. Some command chars can be represented
#	by the X11 keysym.
#
# Arguments:
#	prefix				Misc characters to prepend to command char
#	command				The underlying command char
#	suffix				Misc characters to append to command char
#
# Results:
#	What happened.

proc DoKeymapCmd {prefix command suffix} {

	switch -- $command {
		backslash {set command \\}
		braceleft {set command \{}
		braceright {set command \}}
		bracketleft {set command \[}
		bracketright {set command \]}
		quotedbl {set command \"}
	}
	
	if 0 {
	
	set command [angband keymap find $command]
	angband keypress $prefix$command$suffix

	}

	return
}

# Note: Setting a delay of 0 results in running after the mouse is
# released; setting a delay of 1 or more prevents this
proc ConfigureMouse {} {

	set win .mouse
	toplevel $win
	wm title $win "Mouse Settings"

	set scale $win.speed
	scale $scale \
		-orient horizontal -label "Tracking Delay" \
		-width 15 -sliderlength 20 -length 200 -from 0 -to 200 \
		-command "set ::trackDelay"

	$scale set $::trackDelay

	pack $scale


	set clicks [clock clicks]
	set text [time {after 1} 100]
	set diff [expr {[clock clicks] - $clicks}]

	Debug $text
	Debug "1 ms = [expr {$diff / 100}] clicks"

	return
}

proc TestRedrawSpeed {} {
	set widget [Global main,widget]
	set clicks [clock clicks]
	set text [time {$widget wipe ; update idletasks} 100]
	set diff [expr {[clock clicks] - $clicks}]
	Debug "TestRedrawSpeed: 100 redraws in $diff clicks"

	return
}


# NSMainWindow::ContextMenu_StatusBar --
#
#	Pop up a context menu in the StatusBar to configure it's
# 	appearance.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSMainWindow::ContextMenu_StatusBar {menu x y} {

	$menu delete 0 end

	$menu add command -label [mc "Set Font"] \
		-command "NSModule::LoadIfNeeded NSFont ; NSWindowManager::Display font statusBar"
	$menu add command -label "Set Color" \
		-command {
			set color [tk_chooseColor -parent [Window main] \
				-initialcolor [Value main,statusbar,color]]
			if {$color != ""} {
				Value main,statusbar,color $color
				[Global main,statusBar] configure -foreground $color
			}
		}
	$menu add command -label "Set Autobar Font" \
		-command "NSModule::LoadIfNeeded NSFont ; NSWindowManager::Display font autobar"
	$menu add separator
	$menu add command -label [mc Cancel]

	tk_popup $menu $x $y

	return
}

