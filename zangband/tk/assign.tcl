# File: assign.tcl

# Purpose: the Assign Window and related commands

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSAssign {

	variable MenuString
	variable Priv

# namespace eval NSAssign
}

# NSAssign::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::InitModule {} {

	variable Priv

	MsgCatInit know assign

	InitImageIfNeeded Image_Sound sound.gif

	NSModule::LoadIfNeeded NSIconBrowser

	set Priv(hook) {}
	lappend Priv(hook) hook_effect [mc Effects]
	lappend Priv(hook) hook_feature [mc Features]
	lappend Priv(hook) hook_monster [mc Monsters]
	lappend Priv(hook) hook_object [mc Objects]

	set Priv(find,string) ""
	set Priv(find,fromStart) 1

	set Priv(didAssign) 0
	qebind ASSIGN <Assign> {
		set NSAssign::Priv(didAssign) 1
	}

	# Create the Assign Window
	NSObject::New NSAssign

	return
}

# NSAssign::CloseModule --
#
#	One-time-only-ever cleanup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::CloseModule {} {

	catch {
		destroy [Window assign]
	}

	return
}

# NSAssign::NSAssign --
#
#	Object constructor called by NSObject::New().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::NSAssign {oop} {

	variable Priv
	
	InitWindow $oop

	Info $oop hook hook_monster
	Info $oop group,current -1
	Info $oop member,current -1
	Info $oop display icon
	Info $oop display,ignoreSel 0
	Info $oop radio,display icon

	# Info about selected group & member for each hook
	foreach {hook label} $Priv(hook) {
		Info $oop group,$hook 0
		Info $oop member,$hook -1
	}

	set win [Info $oop win]

	NSWindowManager::RegisterWindow assign $win \
		"GetDefaultGeometry $win reqwidth main" "NSAssign::SetupCmd $oop" \
		"NSAssign::DisplayCmd $oop"		

	# Update ourself when the listBG value changes
	Info $oop listBG,clientId \
		[NSValueManager::AddClient listBG \
		"NSAssign::ValueChanged_listBG $oop"]

	# Update ourself when the font,knowledge value changes
	Info $oop font,clientId \
		[NSValueManager::AddClient font,knowledge \
		"NSAssign::ValueChanged_font_knowledge $oop"]

	# Destroy the object along with the toplevel (later)
	NSUtils::DestroyObjectWithWidget NSAssign $oop $win
	
	bind $win <KeyPress-Escape> "NSAssign::Close $oop"
	bind $win <Control-KeyPress-w> "NSAssign::Close $oop"
	bind $win <KeyPress-f> "NSAssign::Find $oop 0"
	bind $win <KeyPress-g> "NSAssign::Find $oop 1"

	#
	# Global access
	#

	Window assign $win
	Global assign,oop $oop

	return
}

# NSAssign::~NSAssign --
#
#	Object destructor called by NSObject::Delete().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::~NSAssign {oop} {

	NSValueManager::RemoveClient listBG [Info $oop listBG,clientId]
	NSValueManager::RemoveClient font,knowledge [Info $oop font,clientId]

	return
}

# NSAssign::Info --
#
#	Query and modify info.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::Info {oop info args} {

	global NSAssign

	# Verify the object
	NSObject::CheckObject NSAssign $oop

	# Set info
	if {[llength $args]} {
		switch -- $info {
			default {
				set NSAssign($oop,$info) [lindex $args 0]
			}
		}

	# Get info
	} else {
		switch -- $info {
			groupCanvas {
				set canvistId [Info $oop group,canvistId]
				return [NSCanvist::Info $canvistId canvas]
			}
			memberCanvas {
				set canvistId [Info $oop member,canvistId]
				return [NSCanvist::Info $canvistId canvas]
			}
			spriteCanvas {
				set canvistId [Info $oop sprite,canvistId]
				return [NSCanvist2::Info $canvistId canvas]
			}
			default {
				return $NSAssign($oop,$info)
			}
		}
	}

	return
}

# NSAssign::InitWindow --
#
#	Create the window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::InitWindow {oop} {

	variable Priv

	set win .assign$oop
	toplevel $win
	wm title $win [mc Assign]

	wm transient $win [Window main]

	# Start out withdrawn (hidden)
	wm withdraw $win

	# Do stuff when window closes
	wm protocol $win WM_DELETE_WINDOW "NSAssign::Close $oop"

	Info $oop win $win

	InitMenus $oop

	#
	# Tabs!
	#

	set tabsId [NSObject::New NSTabs $win]
	foreach {hook label} $Priv(hook) {
		NSTabs::Add $tabsId $label
	}
	NSTabs::Info $tabsId invokeCmd "NSAssign::InvokeTab $oop"
	NSTabs::Info $tabsId active 1
	Info $oop tabsId $tabsId

	#
	# Group List
	#

	set font [Value font,knowledge]
	set lineSpace [font metrics $font -linespace]
	set rowHgt $lineSpace
	if {[icon size] > $rowHgt} {
		set rowHgt [icon size]
	}
	incr rowHgt 8
	set width [CalcGroupListWidth $oop]

	frame $win.frameGroup \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist $win.frameGroup $rowHgt $width 100 \
		"NSAssign::NewItemCmd $oop" "NSAssign::HighlightItemCmd $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -height [expr {40 * 5}]
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$win.frameGroup.scroll set"
	scrollbar $win.frameGroup.scroll \
		-command "$canvas yview" -orient vertical

#	bind $win.frameGroup.scroll <Map> "eval %W set \[$canvas yview]"
	NSUtils::SynchScrollBar $canvas $win.frameGroup.scroll

	# Do something when a group item is selected
	NSCanvist::Info $canvistId selectionCmd \
		"NSAssign::SelectionChanged_Group $oop"

	pack $win.frameGroup.scroll -side right -fill y
	pack $canvas -side left -expand yes -fill both -anchor nw

	Info $oop group,canvistId $canvistId

	#
	# Member List
	#

	set width 300

	frame $win.frameMember \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist $win.frameMember $rowHgt $width 100 \
		"NSAssign::NewItemCmd $oop" "NSAssign::HighlightItemCmd $oop"]
	set canvas [NSCanvist::Info $canvistId canvas]
	$canvas configure -height [expr {40 * 5}]
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$win.frameMember.scroll set"
	scrollbar $win.frameMember.scroll \
		-command "$canvas yview" -orient vertical

#	bind $win.frameMember.scroll <Map> "eval %W set \[$canvas yview]"
	NSUtils::SynchScrollBar $canvas $win.frameMember.scroll

	# Do something when a member item is selected.
	NSCanvist::Info $canvistId selectionCmd \
		"NSAssign::SelectionChanged_Member $oop"

	# Do something when a selected member is clicked.
	NSCanvist::Info $canvistId clickCmd \
		"NSAssign::Click_Member $oop"

	bind $canvas <Configure> \
		"NSAssign::Configure $oop $canvas"

	Info $oop member,canvistId $canvistId

	pack $win.frameMember.scroll -side right -fill y
	pack $canvas -side left -expand yes -fill both -anchor nw

	MakeDivider $win.divider1 x

	#
	# An NSIconBrowser lets the user examine all icon types
	#

	set browserId [NSObject::New NSIconBrowser $win]
	set canvistId [NSIconBrowser::Info $browserId member,canvistId]
	NSCanvist2::Info $canvistId selectionCmd \
		"NSAssign::SelectionChanged_Icon $oop"
	set canvas [NSIconBrowser::Info $browserId groupCanvas]
	$canvas configure -height [expr {40 * 3}]
	set canvas [NSIconBrowser::Info $browserId memberCanvas]
	$canvas configure -height [expr {40 * 3}]

	# Display progress while listing an icon type
	NSIconBrowser::Info $browserId clientCmd \
		"NSAssign::BrowserCmd $oop"

	NSIconBrowser::Info $browserId group,motionCmd \
		"NSAssign::BrowserMotionCmd $oop group"
	NSIconBrowser::Info $browserId group,leaveCmd \
		"NSAssign::BrowserMotionCmd $oop group"
	
	NSIconBrowser::Info $browserId member,motionCmd \
		"NSAssign::BrowserMotionCmd $oop member"
	NSIconBrowser::Info $browserId member,leaveCmd \
		"NSAssign::BrowserMotionCmd $oop member"

	# Remember the icon browser
	Info $oop browserId $browserId

	#
	# The list of sprites
	#

	set iconSize [expr {[icon size] + 8}]
	set frameSprite $win.frameSprite
	frame $frameSprite \
		-borderwidth 1 -relief sunken
	set canvistId [NSObject::New NSCanvist2 $frameSprite $iconSize \
		$iconSize [expr {40 * 10}] [expr {40 * 3}]]
	NSCanvist2::Info $canvistId newItemCmd \
		"NewItemCmd_Assign"
	NSCanvist2::Info $canvistId highlightCmd \
		"NSIconBrowser::HighlightItemCmd $oop"
	NSCanvist2::Info $canvistId columns 10
	set canvas [NSCanvist2::Info $canvistId canvas]
	scrollbar $frameSprite.yscroll \
		-command "$canvas yview" -orient vertical
	$canvas configure -background [Value listBG]
	$canvas configure -yscrollcommand "$frameSprite.yscroll set"
	NSCanvist2::Info $canvistId selectionCmd \
		"NSAssign::SelectionChanged_Sprite $oop"

#	bind $frameSprite.yscroll <Map> "eval %W set \[$canvas yview]"
	NSUtils::SynchScrollBar $canvas $frameSprite.yscroll

	pack $canvas \
		-side left -expand yes -fill both -anchor nw
	pack $frameSprite.yscroll \
		-side left -fill y

	Info $oop sprite,canvistId $canvistId

	#
	# Statusbar
	#

	MakeStatusBar $win.statusBar 20

	# Progress bar used to display progress of listing icons
	set label [$win.statusBar itemcget t1 -label]
	set progId [NSObject::New NSProgress2 $label 225 10]
	[NSProgress2::Info $progId frame] configure -borderwidth 0
	Info $oop progId $progId

	# Clicking in label2 displays the icon assigned to the selected
	# member.
	bind [$win.statusBar itemcget t2 -label] <ButtonPress-1> \
		"NSAssign::DisplayIcon $oop"

	#
	# Geometry
	#

	grid rowconfig $win 0 -weight 0 -minsize 0
	grid rowconfig $win 1 -weight 0 -minsize 0
	grid rowconfig $win 2 -weight 1 -minsize 0
	grid rowconfig $win 3 -weight 0 -minsize 0
	grid rowconfig $win 4 -weight 1 -minsize 0
	grid rowconfig $win 5 -weight 0 -minsize 0
	grid columnconfig $win 0 -weight 0 -minsize 0
	grid columnconfig $win 1 -weight 1 -minsize 0
 
	if {[Platform windows]} {
		grid [MakeDivider $win.divider2 x] \
			-row 0 -column 0 -rowspan 1 -columnspan 2 -sticky ew -pady 2
	}
	grid [NSTabs::Info $tabsId canvas] \
		-row 1 -column 0 -rowspan 1 -columnspan 2 -sticky ew
	grid $win.frameGroup -in $win \
		-row 2 -column 0 -rowspan 1 -columnspan 1 -sticky ns
	grid $win.frameMember -in $win \
		-row 2 -column 1 -rowspan 1 -columnspan 1 -sticky news
	grid $win.divider1 -in $win \
		-row 3 -column 0 -rowspan 1 -columnspan 2 -sticky ew \
		-pady 2
	grid [NSIconBrowser::Info $browserId frame] \
		-row 4 -column 0 -rowspan 1 -columnspan 2 -sticky news
	grid $win.statusBar -in $win \
		-row 5 -column 0 -rowspan 1 -columnspan 2 -sticky ew

	return
}

# NSAssign::InitMenus --
#
#	Create the menus.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::InitMenus {oop} {

	variable MenuString
	variable Priv

	set win [Info $oop win]
	set mod "Ctrl"

	#
	# Menu bar
	#

	set mbar [NSObject::New NSMenu $win -tearoff 0 \
		-postcommand "NSAssign::MenuSetup $oop" -identifier MENUBAR]
	Info $oop mbarId $mbar

	# Context-sensitive help
	NSMenu::Info $mbar menuSelectCmd "NSAssign::MenuSelect $oop"

	# Call our command when an entry is invoked
	NSMenu::Info $mbar invokeCmd "NSAssign::MenuInvoke $oop"

	#
	# Assign To
	#

	NSObject::New NSMenu $mbar -tearoff 0 -identifier MENU_ASSIGN_TO
	NSMenu::MenuInsertEntry $mbar -end MENUBAR -type cascade \
		-menu MENU_ASSIGN_TO -label [mc "Assign To"] -underline 7 \
		-identifier M_ASSIGN_TO

	set entries {}
	set i 1
	foreach {hook label} $Priv(hook) {
		lappend entries [list -type radiobutton -label $label \
			-variable NSAssign($oop,radio,hook) -value $hook \
			-accelerator $i -identifier E_HOOK_$i]
		bind $win <KeyPress-$i> "NSAssign::SetHook $oop $hook"
		incr i
	}
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Find...] \
		-accelerator f -underline 0 -identifier E_FIND]
	lappend entries [list -type command -label [mc "Find Again"] \
		-accelerator g -underline 6 -identifier E_FIND_AGAIN]
	lappend entries [list -type separator]
	lappend entries [list -type command -label [mc Close] \
		-underline 0 -accelerator $mod+W -identifier E_CLOSE]

	NSMenu::MenuInsertEntries $mbar -end MENU_ASSIGN_TO $entries

	#
	# Assign What Menu
	#

	NSObject::New NSMenu $mbar -tearoff 0 -identifier MENU_ASSIGN_WHAT
	NSMenu::MenuInsertEntry $mbar -end MENUBAR -type cascade \
		-menu MENU_ASSIGN_WHAT -label [mc "Assign What"] -underline 7 \
		-identifier M_ASSIGN_WHAT

	set entries {}
	lappend entries [list -type radiobutton -label [mc Alternate] \
		-variable NSAssign($oop,radio,display) -value alternate \
		-identifier E_ASSIGN_ALTERNATE]
	lappend entries [list -type radiobutton -label [mc Icon] \
		-variable NSAssign($oop,radio,display) -value icon \
		-identifier E_ASSIGN_ICON]
	lappend entries [list -type radiobutton -label [mc Sprite] \
		-variable NSAssign($oop,radio,display) -value sprite \
		-identifier E_ASSIGN_SPRITE]

	NSMenu::MenuInsertEntries $mbar -end MENU_ASSIGN_WHAT $entries

	#
	# Hook Menu
	#

	set m [NSMenu::Info $mbar menu].hook
	menu $m -tearoff 0
	bind $m <<MenuSelect>> \
		"NSAssign::MenuSelect_Hook $oop hook %W"
	Info $oop hookMenu,menu $m
	Info $oop hookMenu,inserted 0

	set MenuString(M_ASSIGN_TO) \
		"Contains commands for displaying and searching groups."
	set MenuString(E_FIND) \
		"Searches for a member by name."
	set MenuString(E_FIND_AGAIN) \
		"Repeats the previous search."
	set MenuString(E_CLOSE) \
		"Closes the window."

	set MenuString(M_ASSIGN_WHAT) \
		"Contains commands for displaying things to assign."
	set MenuString(E_ASSIGN_ALTERNATE) \
		"Displays a list of alterates to assign."
	set MenuString(E_ASSIGN_ICON) \
		"Displays a list of icon types to assign."
	set MenuString(E_ASSIGN_SPRITE) \
		"Displays a list of sprites to assign."

	return
}

# NSAssign::MenuSetup --
#
#	Prepare to post the menus.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::MenuSetup {oop mbarId} {

	variable Priv

	lappend identList E_FIND E_FIND_AGAIN E_CLOSE
		
	set i 0
	foreach {hook label} $Priv(hook) {
		lappend identList E_HOOK_[incr i]
	}

	if {[Info $oop hookMenu,inserted]} {
		set menu [Info $oop hookMenu,menu]
		set last [$menu index end]
		for {set i 0} {$i <= $last} {incr i} {
			if {[string equal [$menu type $i] separator]} continue
			menuentrystate $menu $i disabled
		}
		CallHook $oop menu_setup
	}

	NSMenu::MenuEnable $mbarId $identList

	[Info $oop win].statusBar cover show

	return
}

# NSAssign::MenuSelect --
#
#	Displays a help string associated with a menu entry.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::MenuSelect {oop menuId index ident} {

	variable MenuString

	switch -glob -- $ident {
		{} {
			# Hack -- If there is no identifier but the index isn't "none",
			# it means the hook menu is highlighted.
			if {[string compare $index none]} {
				set desc "Contains commands specific to this group."
			} else {
				set desc {}
			}
		}

		E_HOOK_* {
			set desc "Displays this group."
		}
		
		default {
			if {[info exists MenuString($ident)]} {
				set desc $MenuString($ident)
			} else {
				set menu [NSMenu::Info $menuId menu]
				set desc [$menu entrycget $index -label]
			}
		}
	}

	[Info $oop win].statusBar cover set $desc
	if {![string length $desc]} {
		if {$menuId == [Info $oop mbarId]} {
			[Info $oop win].statusBar cover hide
		}
	}

	return
}

# NSAssign::MenuInvoke --
#
#	Called when a menu entry is invoked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::MenuInvoke {oop menuId ident} {

	variable Priv

	switch -glob -- $ident {
		E_HOOK_* {
			scan $ident "E_HOOK_%d" hookNum
			SetHook $oop [lindex $Priv(hook) [expr {($hookNum - 1) * 2}]]
		}
		E_FIND {Find $oop 0}
		E_FIND_AGAIN {Find $oop 1}
		E_CLOSE {Close $oop}

		E_ASSIGN_* {
			scan $ident "E_ASSIGN_%s" display
			set display [string tolower $display]
			SetDisplay $oop $display
		}
	}

	return
}

# NSAssign::MenuSelect_Hook --
#
#	Displays a help string associated with a menu entry in the
#	hook menu or a submenu of the hook menu.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::MenuSelect_Hook {oop which menu} {

	# Get the index of the active menu entry
	set index [$menu index active]

	# No entry is selected
	if {[string equal $index none]} {
		set string ""

	# An entry is selected
	} else {
		set ident [lindex [Info $oop hookMenu,ident,$which] $index]
		set string [CallHook $oop menu_string $which $ident]
	}

	StatusBar $oop $string 0

	return
}

# NSAssign::DisplayCmd --
#
#	Called by NSWindowManager::Display().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::DisplayCmd {oop message first args} {

	variable Priv
	
	switch -- $message {
		preDisplay {
			
			if {[llength $args]} {
				set hook [lindex $args 0]
				Info $oop hook hook_$hook
				if {[llength $args] > 1} {
					set index [lindex $args 1]
					DisplayMember $oop hook_$hook $index
				}
			}
			set Priv(didAssign) 0

			SetDisplay $oop [Info $oop display]
			SetHook $oop [Info $oop hook]
		}
		postDisplay {
			if {$first} {
				set browserId [Info $oop browserId]
				set canvistId [NSIconBrowser::Info $browserId group,canvistId]
				NSIconBrowser::SetList_Group $browserId
				NSCanvist2::UpdateSelection $canvistId 0 ""
			}
		}
		postWithdraw {
			StateRemember $oop
			NSCanvist::DeleteAll [Info $oop group,canvistId]
			NSCanvist::DeleteAll [Info $oop member,canvistId]
			NSCanvist2::DeleteAll [Info $oop sprite,canvistId]
		}
	}

	return
}

# NSAssign::SetupCmd --
#
#	Called by NSWindowManager::Setup().
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SetupCmd {oop} {

	set win [Info $oop win]

	grid $win.frameSprite \
		-row 4 -column 0 -rowspan 1 -columnspan 2 -sticky news
	update idletasks
	grid forget $win.frameSprite

	return
}

# NSAssign::Close --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::Close {oop} {

	variable Priv

	if {$::DEBUG} {
		if {$Priv(didAssign)} {
			set answer [tk_messageBox -icon question -type yesno \
				-parent [Info $oop win] -message "WriteAssignFile() Now?"]
			if {[string equal $answer yes]} {
				Config::Assign::Write
			}
			set Priv(didAssign) 0
		}
	}

	NSWindowManager::Undisplay assign

	return
}

# NSAssign::StateRemember --
#
#	Remember the group/member selections.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::StateRemember {oop} {

	if {!$NSWindowManager::Priv(assign,setup)} return

	set hook [Info $oop hook]

	# Because we are clearing the lists here, and don't want to
	# upset the user, I save the selected group/member so it can
	# be restored in StateRestore() below.
	Info $oop group,$hook [Info $oop group,current]
	Info $oop member,$hook [Info $oop member,current]

	return
}

# NSAssign::StateRestore --
#
#	Restore the group/member selections.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::StateRestore {oop} {

	set hook [Info $oop hook]
	
	# Restore the selected group
	set current [Info $oop group,$hook]
	set canvistId [Info $oop group,canvistId]
	if {$current != -1} {
		NSCanvist::UpdateSelection $canvistId $current {}
		NSCanvist::See $canvistId $current
	}

	# Restore the selected member
	set current [Info $oop member,$hook]
	set canvistId [Info $oop member,canvistId]
	if {$current != -1} {
		NSCanvist::UpdateSelection $canvistId $current {}
		NSCanvist::See $canvistId $current
	}

	return
}

# NSAssign::CallHook --
#
#	Call the hook.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::CallHook {oop message args} {

	return [uplevel #0 NSAssign::[Info $oop hook] $oop $message $args]
}

# NSAssign::SetHook --
#
#	Set the hook. The hook controls what is displayed and what is
#	done when icons/sprites are selected.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SetHook {oop hook} {

	variable Priv

	if {$hook != [Info $oop hook]} {
		set again 0
	} else {
		set again 1
	}

	if {!$again} {
		StateRemember $oop
	}

	# Remember the hook
	Info $oop hook $hook

	# Clear the hook-specific menu
	set hookMenu [Info $oop hookMenu,menu]
	$hookMenu delete 0 end
	eval destroy [winfo children $hookMenu]

	# Set the group list
	SetList_Group $oop

	# If the hook-menu is empty, remove it, otherwise insert it
	set mbarId [Info $oop mbarId]
	set menu [NSMenu::Info $mbarId menu]
	if {[string equal [$hookMenu index end] none]} {
		if {[Info $oop hookMenu,inserted]} {
			$menu delete end
			Info $oop hookMenu,inserted 0
		}
	} else {
		set index [lsearch -exact $Priv(hook) $hook]
		if {![Info $oop hookMenu,inserted]} {
			$menu add cascade -menu $hookMenu
			Info $oop hookMenu,inserted 1
		}
		$menu entryconfigure end \
			-label [lindex $Priv(hook) [incr index]]
	}

	# Radiobutton menu entries
	Info $oop radio,hook $hook

	set tabsId [Info $oop tabsId]
	set current [NSTabs::Info $tabsId current]
	set tabId [NSTabs::GetNthId $tabsId [expr {[lsearch -exact $Priv(hook) $hook] / 2}]]
	if {$tabId != $current} {
		NSTabs::Smaller $tabsId $current
		NSTabs::Bigger $tabsId $tabId
		NSTabs::Info $tabsId current $tabId
	}

	StateRestore $oop
	
	return
}

# NSAssign::AssignStuff --
#
#	Call the hook to assign stuff, and display the number of members
#	the thing was assigned to.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::AssignStuff {oop what args} {

	set count [eval CallHook $oop assign $what $args]
	if {$count} {
		StatusBar $oop "Assigned $what to $count things." 1
	}

	return
}


# NSAssign::SelectionChanged_Group --
#
#	When a "group" list item is selected, display members in the
#	"member" list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SelectionChanged_Group {oop canvistId select deselect} {

	# If nothing was selected, clear the member list
	if {![llength $select]} {
		set canvistId [Info $oop member,canvistId]
		NSCanvist::DeleteAll $canvistId
		Info $oop group,current -1
		return
	}

	# Get the (first) row
	set row [lindex $select 0]
	Info $oop group,current $row

	# Display members in that group
	SetList_Member $oop $row

	return
}

# NSAssign::SelectionChanged_Icon --
#
#	Do something when an icon is selected in the NSIconBrowser.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SelectionChanged_Icon {oop canvistId select deselect} {

	# Don't assign stuff during feedback
	if {[Info $oop display,ignoreSel]} return

	# Do nothing if no new row was selected
	if {![llength $select]} return

	# Do nothing if no member is selected
	set selection [NSCanvist::Selection [Info $oop member,canvistId]]
	if {![llength $selection]} return

	# Get the (first) index
	set index [lindex $select 0]

	# Get the type of icon
	set browserId [Info $oop browserId]
	set type [NSIconBrowser::Info $browserId iconType]

	# Call hook to assign icons to selected members
	AssignStuff $oop icon $type $index

	# Update the assigned icon in the status bar
	set row [Info $oop member,current]
	set member [lindex [CallHook $oop member_list [Info $oop group,current]] $row]
	[Info $oop win].statusBar itemconfigure t2 -text "#$member: $type $index"

	return
}


# NSAssign::SelectionChanged_Sprite --
#
#	Do something when an icon is selected in the sprite list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SelectionChanged_Sprite {oop canvistId select deselect} {

	# Don't assign stuff during feedback
	if {[Info $oop display,ignoreSel]} return

	# Do nothing if no new row was selected
	if {![llength $select]} return

	# Do nothing if no member is selected
	set selection [NSCanvist::Selection [Info $oop member,canvistId]]
	if {![llength $selection]} return

	# Get the (first) index
	set index [lindex $select 0]

	# Call hook to assign sprite to selected members
	set other [Info $oop display]
	AssignStuff $oop $other $index

	return
}

# NSAssign::SetList_Group --
#
#	.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SetList_Group {oop} {

	set win [Info $oop win]
	set canvistId [Info $oop group,canvistId]

	# Feedback
	StatusBar $oop "Displaying..." 0
	update idletasks

	# Clear the list
	NSCanvist::DeleteAll $canvistId

if 1 {
	# Calculate row height
	set font [Value font,knowledge]
	set rowHgt [font metrics $font -linespace]
	if {[Info $oop hook] == "hook_feature"} {
		set iconSize $NSIconBrowser::Priv(maxIconHeight)
	} else {
		set iconSize [icon size]
	}
	if {$iconSize > $rowHgt} {
		set rowHgt $iconSize
	}
	incr rowHgt 8

	# Set row height of member list
	set canvas [NSCanvist::Info $canvistId canvas]
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt
}

	# Call hook to set the group list
	CallHook $oop set_list_group

	Info $oop group,current -1

	# Hack -- Clear the "member" list
	NSCanvist::DeleteAll [Info $oop member,canvistId]

	# Hack -- Set the new-item command
	if {[string equal [Info $oop hook] hook_feature]} {
		NSCanvist::Info [Info $oop member,canvistId] newRowCmd \
			"NSAssign::NewItemCmd_Feature $oop"
	} else {
		NSCanvist::Info [Info $oop member,canvistId] newRowCmd \
			"NSAssign::NewItemCmd $oop"
	}

	# Feedback
	StatusBar $oop "Done." 1

	return
}

# NSAssign::SetList_Member --
#
#	.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SetList_Member {oop group} {

	set canvistId [Info $oop member,canvistId]

	# Clear the list
	NSCanvist::DeleteAll $canvistId

	Info $oop member,current -1

if 1 {
	# Calculate row height
	set font [Value font,knowledge]
	set rowHgt [font metrics $font -linespace]
	if {[Info $oop hook] == "hook_feature"} {
		set iconSize $NSIconBrowser::Priv(maxIconHeight)
	} else {
		set iconSize [icon size]
	}
	if {$iconSize > $rowHgt} {
		set rowHgt $iconSize
	}
	incr rowHgt 8

	# Set row height of member list
	set canvas [NSCanvist::Info $canvistId canvas]
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt
}

	# Call hook to set the member list
	CallHook $oop set_list_member $group

	return
}

# NSAssign::SetList_Other --
#
#	Set the other list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SetList_Other {oop other} {

	set canvistId [Info $oop sprite,canvistId]
	set canvas [NSCanvist2::Info $canvistId canvas]

	# Clear the list
	NSCanvist2::DeleteAll $canvistId

# Is there a <Configure> event or something?
set width [winfo width $canvas]
set iconSize [expr {[icon size] + 8}]
set columns [expr {$width / $iconSize}]
if {$columns == 0} {set columns 1}
NSCanvist2::Info $canvistId columns $columns
	
	# Get the number of alternates/sprites */
	set max [$other count]

	# Add each alternate/sprite to the list
	for {set i 0} {$i < $max} {incr i} {

		# Append match to the list
		NSCanvist2::Append $canvistId "$other $i"
	}

	return
}


# NSAssign::SetDisplay --
#
#	Displays the list of what is assigned (icons or sprites).
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::SetDisplay {oop display} {

	set win [Info $oop win]
	set browserId [Info $oop browserId]

	switch -- $display {
		alternate {
			set frame $win.frameSprite
			SetList_Other $oop alternate
		}
		icon {
			set frame [NSIconBrowser::Info $browserId frame]
		}
		sprite {
			set frame $win.frameSprite
			SetList_Other $oop sprite
		}
	}

	grid forget [grid slaves $win -row 4]
	grid $frame \
		-row 4 -column 0 -rowspan 1 -columnspan 2 -sticky news

	Info $oop display $display

	# Radiobutton menu entries
	Info $oop radio,display $display

	return
}

# NSAssign::UpdateList --
#
#	Configure a canvas item on the row of a list.
#	When the user assigns an icon to a member, we want to update
#	the member list, and sometimes the group list too.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::UpdateList {oop which row tag args} {

	set canvistId [Info $oop $which,canvistId]
	set canvas [Info $oop ${which}Canvas]
	
	set rowTag [lindex [NSCanvist::Info $canvistId rowTags] $row]
	set itemIdList [$canvas find withtag $rowTag]
	set itemId [FindItemByTag $canvas $itemIdList $tag]
	eval $canvas itemconfigure $itemId $args

	return
}

# NSAssign::DisplayMember --
#
#	Find the group the given member is in, and display it.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::DisplayMember {oop hook member} {

	foreach group [CallHook $oop group_list] {
		set memberList [CallHook $oop member_list $group]
		set index [lsearch -exact $memberList $member]
		if {$index != -1} {
			Info $oop group,$hook $group
			Info $oop member,$hook $index
			break
		}
	}

	return
}

# NSAssign::Find --
#
#	Simple search routine to look for a member by name.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::Find {oop again} {

	variable Priv

	# Repeat the last search
	if {$again && [string length $Priv(find,string)]} {
		set string $Priv(find,string)

	# Enter a string to find, start from the beginning
	} else {

		# Ask the user for a name
		set string [NSUtils::StringBox -title [mc Find] \
			-initial $Priv(find,string) -prompt [mc find-prompt] \
			-buttons [list [mc Find] [mc Cancel]] -parent [Info $oop win]]

		# User cancelled
		if {![string length $string]} return

		# Clean up after the dialog, give message
		StatusBar $oop "Searching..." 1
		update

		# Reset search parameters
		set Priv(find,string) $string
	}

	# Default to searching from the beginning
	set group 0
	set member 0

	# Search in selected group, if any
	if {!$Priv(find,fromStart)} {	
		set groupCurr [Info $oop group,current]
		set memberCurr [Info $oop member,current]
		if {$groupCurr != -1} {
			set group $groupCurr
		}
		if {$memberCurr != -1} {
			set member [expr {$memberCurr + 1}]
		}
	}

	set max [NSCanvist::Info [Info $oop group,canvistId] count]

	# Compare lowercase
	set string [string tolower $string]

	# Check each group
	for {set i $group} {$i < $max} {incr i} {

		# Get a list of member indexes
		set match [lrange [CallHook $oop member_list $i] $member end]

		# Check each member index
		foreach index $match {
	
			# Get the member name
			set name2 [CallHook $oop member_name $i $index]

			# Compare lowercase
			set name2 [string tolower $name2]

			# Found a match
			if {[string first $string $name2] != -1} {
	
				set canvistId [Info $oop group,canvistId]
	
				# The new group is not displayed
				if {![NSCanvist::IsRowSelected $canvistId $i]} {

					# Clear the current selection
					NSCanvist::RemoveSelection $canvistId
					
					# Select the new group. As a side effect, the
					# SetList_Member() command is called to display
					# the monsters in the group.
					NSCanvist::UpdateSelection $canvistId $i {}
					NSCanvist::See $canvistId $i

				# The new group is already selected
				} else {
				}
	
				set canvistId [Info $oop member,canvistId]
	
				# Select the matching member, deselecting all others
				NSCanvist::UpdateSelection $canvistId $member all
				NSCanvist::See $canvistId $member
	
				# Don't search from start next time
				set Priv(find,fromStart) 0

				# Clear "Searching..." message
				StatusBar $oop "" 0

				# Done
				return
			}

			# Next row
			incr member
		}

		# First row
		set member 0
	}

	# If we didn't search from the start, then wrap around
	if {!$Priv(find,fromStart)} {
		set Priv(find,fromStart) 1
		Find $oop 1
		return
	}

	StatusBar $oop "No match for \"$string\"." 1

	return
}

# NSAssign::StatusBar --
#
#	Display text in the status bar, perhaps clearing it later.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::StatusBar {oop text zap} {

	set win [Info $oop win]
	set label [$win.statusBar itemcget t1 -label]
	$label configure -text $text
	if {$zap} {
		NSUtils::ZapLabel $label
	}

	return
}

# NSAssign::NewItemCmd --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::NewItemCmd {oop canvistId y icon text {hasSound 0}} {

	set c [NSCanvist::Info $canvistId canvas]
	set lineHeight [NSCanvist::Info $canvistId rowHgt]
	set font [Value font,knowledge]

	set fh [font metrics $font -linespace]
	set diff [expr {int([expr {($lineHeight - $fh) / 2}])}]

	# Selection rectangle around everything
	lappend itemIdList [$c create rectangle 2 [expr {$y + 2}] \
		[expr {([winfo width $c] - 1) - 2}] [expr {$y + $lineHeight - 2}] \
		-fill {} -outline {} -tags selrect -width 2.0]

	# Image
	if {[Info $oop hook] == "hook_feature"} {
		set iw $NSIconBrowser::Priv(maxIconWidth)
		set ih $NSIconBrowser::Priv(maxIconHeight)
	} else {
		set iw [icon size]
		set ih [icon size]
	}
if 0 {
	set wid [expr {$iw + 8}]
	set xdiff [expr {int([expr {($wid - $iw) / 2}])}]
	set ydiff [expr {int([expr {($lineHeight - $ih) / 2}])}]
}
	lappend itemIdList [$c create widget [expr {4 + $iw / 2}] \
		[expr {$y + 4 + $ih / 2}] \
		-assign $icon -anchor center -tags icon]

	# Text
	set fill White
	set textLeft [expr {4 + $iw + 4 + 1}]
	lappend itemIdList [$c create text $textLeft [expr {$y + $diff}] \
		-text $text -anchor nw -font $font -fill $fill -tags text]

	# Sound image
	set image Image_Empty
	if {$hasSound} {
		set image Image_Sound
	}
	set iw [image width Image_Sound]
	set ih [image height Image_Sound]
	set ydiff [expr {int([expr {($lineHeight - $ih) / 2}])}]
	set x [expr {$textLeft + [font measure $font $text] + 4}]
	lappend itemIdList [$c create image $x [expr {$y + $diff}] \
		-image $image -anchor nw -tags image]

	return $itemIdList
}

# NSAssign::NewItemCmd_Feature --
#
#	Called by NSCanvist::InsertItem() to create a list row.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::NewItemCmd_Feature {oop canvistId y iconFG iconBG text} {

	set c [NSCanvist::Info $canvistId canvas]
	set rowHgt [NSCanvist::Info $canvistId rowHgt]
	set font [Value font,knowledge]

	set fh [font metrics $font -linespace]
	set diff [expr {int([expr {($rowHgt - $fh) / 2}])}]

	# Selection rectangle around everything
	lappend itemIdList [$c create rectangle 2 [expr {$y + 2}] \
		[expr {([winfo width $c] - 1) - 2}] [expr {$y + $rowHgt - 2}] \
		-fill {} -outline {} -tags selrect -width 2.0]

	# Image 1
	set iw $NSIconBrowser::Priv(maxIconWidth)
	set ih $NSIconBrowser::Priv(maxIconHeight)
if 0 {
	set wid [expr {$iw + 8}]
	set xdiff [expr {int([expr {($wid - $iw) / 2}])}]
	set ydiff [expr {int([expr {($rowHgt - $ih) / 2}])}]
}
	lappend itemIdList [$c create widget [expr {4 + $iw / 2}] \
		[expr {$y + 4 + $ih / 2}] \
		-assign $iconFG -anchor center -tags icon]

	# Image 2
	lappend itemIdList [$c create widget [expr {8 + $iw + $iw / 2}] \
		[expr {$y + 4 + $ih / 2}] \
		-assign $iconBG -anchor center -tags iconBG]

	# Text
	set fill White
	set textLeft [expr {4 + $iw + 4 + $iw + 4}]
	lappend itemIdList [$c create text $textLeft [expr {$y + $diff}] \
		-text $text -anchor nw -font $font -fill $fill -tags text]

	return $itemIdList
}

# NSAssign::HighlightItemCmd --
#
#	Called by NSCanvist::Select() to highlight a row.
#
# Arguments:
#	oop					OOP ID. See above.
#	canvistId					OOP ID of NSCanvist object.
#	state					1 or 0 highlight state.
#	args					List of canvas item ids
#
# Results:
#	What happened.

proc NSAssign::HighlightItemCmd {oop canvistId state args} {

	set canvas [NSCanvist::Info $canvistId canvas]
	set itemIdList $args

	set idRect [FindItemByTag $canvas $itemIdList selrect]

	if {[NSUtils::HasFocus $canvas]} {
		set fill [Value listHilite]
	} else {
		set fill [Value listInactive]
	}

	if {$state} {
		$canvas itemconfigure $idRect -outline $fill

	} else {
		$canvas itemconfigure $idRect -outline {}
	}

	return
}

# NSAssign::Configure --
#
#	Called as a <Configure> event script. Resizes the selection rectangles
#	so they fit properly.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::Configure {oop canvas} {

	# Get the width of the canvas
	set canvasWidth [winfo width $canvas]

	foreach itemId [$canvas find withtag selrect] {
		set coords [$canvas coords $itemId]
		set right [expr {($canvasWidth - 1) - 2}]
		set coords [lreplace $coords 2 2 $right]
		eval $canvas coords $itemId $coords
	}

	return
}

# NSAssign::InvokeTab --
#
#	Called when a tab is clicked.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::InvokeTab {oop tabsId tabId} {

	variable Priv
	set index [lsearch -exact [NSTabs::Info $tabsId id] $tabId]
	SetHook $oop [lindex $Priv(hook) [expr {$index * 2}]]

	return
}

# NSAssign::Progress --
#
#	Show, update, and hide the progress bar.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::Progress {oop action args} {

	set win [Info $oop win]
	set progId [Info $oop progId]
	
	switch -- $action {
		open {
			NSProgress2::Zero $progId
			pack [NSProgress2::Info $progId frame] -pady 1 -fill x
			update idletasks
			Info $oop oldLabel2 [$win.statusBar itemcget t2 -text]
		}
		update {
			set cur [lindex $args 0]
			set max [lindex $args 1]
			set bump [expr {(($max / 20) > 40) ? ($max / 20) : 40}]
			if {$cur && ($cur % $bump) == 0} {
				NSProgress2::SetDoneRatio $progId [expr {$cur / double($max)}]
				$win.statusBar itemconfigure t2 -text "$cur/$max"
				update idletasks
			}
		}
		close {
			NSProgress2::SetDoneRatio $progId 1.0
			update idletasks
			pack forget [NSProgress2::Info $progId frame]
			$win.statusBar itemconfigure t2 -text [Info $oop oldLabel2]
		}
	}

	return
}

# NSAssign::BrowserCmd --
#
#	Called by NSIconBrowser when displaying an icon type. Display
#	the progress of listing the icons. Note that this can
#	actually slow down listing the icons.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::BrowserCmd {oop action args} {

	set win [Info $oop win]
	set progId [Info $oop progId]
	
	switch -- $action {
		open {
			StatusBar $oop {} 0
			NSProgress2::Zero $progId
			pack [NSProgress2::Info $progId frame] -pady 1 -fill x
			update idletasks
			Info $oop oldLabel2 [$win.statusBar itemcget t2 -text]
		}
		update {
			set cur [lindex $args 0]
			set max [lindex $args 1]
			set bump [expr {(($max / 20) > 40) ? ($max / 20) : 40}]
set bump [expr {$max / 20}]
			if {$cur && ($cur % $bump) == 0} {
				NSProgress2::SetDoneRatio $progId [expr {$cur / double($max)}]
				$win.statusBar itemconfigure t2 -text "$cur/$max"
				update idletasks
			}
		}
		close {
			NSProgress2::SetDoneRatio $progId 1.0
			update idletasks
			pack forget [NSProgress2::Info $progId frame]
			$win.statusBar itemconfigure t2 -text [Info $oop oldLabel2]
		}
	}

	return
}


proc NSAssign::hook_effect {oop message args} {

	switch -- $message {

		set_list_group {

			set canvistId [Info $oop group,canvistId]

			set assign "icon [effect assign ball fire]"
			NSCanvist::Insert $canvistId end $assign [mc Ball]

			set assign "icon [effect assign bolt fire]"
			NSCanvist::Insert $canvistId end $assign [mc Bolt]

			set assign "icon [effect assign ammo arrow]"
			NSCanvist::Insert $canvistId end $assign [mc Ammunition]
		}

		set_list_member {
			set group [lindex $args 0]
			set effectType [lindex [effect groups] $group]

			set canvistId [Info $oop member,canvistId]
			foreach effect [effect names $effectType] {
				set assign "icon [effect assign $effectType $effect]"
				NSCanvist::Insert $canvistId end $assign $effect
				lappend match $effect
			}

			# Keep a list of matching effect names
			Info $oop member,match $match
		}

		assign {
			set count 0
			set what [lindex $args 0]
			set group [Info $oop group,current]
			set effectType [lindex [effect groups] $group]
			switch -- $what {
				alternate {
					tk_messageBox -title "Assign" -icon info \
						-message "You can only assign alternates to\
						objects" -parent [Info $oop win]
				}
				icon {
					set iconType [lindex $args 1]
					set iconIndex [lindex $args 2]
					set assign "icon $iconType $iconIndex"
					set canvistId [Info $oop member,canvistId]
					foreach row [NSCanvist::Selection $canvistId] {
						set effectName [lindex [Info $oop member,match] $row]
						effect assign $effectType $effectName \
							-type $iconType -index $iconIndex
						UpdateList $oop member $row icon -assign $assign
						incr count
					}
					if {[string compare $effectType ammo] && [string equal $effectName fire]} {
						set row [Info $oop group,current]
						UpdateList $oop group $row icon -assign $assign
					}
					if {[string equal $effectType ammo] && [string equal $effectName arrow]} {
						set row [Info $oop group,current]
						UpdateList $oop group $row icon -assign $assign
					}
				}
			}
			return $count
		}

		get_icon {
			set row [lindex $args 0]
			set effectName [lindex [Info $oop member,match] $row]
			set group [Info $oop group,current]
			set effectType [lindex [effect groups] $group]
			return "icon [effect assign $effectType $effectName]"
		}

		select_member {
		}

		group_names {
			return [list Ball Bolt Ammo]
		}

		member_name {
			set group [lindex $args 0]
			set index [lindex $args 1]
			set effectType [lindex [effect groups] $group]
			return [lindex [effect names $effectType] $index]
		}

		member_list {
			set group [lindex $args 0]
			set effectType [lindex [effect groups] $group]
			set index -1
			foreach effect [effect names $effectType] {
				lappend match [incr index]
			}
			return $match
		}
	}

	return
}

proc NSAssign::hook_feature {oop message args} {

	global NSAssign

	set feat_name [list \
		"nothing" "open floor" "invisible trap" "glyph of warding" \
		"open door" "broken door" "up staircase" "down staircase" \
		"quest entrance" "quest exit" "quest down level" "quest up level" \
		"town exit" "" "" "" "trap door" "open pit" \
		"spiked pit" "poisoned pit" "strange rune (summon)" \
		"strange rune (teleport)" "discolored spot (fire)" \
		"discolored spot (acid)" "dart trap (slow)" "dart trap (strength)" \
		"dart trap (dexterity)" "dart trap (constitution)" \
		"gas trap (blind)" "gas trap (confuse)" "gas trap (poison)" \
		"gas trap (sleep)" "locked door" "locked door" "locked door" \
		"locked door" "locked door" "locked door" "locked door" \
		"locked door" "jammed door" "jammed door" "jammed door" \
		"jammed door" "jammed door" "jammed door" "jammed door" \
		"jammed door" "secret door" "pile of rubble" "magma vein" \
		"quartz vein" "magma vein + hidden treasure" "quartz vein + hidden treasure" \
		"magma vein + treasure" "quartz vein + treasure" \
		"granite wall (basic)" "granite wall (inner)" \
		"granite wall (outer)" "granite wall (solid)" \
		"permanent wall (basic)" "permanent wall (inner)" \
		"permanent wall (outer)" "permanent wall (solid)" \
		"explosive rune" "Pattern startpoint" \
		"section of the Pattern" "section of the Pattern" \
		"section of the Pattern" "section of the Pattern" \
		"section of the Pattern" "section of the Pattern (discharged)" \
		"Pattern exit" "corrupted section of the Pattern" \
		"General Store" "Armoury" "Weapon Smiths" "Temple" "Alchemy Shop" \
		"Magic Shop" "Black Market" "Home" "Bookstore" \
		"a pool of deep water" "stream of shallow water" \
		"a pool of deep lava" "a stream of shallow lava" \
		"dark pit" "dirt" "patch of grass" \
		"compact rune" "invisible wall" "" "" "" "" \
		"tree" "mountain chain" \
		"" "" "" "" "" "" "" "" "" "" "" "" "" "" \
		"" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" \
		Building Building Building Building Building \
		Building Building Building Building Building \
		Building Building Building Building Building \
		Building Building Building Building Building \
		Building Building Building Building Building \
		Building Building Building Building Building \
		Building Building \
	]

	set showAll 0

	switch -- $message {

		set_list_group {

			set canvistId [Info $oop group,canvistId]

			# Use down staircase as the group icon
			set icon [assign set feature [const FEAT_MORE]]
			NSCanvist::Insert $canvistId end $icon [mc Features]

			set hookMenu [Info $oop hookMenu,menu]

			# Build a list of entry identifiers
			set ident {}
if 0 {
			# Lighting
			set menu $hookMenu.light
			menu $menu -tearoff 0
			bind $menu <<MenuSelect>> \
				"NSAssign::MenuSelect_Hook $oop light %W"
			$hookMenu add cascade -menu $menu -label [mc Lighting]
			lappend ident m_light
			$menu add radiobutton -label [mc Icon] \
				-variable NSAssign($oop,hookMenu,light) -value icon \
				-command "NSAssign::CallHook $oop menu_cmd light"
			$menu add radiobutton -label [mc None] \
				-variable NSAssign($oop,hookMenu,light) -value none \
				-command "NSAssign::CallHook $oop menu_cmd light"
			$menu add radiobutton -label [mc Tint] \
				-variable NSAssign($oop,hookMenu,light) -value tint \
				-command "NSAssign::CallHook $oop menu_cmd light"
			Info $oop hookMenu,ident,light [list icon none tint]
}
			# Background
			$hookMenu add command -label [mc Background...] \
				-command "NSAssign::CallHook $oop menu_cmd background"
			lappend ident background

			# Optional memorize
			$hookMenu add checkbutton -label [mc Memorize] \
				-variable NSAssign($oop,hookMenu,boring) \
				-onvalue 0 -offvalue 1 \
				-command "NSAssign::CallHook $oop menu_cmd boring"
			lappend ident boring

			# Force lighting in town at night
			$hookMenu add checkbutton -label [mc "Town Light"] \
				-variable NSAssign($oop,hookMenu,town) \
				-onvalue 1 -offvalue 0 \
				-command "NSAssign::CallHook $oop menu_cmd town"
			lappend ident town

			Info $oop hookMenu,ident,hook $ident
		}

		set_list_member {
			set canvistId [Info $oop member,canvistId]
			set f_idx 0

			# Collect info for each row
			set itemList {}

			# Check each feature name
			foreach name $feat_name {

				# This is a real feature
				if {$showAll || ([string length $name] &&
					($f_idx == [struct set feature_type $f_idx mimic]))} {

					# Get the icon assigned to that feature
					set icon [assign set feature $f_idx]

					# ... and the background icon
					set bg_idx [feature configure $f_idx -background]
					set iconBG [assign set feature $bg_idx]

					# Make sure binary and feat_name agree
					if {$::DEBUG} {
						set name2 [angband f_info info $f_idx name]
						append name " \[$name2\]"
					}

					# Collect info for each row
					lappend itemList [list $icon $iconBG $name]

					# Keep a list of matchin indexes
					lappend match $f_idx

				} else {
					set name2 [angband f_info info $f_idx name]
				}

				# Next feature
				incr f_idx
			}

			# Add each row to the list
			NSCanvist::InsertMany $canvistId end $itemList

			# Keep a list of matching indexes
			Info $oop member,match $match
		}

		assign {

			# Count number of assignments actually made
			set count 0

			# What is being assigned?
			set what [lindex $args 0]
			
			switch -- $what {
				alternate -
				icon -
				sprite {
					# Build the assignment, such as "sprite 2"
					set assign $what
					foreach arg [lrange $args 1 end] {
						append assign " $arg"
					}
					set canvistId [Info $oop member,canvistId]
					set match [Info $oop member,match]

					# Check each selected member
					foreach row [NSCanvist::Selection $canvistId] {

						# Get the feature index
						set f_idx [lindex $match $row]

						# Assign to the feature
						assign set feature $f_idx $assign

						# Update the member list
						UpdateList $oop member $row icon -assign $assign

						# Count assignments
						incr count

						# Hack -- Down staircase is used as group icon
						if {$f_idx == [const FEAT_MORE]} {
							set row 0
							UpdateList $oop group $row icon -assign $assign
						}

						# Hack -- Update list for features using this
						# feature as the background
						set row2 0
						foreach f2_idx $match {
							set bg_idx [feature configure $f2_idx -background]
							if {$bg_idx == $f_idx} {
								UpdateList $oop member $row2 iconBG -assign $assign
							}
							incr row2
						}
					}
				}
			}

			# Return the number of assignments made
			return $count
		}

		get_icon {
			set row [lindex $args 0]
			set f_idx [lindex [Info $oop member,match] $row]
			return [assign set feature $f_idx]
		}

		select_member {
			set row [lindex $args 0]
			set f_idx [lindex [Info $oop member,match] $row]
if 0 {
			Info $oop hookMenu,light [feature configure $f_idx -light]
}
			Info $oop hookMenu,boring [feature configure $f_idx -boring]
			Info $oop hookMenu,town [feature configure $f_idx -town]
		}

		group_names {
			return Feature
		}

		member_name {
			set group [lindex $args 0]
			set index [lindex $args 1]
			return [lindex $feat_name $index]
		}

		member_list {
			set f_idx 0
			foreach name $feat_name {
				if {$showAll || ([string length $name] &&
					($f_idx == [struct set feature_type $f_idx mimic]))} {
					lappend match $f_idx
				}
				incr f_idx
			}
			return $match
		}

		menu_setup {
			set hookMenu [Info $oop hookMenu,menu]
			if {[Info $oop member,current] != -1} {
				menuentrystate $hookMenu 0 normal
				menuentrystate $hookMenu 1 normal
				menuentrystate $hookMenu 2 normal
				menuentrystate $hookMenu 3 normal
			}
		}

		menu_cmd {
			set canvistId [Info $oop member,canvistId]
			set entry [lindex $args 0]
			switch -- $entry {
				background {
					set row [Info $oop member,current]
					set f_idx [lindex [Info $oop member,match] $row]
					set initial [feature configure $f_idx -background]
					set bg [NSUtils::StringBox -title "Set Feature Background" \
						-initial $initial -prompt "Feature Index" \
						-buttons [list [mc OK] [mc Cancel]] \
						-parent [Info $oop win]]
					if {![string length $bg]} return
					foreach row [NSCanvist::Selection $canvistId] {
						set f_idx [lindex [Info $oop member,match] $row]
						feature configure $f_idx -background $bg
						set assign [assign set feature $bg]
						UpdateList $oop member $row iconBG -assign $assign
					}
					StatusBar $oop "Set feature background to \"$bg\"." 1
				}
				light {
if 0 {
					set light [Info $oop hookMenu,light]
					foreach row [NSCanvist::Selection $canvistId] {
						set f_idx [lindex [Info $oop member,match] $row]
						feature configure $f_idx -light $light
					}
					StatusBar $oop "Set feature lighting to \"$light\"." 1
}
				}
				boring -
				town {
					set value [Info $oop hookMenu,$entry]
					foreach row [NSCanvist::Selection $canvistId] {
						set f_idx [lindex [Info $oop member,match] $row]
						feature configure $f_idx -$entry $value
					}
					StatusBar $oop "Set \"-$entry\" to \"$value\"." 1
				}
			}
		}

		menu_string {
			switch -- [lindex $args 1] {
				m_light {
if 0 {
					return "Contains options which control lighting."
}
				}
				background {
					return "Sets the feature index to use as the background for masked icons."
				}
				boring {
					return "The feature is memorized."
				}
				town {
					return "The feature is always brightest on the town level."
				}
				icon {
if 0 {
					return "The feature uses a sequence of icons for lighting."
}
				}
				none {
if 0 {
					return "The feature is always brightest."
}
				}
				tint {
if 0 {
					return "The feature uses a tint table for lighting."
}
				}
				default {
					return ""
				}
			}
		}
	}

	return
}

proc NSAssign::hook_monster {oop message args} {

	switch -- $message {

		set_list_group {

			set canvistId [Info $oop group,canvistId]

			foreach {title findSpec} [Global groups,r_info] {

				# Find the last monster in the group
				set match [eval angband r_info find -unique no -limit 1 \
					-backwards $findSpec]

				if {![llength $match]} {
					tk_messageBox -message "Monster group \"$title\" is empty!"
					continue
				}

				# Get the icon
				set icon [angband r_info info [lindex $match 0] icon]
		
				# Add this group to the list
				NSCanvist::Insert $canvistId end $icon [mc $title]
			}
		}

		set_list_member {

			set config [Info $oop sound,config]
			set canvistId [Info $oop member,canvistId]

			set group [lindex $args 0]
			set findSpec [lindex [Global groups,r_info] [expr {$group * 2 + 1}]]

			# Get a list of monsters in the group
			set match [eval angband r_info find -unique no $findSpec]

			# Collect info for each row
			set itemList {}

			# Add each match to the list
			foreach index $match {
		
				# Get the icon and name
				set icon [angband r_info info $index icon]
				set name [angband r_info info $index name]

				# Are sounds assigned?
				set hasSound 0

				# Collect info for each row
				lappend itemList [list $icon $name $hasSound]
			}

			# Add each row to the list
			NSCanvist::InsertMany $canvistId end $itemList

			# Keep a list of matching indexes
			Info $oop member,match $match
		}

		assign {
			set count 0
			set canvistId [Info $oop member,canvistId]
			set what [lindex $args 0]
			switch -- $what {
				alternate {
					tk_messageBox -title "Assign" -icon info \
						-message "You can only assign alternates to\
						features and objects" -parent [Info $oop win]
				}
				icon -
				sprite {
					# Build the assignment, such as "sprite 2"
					set assign $what
					foreach arg [lrange $args 1 end] {
						append assign " $arg"
					}
					set match [Info $oop member,match]
					foreach row [NSCanvist::Selection $canvistId] {
						set index [lindex $match $row]
						assign set monster $index $assign
						UpdateList $oop member $row icon -assign $assign
						incr count
					}
					if {$row == ([NSCanvist::Info $canvistId count] - 1)} {
						set row [Info $oop group,current]
						UpdateList $oop group $row icon -assign $assign
					}
				}
			}
			return $count
		}

		get_icon {
			set row [lindex $args 0]
			set index [lindex [Info $oop member,match] $row]			
			return [angband r_info info $index icon]
		}

		select_member {
			set row [lindex $args 0]
			set index [lindex [Info $oop member,match] $row]
			NSRecall::RecallMonster $index
		}

		group_names {
			set names {}
			foreach {title findSpec} [Global groups,r_info] {
				lappend names [mc $title]
			}
			return $names
		}

		group_list {
			set index -1
			set result {}
			foreach {title findSpec} [Global groups,r_info] {
				lappend result [incr index]
			}
			return $result
		}

		member_name {
			set group [lindex $args 0]
			set index [lindex $args 1]
			return [angband r_info info $index name]
		}

		member_list {
			set group [lindex $args 0]
			set findSpec [lindex [Global groups,r_info] [expr {$group * 2 + 1}]]
			return [eval angband r_info find -unique no $findSpec]
		}
	}	

	return
}

proc NSAssign::hook_object {oop message args} {

	switch -- $message {

		set_list_group {

			set canvistId [Info $oop group,canvistId]

			foreach {title findSpec} [Global groups,k_info] {

				# Find the last monster in the group
				set match [eval angband k_info find -limit 1 \
					-backwards $findSpec]

				# Get the assignment. This resolves alternates.
				set icon [angband k_info info [lindex $match 0] icon]
		
				# Add this group to the list
				NSCanvist::Insert $canvistId end $icon [mc $title]
			}
		}

		set_list_member {

			set canvistId [Info $oop member,canvistId]

			set group [lindex $args 0]
			set findSpec [lindex [Global groups,k_info] [expr {$group * 2 + 1}]]

			# Get a list of monsters in the group
			set match [eval angband k_info find $findSpec]

			# Collect info for each row
			set itemList {}

			# Add each match to the list
			foreach index $match {
		
				# Get the assignment. This resolves alternates.
				set icon [angband k_info info $index icon]

				# Hack -- object_desc
				set name [angband k_info info $index object_desc]

				# Collect info for each row
				lappend itemList [list $icon $name]
			}

			# Add each row to the list
			NSCanvist::InsertMany $canvistId end $itemList

			# Keep a list of matching indexes
			Info $oop member,match $match
		}

		assign {
			set count 0
			set canvistId [Info $oop member,canvistId]
			set what [lindex $args 0]
			switch -- $what {
				alternate -
				icon -
				sprite {
					# Build the assignment, such as "sprite 2"
					set assign $what
					foreach arg [lrange $args 1 end] {
						append assign " $arg"
					}
					set match [Info $oop member,match]
					foreach row [NSCanvist::Selection $canvistId] {
						set index [lindex $match $row]
						assign set object $index $assign
						set assign [angband k_info info $index icon]
						UpdateList $oop member $row icon -assign $assign
						incr count
					}
					if {$row == ([NSCanvist::Info $canvistId count] - 1)} {
						set row [Info $oop group,current]
						UpdateList $oop group $row icon -assign $assign
					}
				}
			}
			return $count
		}

		get_icon {
			set row [lindex $args 0]
			set index [lindex [Info $oop member,match] $row]

			# Return the assignment. Does not resolve alternates.
#			return [angband k_info info $index icon]
			return [assign set object $index]
		}

		select_member {
			set row [lindex $args 0]
			set index [lindex [Info $oop member,match] $row]
			NSRecall::RecallObjectKind $index
		}

		group_names {
			set names {}
			foreach {title findSpec} [Global groups,k_info] {
				lappend names [mc $title]
			}
			return $names
		}

		group_list {
			set index -1
			set result {}
			foreach {title findSpec} [Global groups,k_info] {
				lappend result [incr index]
			}
			return $result
		}

		member_name {
			set group [lindex $args 0]
			set index [lindex $args 1]
			return [angband k_info info $index object_desc]
		}

		member_list {
			set group [lindex $args 0]
			set findSpec [lindex [Global groups,k_info] [expr {$group * 2 + 1}]]
			return [eval angband k_info find $findSpec]
		}
	}

	return
}

proc NSAssign::hook_XXX {oop message args} {

	switch -- $message {

		set_list_group {
		}

		set_list_member {
		}

		assign {
		}

		get_icon {
		}

		select_member {
		}

		member_name {
		}

		member_list {
		}
	}

	return
}

# NSAssign::ValueChanged_font_knowledge --
#
#	Called when the font,knowledge value changes.
#	Updates the Assign Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::ValueChanged_font_knowledge {oop} {

	# Get the desired font
	set font [Value font,knowledge]

	# Calculate row height
	set rowHgt [font metrics $font -linespace]
	if {[icon size] > $rowHgt} {
		set rowHgt [icon size]
	}
	incr rowHgt 8

	# Set row height of group list
	set canvistId [Info $oop group,canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt

	# Set the width of the group list
	set oldWidth [winfo width $canvas]
	set newWidth [CalcGroupListWidth $oop]
	$canvas configure -width $newWidth

	# Set the scrollregion to prevent horizontal scrolling
	scan [$canvas cget -scrollregion] "%s %s %s %s" x1 y1 x2 y2
	$canvas configure -scrollregion "$x1 $y1 $newWidth $y2"

	# Hack -- Resize the toplevel so the member list is not resized
	set diff [expr {$newWidth - $oldWidth}]
	if {$diff} {
		set win [Info $oop win]
		set newWidth [expr {[winfo width $win] + $diff}]
		NSToplevel::SetTotalWidth $win $newWidth
	}

	# Set row height of member list
	set canvistId [Info $oop member,canvistId]
	set canvas [NSCanvist::Info $canvistId canvas]
	NSCanvist::Info $canvistId rowHgt $rowHgt
	$canvas configure -yscrollincrement $rowHgt

	###
	set rowHgt [font metrics [Value font,knowledge] -linespace]
	incr rowHgt 8

	return
}

# NSAssign::ValueChanged_listBG --
#
#	Called when the listBG value changes.
#	Updates the Assign Window.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::ValueChanged_listBG {oop} {

	set color [Value listBG]
	
	[Info $oop groupCanvas] configure -background $color
	[Info $oop memberCanvas] configure -background $color
	[Info $oop spriteCanvas] configure -background $color

	return
}

# NSAssign::CalcGroupListWidth --
#
#	Returns the desired width of the group list.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::CalcGroupListWidth {oop} {

	variable Priv

	# Get the desired font
	set font [Value font,knowledge]

	# Minimum desired width of the group list
	set maxWidth 100

	# Check each hook
	foreach {hook title} $Priv(hook) {

		# Check each name
		foreach name [$hook $oop group_names] {

			# Calculate the width in pixels
			set width [font measure $font $name]
			
			# Remember the maximum width
			if {$width > $maxWidth} {
				set maxWidth $width
			}
		}
	}

	return [expr {[icon size] + 8 + $maxWidth + 20 + 4}]
}

# NSAssign::BrowserMotionCmd --
#
#	Called by NSIconBrowser.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::BrowserMotionCmd {oop which iconIndex} {

	set browserId [Info $oop browserId]

	# No icon was hit
	if {$iconIndex == -1} {
		StatusBar $oop {} 0
		return
	}

	# The mouse moved over the group canvas (ie, icon types)
	if {[string equal $which group]} {
		set iconType [lindex [NSIconBrowser::Info $browserId iconTypes] $iconIndex]
		StatusBar $oop "Icon type \"$iconType\"." 0
		return
	}

	set iconType [NSIconBrowser::Info $browserId iconType]
	set assign "icon $iconType $iconIndex"

	# Check each feature
	set max [angband f_info max]
	for {set index 0} {$index < $max} {incr index} {
		set assign2 [assign set feature $index]
		if {[string equal $assign $assign2]} {
			set name [angband f_info info $index name]
			StatusBar $oop "($iconType $iconIndex) [mc Feature] #$index: \"$name\"" 0
			return
		}
	}

	# Check each object
	set max [angband k_info max]
	for {set index 0} {$index < $max} {incr index} {
		set assign2 [assign set object $index]
		if {[string equal $assign $assign2]} {
			set name [angband k_info info $index object_desc]
			StatusBar $oop "($iconType $iconIndex) [mc Object] #$index: \"$name\"" 0
			return
		}
	}

	# Check each monster
	set max [angband r_info max]
	for {set index 0} {$index < $max} {incr index} {
		set assign2 [assign set monster $index]
		if {[string equal $assign $assign2]} {
			set name [angband r_info info $index name]
			StatusBar $oop "($iconType $iconIndex) [mc Monster] #$index: \"$name\"" 0
			return
		}
	}

	StatusBar $oop "($iconType $iconIndex) [mc {Not assigned.}]" 0

	return
}

# NSAssign::DisplayIcon --
#
#	When statusBar.label2 is clicked, we display the icon of the
#	selected member, if any.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSAssign::DisplayIcon {oop} {

	set browserId [Info $oop browserId]
	set row [Info $oop member,current]
	if {$row == -1} return

	set icon [CallHook $oop get_icon $row]
	scan $icon "%s" assignType
	if {[string compare $assignType icon]} return

	if {[string compare [Info $oop display] icon]} {
		SetDisplay $oop icon
	}

	scan $icon "%s %s %d" assignType iconType iconIndex
	if {[lsearch -exact [NSIconBrowser::Info $browserId iconTypes] $iconType] != -1} {
		Info $oop display,ignoreSel 1
		NSIconBrowser::SeeIcon $browserId $iconType $iconIndex
		Info $oop display,ignoreSel 0
	}

	return
}
