# File: value-manager.tcl

# Purpose: the Value Manager

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

# The Value Manager records various values, and informs clients when those
# values are modified.

namespace eval NSValueManager {

	variable Command
	variable Value
	variable ValuePriv

# namespace eval NSValueManager
}

# NSValueManager::InitModule --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::InitModule {} {

	global Angband
	variable Value
	variable ValuePriv

	# Each client gets a unique ID
	set ValuePriv(clientId) 0

	# These are the default values. The tk/config/value file is sourced
	# below to set user-defined values.
	
	Manage healthBarBD [palette set 220]
	Manage healthBarBD2 145
	Manage healthBarBL [palette set 35]
	Manage healthBarBL2 170
	Manage healthBarDone [palette set 71]
	Manage healthBarDone2 145
	Manage healthBarToDo [palette set 129]
	Manage healthBarToDo2 145

	Manage healthNameBD [palette set 252]
	Manage healthNameBD2 145
	Manage healthNameBG [palette set 252]
	Manage healthNameBG2 145
	Manage healthNameBL [palette set 252]
	Manage healthNameBL2 145
	Manage healthNameText [palette set 0]
	Manage healthNameText2 180

	Manage friendBarBD [palette set 212]
	Manage friendBarBD2 145
	Manage friendBarBL [palette set 192]
	Manage friendBarBL2 145
	Manage friendBarDone [palette set 198]
	Manage friendBarDone2 160
	Manage friendBarToDo [palette set 129]
	Manage friendBarToDo2 145
	Manage listBG [palette set 253]
	Manage listHilite [palette set 158]
	Manage listInactive [palette set 249]

	Manage statusGoodBD [palette set 230]
	Manage statusGoodBD2 145
	Manage statusGoodBG [palette set 197]
	Manage statusGoodBG2 145
	Manage statusGoodBL [palette set 191]
	Manage statusGoodBL2 145
	Manage statusGoodText [palette set 0]
	Manage statusGoodText2 180

	Manage statusInfoBD [palette set 240]
	Manage statusInfoBD2 145
	Manage statusInfoBG [palette set 212]
	Manage statusInfoBG2 145
	Manage statusInfoBL [palette set 210]
	Manage statusInfoBL2 170
	Manage statusInfoText [palette set 0]
	Manage statusInfoText2 180

	Manage statusBadBD [palette set 220]
	Manage statusBadBD2 145
	Manage statusBadBG [palette set 107]
	Manage statusBadBG2 145
	Manage statusBadBL [palette set 71]
	Manage statusBadBL2 170
	Manage statusBadText [palette set 0]
	Manage statusBadText2 180

	Manage targetBD [palette set 220]
	Manage targetBD2 145
	Manage targetBG [palette set 71]
	Manage targetBG2 145
	Manage targetBL [palette set 35]
	Manage targetBL2 170
	Manage targetText [palette set 0]
	Manage targetText2 180

	Manage target2BD [palette set 253]
	Manage target2BD2 145
	Manage target2BG [palette set 129]
	Manage target2BG2 145
	Manage target2BL [palette set 0]
	Manage target2BL2 145
	Manage target2Text [palette set 0]
	Manage target2Text2 180

	# These are the standard Angband terminal colors mapped to
	# entries in our 256-color palette. They can be accessed via
	# "Value TERM_XXX".
	
	Manage TERM_DARK [palette set 255]
	Manage TERM_WHITE [palette set 0]
	Manage TERM_SLATE [palette set 250]
	Manage TERM_ORANGE [palette set 17]
	Manage TERM_RED [palette set 217]
	Manage TERM_GREEN [palette set 227]
	Manage TERM_BLUE [palette set 204]
	Manage TERM_UMBER [palette set 101]
	Manage TERM_L_DARK [palette set 129]
	Manage TERM_L_WHITE [palette set 247]
	Manage TERM_VIOLET [palette set 30]
	Manage TERM_YELLOW [palette set 5]
	Manage TERM_L_RED [palette set 35]
	Manage TERM_L_GREEN [palette set 185] ; # ~LawnGreen
	Manage TERM_L_BLUE [palette set 180] ; # ~turquoise2
	Manage TERM_L_UMBER [palette set 52]
	
	Manage ambient_delay 120

	Manage tip,current 1000
	Manage tip,show 1

	Manage progress,showBars 1
	Manage progress,showNumbers 1

	Manage recall,show 1
	Manage recall,showicon 1

	Manage record,dump 1
	Manage record,message 1
	Manage record,photo 1

	Manage bigmap,scale 4
	Manage micromap,scale 4
	Manage micromap,float 1
	Manage map,detail high

	Manage main,statusbar,color White ; # #80FFFF
	Manage message,float 0

	Manage misc,mode,exp 1
	Manage misc,mode,armor_class 1
	Manage misc,textLabels [expr {[winfo screenwidth .] < 800}]
	Manage misc,layout tall
	Manage misc,float 0

	Manage messages,max 256

	Manage settings,showUnused 0

	# Path elements of most-recently-used savefile
	Manage savefile {}

	# Icon configuration
	Manage config,prefix dg32

	Manage choicewindow,show 0
	Manage choicewindow,autoexpand 1
	Manage choicewindow,showicon 0

	Manage inventory,alwaysOnTop 1
	Manage inventory,style new

	Manage store,style new

	Manage message2window,show 0

	Manage font,autobar [Global font,sys,normal]
	Manage font,choice [Global font,sys,normal]
	Manage font,inventory [Global font,sys,normal]
	Manage font,knowledge [Global font,sys,normal]
	Manage font,macros [Global font,fixed,normal]
	Manage font,magic [Global font,sys,normal]
	Manage font,message [Global font,sys,normal]
	Manage font,messages [Global font,sys,normal]
	Manage font,misc [Global font,sys,small]
	Manage font,miscPopup [Global font,sys,normal]
	Manage font,monster [Global font,sys,normal]
	Manage font,recall [Global font,sys,large]
	Manage font,options [Global font,sys,normal]
	Manage font,status [Global font,sys,normal]
	Manage font,statusBar [Global font,sys,normal]
	Manage font,store [Global font,sys,normal]

	### One-time warnings to the user.

	# Show Setup Window first time
	Manage warning,setup 0

#	Manage options,autosave 1
	Manage window,autosave 1

	variable Write
	set Write(savefile) "NSValueManager::Write"
	variable Read
	set Read(savefile) "NSValueManager::Read"
	
	LoadValueFile

	return
}

# NSValueManager::CloseModule --
#
#	Called before the game exits. Dumps all values to the tk/config/value
#	file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::CloseModule {} {

	global Angband
	variable Value
	variable Write

	set tempName [NSUtils::TempFileName [PathTk config]]
	if {[catch {openlf $tempName} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the \"value\" file for writing:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return
	}

	puts $fileId "# Automatically generated. Do not edit.\n"

	foreach name [lsort [array names Value]] {
		if {[info exists Write($name)]} {
			set value [eval $Write($name) $name]
		} else {
			set value [Get $name]
		}
		puts $fileId [list Manage $name $value]
	}

	close $fileId

	set fileName [NSUtils::ReadLink [PathTk config value]]
	if {[file exists $fileName]} {
		file rename -force -- $fileName $fileName.bak
	}
	file rename -- $tempName $fileName

	return
}

# NSValueManager::LoadValueFile --
#
#	Read tk\config\value.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::LoadValueFile {} {

	variable Value
	variable Read

	set fileName [PathTk config value]
	if {[file exists $fileName]} {
		source $fileName
	}

	foreach name [array names Read] {
		if {[info exists Value($name)]} {
			eval $Read($name) $name
		}
	}

	return
}

# NSValueManager::AddClient --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::AddClient {name command} {

	variable Command
	variable ValuePriv

	set clientId [incr ValuePriv(clientId)]
	set Command($name,$clientId) $command
	return $clientId
}

# NSValueManager::RemoveClient --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::RemoveClient {name clientId} {

	variable Command

	unset Command($name,$clientId)

	return
}

# NSValueManager::Manage --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::Manage {name value} {

	variable Value

	set Value($name) $value

	return
}

# NSValueManager::Set --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::Set {name value} {

	variable Value

	set Value($name) $value
	Changed $name

	return
}

# NSValueManager::Get --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::Get {name} {

	variable Value

	return $Value($name)
}

# NSValueManager::Changed --
#
#	Description
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::Changed {name} {

	variable Command

	foreach name [array names Command $name,*] {
		catch {uplevel #0 $Command($name)} err
	}

	return
}

# NSValueManager::Write --
#
#	Special handling of some values.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::Write {name} {

	# Instead of writing "Manage savefile {C:/ AngbandTk lib save SaveFile}"
	# we will write "Manage savefile {PathTk lib save SaveFile}"
	# so the user can move the game directory without trouble, and
	# make UpgradeTool's job easier.

	set value [Get $name]
	if {![string length $value]} {
		return $value
	}

	switch -- $name {
		savefile {
			set path [eval file join $value]
			if {[IsFileInPath $path]} {
				set list [StripCommon $path [PathTk]]
				return [concat PathTk $list]
			}
		}
	}

	return [list $value]
}

# NSValueManager::Read --
#
#	Special handling of some values.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSValueManager::Read {name} {

	set value [Get $name]
	if {![string length $value]} {
		return
	}

	switch -- $name {
		savefile {
			if {[lindex $value 0] == "PathTk"} {
				Manage $name [file split [eval $value]]
			}
		}
	}

	return
}

# ColorFromValue --
#
#	Return the palette RGB color for the value with the given name.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc ColorFromValue {valueName} {

	set index [NSValueManager::Get $valueName]
	return [palette set $index]
}

# Value --
#
#	Convenience interface to NSValueManager::Get/Set
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Value {name args} {

	# Set info
	if {[llength $args]} {
		NSValueManager::Set $name [lindex $args 0]

	# Get info
	} else {
		return [NSValueManager::Get $name]
	}

	return
}

# ListBackgroundChanged --
#
#	Configures the background color of the given canvas to the
#	color of the "listBG" value.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc ListBackgroundChanged {canvas} {

	$canvas configure -background [Value listBG]

	return
}

# default_tval_to_attr --
#
#	Returns the color associated with the given object tval.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc default_tval_to_attr {tval} {

	return [Value $tval]
}

# setg --
#
#	A namespace with commands called when the tk/config/setting file
#	is sourced.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

namespace eval setg {

proc one {keyword value} {

	# Never set cheating options
	if {[string match cheat* $keyword]} return

	# Ignore errors, such as outdated settings
	catch {
		Setting $keyword $value
	}

	return
}

# namespace eval setg
}

# Setting --
#
#	A namespace with commands called when the tk/config/setting file
#	is sourced.
#

namespace eval Config::Setting {

# namespace eval Config::Setting
}

proc Config::Setting::Source {path} {

	source $path

	return
}

proc Config::Setting::One {keyword value} {

	# Never set cheating options
	if {[string match cheat* $keyword]} return

	# Ignore errors, such as outdated settings
	catch {
		Setting $keyword $value
	}

	return
}

# ExpandSetting --
#
#	When a setting is changed in Angband (possibly by the "setting" command,
#	or by reading a preference file) a <Setting> quasi-event is generated.
#	Since some of our settings are unknown to Angband (they exist in Tcl code
#	only, such as show_icons) we use qeinstall for those settings so that
#	scripts can use qebind in a consistent manner to know when any setting
#	changes.
#
#	ExpandSetting is called to perform %-substitution in a qebind script.

proc ExpandSetting {which object event detail} {

	switch -- $which {
		c {
			return [Setting $detail]
		}
		d {
			return $detail
		}
		W {
			return $object
		}
		default {
			return $which
		}
	}
}

# Setting --
#
#	Get or set the value of a setting. This one routine consolidates
#	all the different setting-related commands.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Setting {keyword args} {

	# Set
	if {[llength $args]} {
		set value [lindex $args 0]
		switch -- $keyword {
			default {
				angband setting set $keyword $value
			}
		}

	# Get
	} else {
		switch -- $keyword {
			ambient_delay{
				return [Value $keyword]
			}
			default {
				return [angband setting set $keyword]
			}
		}
	}

	return
}

# SettingDesc --
#
#	Return the human-readable description for a setting.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc SettingDesc {keyword} {

	switch -- $keyword {
		default {
			return [angband setting desc $keyword]
		}
	}
}

set data {}
lappend data \
	"Ancient Dragons" "-d_char D" \
	"Angelic Beings" "-d_char A" \
	"Birds" "-d_char B" \
	"Canines" "-d_char C" \
	"Creeping Coins" "-d_char $" \
	"Demihumans" "-d_char h" \
	"Dragons" "-d_char d" \
	"Elementals" "-d_char E" \
	"Elemental Ball" "-d_char *" \
	"Eyes/Beholders" "-d_char e" \
	"Felines" "-d_char f" \
	"Ghosts" "-d_char G" \
	"Giant Ants" "-d_char a" \
	"Giant Bats" "-d_char b" \
	"Giant Centipedes" "-d_char c" \
	"Giant Dragon Flies" "-d_char F" \
	"Giants" "-d_char P" \
	"Golems" "-d_char g" \
	"Humans" "-d_char p" \
	"Hybrids" "-d_char H" \
	"Hydras" "-d_char M" \
	"Icky Things" "-d_char i" \
	"Jellies" "-d_char j" \
	"Killer Beetles" "-d_char K" \
	"Kobolds" "-d_char k" \
	"Lichs" "-d_char L" \
	"Major Demons" "-d_char U" \
	"Minor Demons" "-d_char u" \
	"Molds" "-d_char m" \
	"Multiplying Insects" "-d_char I" \
	"Mushroom Patches" "-d_char ," \
	"Nagas" "-d_char n" \
	"Ogres" "-d_char O" \
	"Orcs" "-d_char o" \
	"Quadropeds" "-d_char q" \
	"Quylthulgs" "-d_char Q" \
	"Reptiles/Amphibians" "-d_char R" \
	"Rodents" "-d_char r" \
	"Scorpions/Spiders" "-d_char S" \
	"Skeletons" "-d_char s" \
	"Snakes" "-d_char J" \
	"Townspeople" "-d_char t" \
	"Mimics" "-d_char !?=.#|>(+&" \
	"Trolls" "-d_char T" \
	"Vampires" "-d_char V" \
	"Vortices" "-d_char v" \
	"Aquatic Creatures" "-d_char ~" \
	"Wights/Wraiths" "-d_char W" \
	"Worms/Worm Masses" "-d_char w" \
	"Xorns/Xarens" "-d_char X" \
	"Yeeks" "-d_char y" \
	"Yeti" "-d_char Y" \
	"Zephyr Hounds" "-d_char Z" \
	"Zombies/Mummies" "-d_char z" \
	"Uniques" "-unique yes"
Global groups,r_info $data
unset data

proc DumpValueManager {} {

	set win .dumpvalue
	toplevel $win
	text $win.text
	pack $win.text

	foreach name [lsort [array names NSValueManager::Value]] {
		set value [NSValueManager::Get $name]
		$win.text insert end "Manage $name $value\n"
	}

	return
}

