# File: config.tcl

# Purpose: icon configuration management

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

namespace eval NSConfig {

	variable Priv

# namespace eval NSConfig
}

# NSConfig::InitModule --
#
#	One-time-only-ever initialization.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::InitModule {} {

	# Read tk/config, which contains a list of icon configurations
	# and the current icon configuration.
	ReadConfigFile

	# Set the default set of files to pass to SourceOne. These
	# can be overridden by scripts to use common configuration
	# files. See ShareConfigFile() below.
	SetPrefix [Value config,prefix]

	return
}

# NSConfig::ReadConfigFile --
#
#	Reads the tk/config file, which holds a list of configuration
#	prefixes, along with descriptive text for each prefix. Each
#	prefix can be used to read and write certain icon configuration
#	files.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::ReadConfigFile {} {

	variable Priv

	if {[catch {open [PathTk config config]} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the \"config\" file for reading:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return
	}

	while {![eof $fileId]} {

		# Read a line
		set count [gets $fileId list]
		if {$count == -1} break

		# Save the text, so it can be written out later
		lappend Priv(text) $list

		if {$count == 0} continue

		switch -- [lindex $list 0] {
			Config: {
				lappend Priv(config) [lindex $list 1] [lindex $list 2]
			}
		}
	}

	close $fileId

	return
}

# NSConfig::Load --
#
#	Processes the set of files for the "current" configuration set.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::Load {} {

	# Get the current configuration prefix
	set prefix [Global config,prefix]
	
	angband_load note $prefix

	# Try "prefix.cfg"
	SourceOne $prefix.cfg
	
	# Try prefixNN.cfg
	regsub {(16|24|32)} $prefix NN prefix
	SourceOne $prefix.cfg

	# These next files are automatically generated at shutdown
	# Source [Global config,alternate] Config::Alternate
	# Source [Global config,sprite] Config::Sprite
	# Source [Global config,assign] Config::Assign

	# Custom icon assignments for each character
	Source char-icon Config::CharIcon
#	charicon::Assign
	
	# Map symbols
	angband_load note "Initializing map..."
	SourceOne config-map 1

#Colormapify

	return
}

# NSConfig::InitIcons --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::InitIcons {iconSize} {

	if {[Platform unix]} {
		angband init_icons $iconSize [winfo depth .]
	}
	if {[Platform windows]} {
		set depth [winfo depth .]
		if {$depth != 8} {
			set depth 16
		}
		angband init_icons $iconSize $depth
	}

	return
}

# NSConfig::SetPrefix --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::SetPrefix {prefix} {

	Value config,prefix $prefix

	Global config,prefix $prefix
	Global config,alternate $prefix-alternate
	Global config,sprite $prefix-sprite
	Global config,assign $prefix-assign
	Global config,town $prefix-town
	Global config,postop $prefix-postop

	return
}

# NSConfig::ShareConfigFile --
#
#	Description.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::ShareConfigFile {which file} {

	switch -- $which {
		alternate -
		sprite -
		assign -
		town {
			Global config,$which $file
		}

		default {
			error "unknown config file \"$which\""
		}
	}

	return
}

# NSConfig::SourceOne --
#
#	Looks for the given file in the tk/config directory. If it
#	exists, it is sourced at the global level. This command is
#	usually called from a icon configuration file, type ".cfg".
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::SourceOne {fileName {required 0}} {

	set fileName [file tail $fileName]
	set path [PathTk config $fileName]
	if {[file exists $path]} {
		uplevel #0 source $path
		return
	}
	set path [PathTk config $fileName]
	if {[file exists $path]} {
		uplevel #0 source $path
		return
	}
	if {$required} {
		error "can't find file \"$fileName\""
	}

	return
}

# NSConfig::Source --
#
#	Looks for the given file in the tk/config directory. If it
#	exists, it is sourced in the given namespace. This command is
#	usually called from a icon configuration file, type ".cfg".
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::Source {fileName namespace} {

	set fileName [file tail $fileName]
	set fileName [PathTk config $fileName]
	if {[file exists $fileName]} {
		${namespace}::Source $fileName
	}

	return
}

# NSConfig::FileLibData --
#
#	Takes the "tail" of the given file name, and appends it to the
#	complete pathname of the image directory.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::FileLibData {file} {

	set file [file tail $file]
	return [PathTk image $file]
}

# NSConfig::FindImage --
#
#	Find an image file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::FindImageFile {imageFile} {

	set path [PathTk image $imageFile]
	if {[file exists $path]} {
		return $path
	}
	set path [PathTk image dg [file tail $imageFile]]
	if {[file exists $path]} {
		return $path
	}
	error "icon image file \"$imageFile\" was not found"

	return
}

# NSConfig::CheckIconData --
#
#	Creates an icon data file in the lib/data directory if it does
#	not already exist. If it exists but is older than the given
#	image file, it is overwritten.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::CheckIconData {imageFile iconFile imageSize makeMask} {

	set force 0

	# The image file is required
	set imageFile [FindImageFile $imageFile]

	# The icon file may or may not exist
	set iconFile [FileLibData $iconFile]
	if {!$force && [file exists $iconFile]} {

		set mtime [file mtime $imageFile]
		set mtime2 [file mtime $iconFile]
		if {$mtime < $mtime2} return
	}
	
	# Tell the user why she's waiting
	angband_load prompt "Writing icon file \"[file tail $iconFile]...\""

	# Build a command
	set iconSize [icon size]
	set command "icon makeicon -iconwidth $iconSize -iconheight $iconSize -imagefile $imageFile -datafile $iconFile"
	if {$imageSize != $iconSize} {
		append command " -imagewidth $imageSize -imageheight $imageSize"
	}
	if {$makeMask} {
		append command " -transparent white"
	}
	
	eval $command

	return
}

# NSConfig::CreateIconType --
#
#	Creates a new kind of icon from the given icon-image file
#	and optional mask-image file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

# NOT USED
proc NSConfig::CreateIconType {iconType iconFile maskFile imageSize} {

	# Hack -- Tell user what is happening
	set canvas $::AngbandPriv(load,win).canvas
	$canvas itemconfigure message -text "Initializing icons: $iconType"
	update

	set depth [icon depth]

	# dragon.gif --> dragon.icd
	set iconFile [file tail $iconFile]
	set iconData [file rootname $iconFile].$depth.icd
	
	if {$imageSize != [icon size]} {
		regsub $imageSize $iconData [icon size] iconData
	}

	CheckIconData $iconFile $iconData $imageSize 0
	set iconData [FileLibData $iconData]
	
	# Masked
	if {[string length $maskFile]} {

		# Hack -- Since maskData is formed from iconFile
		if {$imageSize != [icon size]} {
			regsub $imageSize $iconFile [icon size] iconFile
		}

		# dragon_mask.gif --> dragon.msk
		set maskData [file rootname $iconFile].$depth.msk

		set makeMask 0
		if {[regsub :makeMask $maskFile "" maskFile]} {
			set makeMask 1
		}

		CheckIconData $maskFile $maskData $imageSize $makeMask
		set maskData [FileLibData $maskData]

		# Now create the icon type
		if {[catch {
			icon createtype $iconType -file $iconData -maskfile $maskData
		} result]} {

			# Sometimes I get errors from gzclose() when decompressing
			# a file. In these cases I will delete the file just in
			# case it was corrupted.
			set error $::errorInfo
			file delete $iconData $maskData
			error $error
		}

	# Unmasked
	} else {

		# Now create the icon type
		if {[catch {
			icon createtype $iconType -file $iconData
		} result]} {

			# Sometimes I get errors from gzclose() when decompressing
			# a file. In these cases I will delete the file just in
			# case it was corrupted.
			set error $::errorInfo
			file delete $iconData

			# Propagate the error
			error $error
		}
	}

	return
}

proc NSConfig::CreateIconType {iconType iconFile maskFile imageSize} {

	# Hack -- Tell user what is happening
	set canvas $::AngbandPriv(load,win).canvas
	$canvas itemconfigure message -text "Initializing icons: $iconType"
	update
	
	set depth [icon depth]

	# dragon.gif --> dragon.icd
	set iconFile [file tail $iconFile]
	set iconData [file rootname $iconFile].$depth.icd

	if {$imageSize != [icon size]} {
		regsub $imageSize $iconData [icon size] iconData
	}

	CheckIconData $iconFile $iconData $imageSize 0
	set iconData [FileLibData $iconData]
	
	# Now create the icon type
	if {[catch {
		icon createtype $iconType -file $iconData
	} result]} {

		# Sometimes I get errors from gzclose() when decompressing
		# a file. In these cases I will delete the file just in
		# case it was corrupted.
		set error $::errorInfo
		file delete $iconData

		# Propagate the error
		error $error
	}

	return
}

# NSConfig::Maskify --
#
#	After the masks data is loaded, and any mask-assignments are done
#	(with the "icon mask" command), we must fiddle with the icon data
#	so it does not conflict with the mask data.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::Maskify {iconType} {

#	icon maskify $iconType
icon rle $iconType White

	return
}

# NSConfig::NoMoreIcons --
#
#	This is a big silly hack called when all the icon types have
#	been created. It is used just so I can update the progress bar
#	during startup.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc NSConfig::NoMoreIcons {} {

	global AngbandPriv
	
	set canvas $AngbandPriv(load,win).canvas
	$canvas itemconfigure message -text "Assigning icons..."
	update

	return
}

# NSConfig::Colormapify --
#
#	After all the icon types have been created, the masks assigned,
#	and Maskify has been called where needed, then convert from the
#	palette-indexed data to colormap-indexed data.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

#UNUSED
proc NSConfig::Colormapify {} {

	foreach type [icon gettypes] {
		switch -- $type {
			blank -
			default -
			none {
			}
			default {
				if {[icon ascii isascii $type]} continue
				icon colormapify $type
			}
		}
	}

	return
}


# Alternate --
#
#	A namespace with commands called when the tk/config/alternate file
#	is sourced.
#

namespace eval Config::Alternate {

	variable Priv

# namespace eval Config::Alternate
}

proc Config::Alternate::Source {path} {

	source $path

	return
}

proc Config::Alternate::Type {type} {

	variable Priv

	lappend Priv(type) $type

	return
}

proc Config::Alternate::New {reason} {

	variable Priv

	set Priv(id) [alternate create $reason]

	return
}

proc Config::Alternate::Ins {typeIndex iconIndex} {

	variable Priv

	set type [lindex $Priv(type) $typeIndex]
	alternate insert $Priv(id) 1000 -type $type -index $iconIndex

	return
}

# Config::Alternate::Write --
#
#	Writes the tk/config/$prefix-alternate file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Config::Alternate::Write {} {

	set tempName [NSUtils::TempFileName [PathTk config]]
	if {[catch {openlf $tempName} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the \"alternate\" file for writing:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return
	}

	puts $fileId "# Automatically generated. Do not edit.\n"

	set max [alternate count]
	if {$max} {
		set typeList [icon gettypes]
		set index 0
		foreach type $typeList {
			puts $fileId "Type $type"
			set type2index($type) $index
			incr index
		}
		puts $fileId ""
	
		for {set i 0} {$i < $max} {incr i} {
			puts $fileId "New [alternate configure $i -reason]"
			foreach frame [alternate get $i] {
				scan $frame "%s %n" iconType n
				set extra [string range $frame $n end]
				puts $fileId "Ins $type2index($iconType) $extra"
			}
			puts $fileId ""
		}
	}

	close $fileId

	set fileName [NSUtils::ReadLink [PathTk config [Global config,alternate]]]
	if {[file exists $fileName]} {
		file rename -force -- $fileName $fileName.bak
	}
	file rename -- $tempName $fileName

	return
}

# Config::Assign --
#
#	A namespace with commands called when the tk/config/assign file
#	is sourced.
#

namespace eval Config::Assign {

	variable Priv

#namespace eval Config::Assign
}

# Evaluate a script
proc Config::Assign::Source {path} {

	source $path

	return
}

# Add an assign type
proc Config::Assign::Assign {type} {

	variable Priv

	lappend Priv(assignType) $type

	return
}

# Add an icon type
proc Config::Assign::Type {type} {

	variable Priv

	lappend Priv(type) $type

	return
}

# Start assigning to this group
proc Config::Assign::Group {group} {

	variable Priv

	set Priv(group) $group

	return
}

proc Config::Assign::One {member assignTypeIndex args} {

	variable Priv

	set Priv(member) $member

	# Get the current assignment for this thing
	set assign [assign set $Priv(group) $member]
	scan $assign %s assignType
	
	### Hack -- Don't override "flavor" assignments
	switch -- $assignType {
		icon {
			scan $assign "%s %s" assignType iconType

			### Hack -- Don't override "none" assignments
			if {[string equal $iconType none]} return
		}
		flavor {
			### Double-Hack -- Do override "flavor" assignments to insta-artifacts,
			### since they may have custom icons (ex Rings of Power).
			if {[string equal $Priv(group) object]} {
				if {![angband k_info info $member insta_art]} return
			} else {
				return
			}
		}
	}

	# Get the type we are assigning
	set assignType [lindex $Priv(assignType) $assignTypeIndex]

	switch -- $assignType {
		alternate {
			set assign "alternate $args"
		}
		flavor {
			error "flavor assignments should not be saved"
		}
		icon {

			set typeIndex [lindex $args 0]
			set iconType [lindex $Priv(type) $typeIndex]

			### Hack -- Don't allow ascii assignments (they shouldn't be saved)
			if {[icon ascii isascii $iconType]} {
				return
			}

			set assign "icon $iconType [lrange $args 1 end]"
		}
		sprite {
			set assign "sprite $args"
		}
	}

	if {[catch {assign validate $assign}]} {
		set assign "icon default 0"
	}

	assign set $Priv(group) $member $assign

	# If we just assigned an icon to the first character image, then
	# it means the user wants the same icon for every character.
	if {!$member && [string equal $Priv(group) character]} {
		Global autoAssign 0
	}

	return
}

proc Config::Assign::Effect {effect typeIndex iconIndex} {

	variable Priv

	set iconType [lindex $Priv(type) $typeIndex]

	if {[catch {icon validate -type $iconType -index $iconIndex}]} {
		set iconType default
		set iconIndex 0
	}
	effect assign $Priv(group) $effect -type $iconType -index $iconIndex

	return
}

proc Config::Assign::Feat {light background} {

	variable Priv

	feature configure $Priv(member) -light $light -background $background

	return
}

# Config::Assign::Write --
#
#	Writes the tk/config/assign file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Config::Assign::Write {} {

	set tempName [NSUtils::TempFileName [PathTk config]]
	if {[catch {openlf $tempName} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the \"assign\" file for writing:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return
	}
	
	puts $fileId "# Automatically generated. Do not edit.\n"

	# List of assign types
	set index 0
	foreach name [assign types] {
		puts $fileId "Assign $name"
		set ignoreAssign($name) 0
		set assign2index($name) $index
		incr index
	}
	puts $fileId ""

	# Don't save "flavor" assignments since they are different for each
	# character.
	set ignoreAssign(flavor) 1

	# List of icon types
	set index 0
	foreach type [icon gettypes] {
		puts $fileId "Type $type"
		set type2index($type) $index
		incr index
	}
	puts $fileId ""

	# Don't save ascii-type assignments, since the user can't change them.
	foreach iconType [icon gettypes] {
		if {[icon ascii isascii $iconType]} {
			set ignoreType($iconType) 1
		} else {
			set ignoreType($iconType) 0
		}
	}

	# Features
	set max [angband f_info max]
	puts $fileId "Group feature"
	for {set f_idx 1} {$f_idx < $max} {incr f_idx} {

		set assign [assign set feature $f_idx]
		scan $assign "%s %n" assignType n
		if {$ignoreAssign($assignType)} continue
		set assignIndex $assign2index($assignType)

		if {[string equal $assignType icon]} {
			scan $assign "%s %s %n" assignType iconType n
			if {$ignoreType($iconType)} continue
			set typeIndex $type2index($iconType)
			set assignStr "$typeIndex [string range $assign $n end]"
		} else {
			set assignStr [string range $assign $n end]
		}
		puts $fileId "One $f_idx $assignIndex $assignStr"

		set light [feature configure $f_idx -light]
		set background [feature configure $f_idx -background]
		puts $fileId "Feat $light $background"
	}
	puts $fileId ""

	# Monsters
	set max [angband r_info max]
	puts $fileId "Group monster"
	for {set r_idx 1} {$r_idx < $max} {incr r_idx} {

		set assign [assign set monster $r_idx]
		scan $assign "%s %n" assignType n
		if {$ignoreAssign($assignType)} continue
		set assignIndex $assign2index($assignType)

		if {[string equal $assignType icon]} {
			scan $assign "%s %s %n" assignType iconType n
			if {$ignoreType($iconType)} continue
			set typeIndex $type2index($iconType)
			set assignStr "$typeIndex [string range $assign $n end]"
		} else {
			set assignStr [string range $assign $n end]
		}
		puts $fileId "One $r_idx $assignIndex $assignStr"
	}
	puts $fileId ""

	# Objects
	set max [angband k_info max]
	puts $fileId "Group object"
	for {set k_idx 1} {$k_idx < $max} {incr k_idx} {

		set assign [assign set object $k_idx]
		scan $assign "%s %n" assignType n
		if {$ignoreAssign($assignType)} continue
		set assignIndex $assign2index($assignType)

		if {[string equal $assignType icon]} {
			scan $assign "%s %s %n" assignType iconType n
			if {$ignoreType($iconType)} continue
			set typeIndex $type2index($iconType)
			set assignStr "$typeIndex [string range $assign $n end]"
		} else {
			set assignStr [string range $assign $n end]
		}
		puts $fileId "One $k_idx $assignIndex $assignStr"
	}
	puts $fileId ""

	# Effect (ball)
	puts $fileId "Group ball"
	foreach effect [effect names ball] {
		set icon [effect assign ball $effect]
		scan $icon "%s %d" iconType iconIndex
		if {$ignoreType($iconType)} continue
		set typeIndex $type2index($iconType)
		puts $fileId "Effect $effect $typeIndex $iconIndex"
	}
	puts $fileId ""

	# Effect (bolt)
	puts $fileId "Group bolt"
	foreach effect [effect names bolt] {
		set icon [effect assign bolt $effect]
		scan $icon "%s %d" iconType iconIndex
		if {$ignoreType($iconType)} continue
		set typeIndex $type2index($iconType)
		puts $fileId "Effect $effect $typeIndex $iconIndex"
	}
	puts $fileId ""

	# Effect (ammo)
	puts $fileId "Group ammo"
	foreach effect [effect names ammo] {
		set icon [effect assign ammo $effect]
		scan $icon "%s %d" iconType iconIndex
		if {$ignoreType($iconType)} continue
		set typeIndex $type2index($iconType)
		puts $fileId "Effect $effect $typeIndex $iconIndex"
	}
	puts $fileId ""

	close $fileId

	set fileName [NSUtils::ReadLink [PathTk config [Global config,assign]]]
	if {[file exists $fileName]} {
		file rename -force -- $fileName $fileName.bak
	}
	file rename -- $tempName $fileName

	return
}

# Config::CharIcon --
#
#	A namespace with commands called when the tk/config/char-icon file
#	is sourced.

namespace eval Config::CharIcon {

	variable Priv

# namespace eval Config::CharIcon
}

# Evaluate a script
proc Config::CharIcon::Source {path} {

	source $path

	Assign

	return
}

proc Config::CharIcon::Add {name prefix index icon} {

	variable Priv
	
	set Priv(icon,$name,$prefix,$index) $icon

	return
}

# If there is a user-specified icon assignment for the current character
# and configuration, assign it.
proc Config::CharIcon::Assign {} {

	variable Priv

	set name [angband player name]
	set prefix [Global config,prefix]
	set max [assign count character]
	
	for {set i 0} {$i < $max} {incr i} {
		if {[info exists Priv(icon,$name,$prefix,$i)]} {
			set assign $Priv(icon,$name,$prefix,$i)
			scan $assign "%s %n" assignType n
			set extra [string range $assign $n end]
			catch {
				assign set character $i $assign
				if {!$i} {
					Global autoAssign 0
				}
			}
		}
	}
	
	return
}

# When the character icon is automatically assigned, we must clear
# any user-specified icon assignment.
proc Config::CharIcon::AutoAssign {} {

	variable Priv

	set name [angband player name]
	set prefix [Global config,prefix]

	if {[info exists Priv(icon,$name,$prefix,0)]} {
		unset Priv(icon,$name,$prefix,0)
	}

	return
}

proc Config::CharIcon::Debug {} {

	variable Priv

	set name [angband player name]
	set prefix [Global config,prefix]

	set ignore ""
	if {[Global autoAssign]} {
		set ignore [assign set character 0]
	}
	set max [assign count character]
	for {set i 0} {$i < $max} {incr i} {
		set assign [assign set character $i]
		if {[string compare $assign $ignore]} {
			set Priv(icon,$name,$prefix,$i) $assign
		} else {
			if {[info exists Priv(icon,$name,$prefix,$i)]} {
				unset Priv(icon,$name,$prefix,$i)
			}
		}
	}

	::Debug "\nBEGIN Config::CharIcon::Debug"
	set nameList [lsort [array names Priv icon,*]]
	foreach name $nameList {
		regexp "icon,(.*),(.*),(.*)" $name ignore charName prefix index
		set assign $Priv($name)
		::Debug "add \"$charName\" $prefix $index \"$assign\""
	}
	::Debug "\nEND Config::CharIcon::Debug"
	
	return
}

proc Config::CharIcon::Write {} {

	variable Priv

	set tempName [NSUtils::TempFileName [PathTk config]]
	if {[catch {openlf $tempName} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the \"char-icon\" file for writing:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return
	}

	puts $fileId "# Automatically generated. Do not edit.\n"

	set name [angband player name]
	set prefix [Global config,prefix]

	# Hack -- If the extra "shape" assignments are the same as
	# the auto-assigned "normal" character icon, I do not save
	# them.
	set ignoreIcon ""

	if {[Global autoAssign]} {
		set ignoreIcon [assign set character 0]
	}
	set max [assign count character]
	for {set i 0} {$i < $max} {incr i} {
		set icon [assign set character $i]
		if {[string compare $icon $ignoreIcon]} {
			set Priv(icon,$name,$prefix,$i) $icon
		} else {
			if {[info exists Priv(icon,$name,$prefix,$i)]} {
				unset Priv(icon,$name,$prefix,$i)
			}
		}
	}

	set nameList [lsort [array names Priv icon,*]]
	foreach name $nameList {
		regexp "icon,(.*),(.*),(.*)" $name ignore charName prefix index
		set icon $Priv($name)
		puts $fileId "Add \"$charName\" $prefix $index \"$icon\""
	}

	close $fileId

	set fileName [NSUtils::ReadLink [PathTk config char-icon]]
	if {[file exists $fileName]} {
		file rename -force -- $fileName $fileName.bak
	}
	file rename -- $tempName $fileName

	return
}

# Config::Sprite --
#
#	A namespace with commands called when the tk/config/sprite file
#	is sourced.
#

namespace eval Config::Sprite {

	variable Priv

# namespace eval Config::Sprite
}

proc Config::Sprite::Source {path} {

	source $path

	return
}

proc Config::Sprite::Type {type} {

	variable Priv

	lappend Priv(type) $type

	return
}

proc Config::Sprite::New {delay reverse} {

	variable Priv

	set Priv(id) [sprite create]
	sprite configure $Priv(id) -delay $delay -reverse $reverse

	return
}

proc Config::Sprite::Ins {typeIndex iconIndex} {

	variable Priv

	set iconType [lindex $Priv(type) $typeIndex]
	if {[catch {icon validate -type $iconType -index $iconIndex}]} {
		set iconType default
		set iconIndex 0
	}
	sprite insert $Priv(id) 1000 -type $iconType -index $iconIndex

	return
}

# Config::Sprite::Write --
#
#	Writes the tk/config/sprite file.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc Config::Sprite::Write {} {

	set tempName [NSUtils::TempFileName [PathTk config]]
	if {[catch {openlf $tempName} fileId]} {
		set msg "The following error occurred while attempting to open "
		append msg "the \"sprite\" file for writing:\n\n$fileId"
		tk_messageBox -title Oops -message $msg
		return
	}

	puts $fileId "# Automatically generated. Do not edit.\n"

	set max [sprite count]
	if {$max} {
		set typeList [icon gettypes]
		set index 0
		foreach type $typeList {
			puts $fileId "Type $type"
			set type2index($type) $index
			incr index
		}
		puts $fileId ""
	
		for {set i 0} {$i < $max} {incr i} {
			puts $fileId "New [sprite configure $i -delay] \
				[sprite configure $i -reverse]"
			foreach frame [sprite get $i] {
				scan $frame "%s %n" iconType n
				set extra [string range $frame $n end]
				puts $fileId "Ins $type2index($iconType) $extra"
			}
			puts $fileId ""
		}
	}

	close $fileId

	set fileName [NSUtils::ReadLink [PathTk config [Global config,sprite]]]
	if {[file exists $fileName]} {
		file rename -force -- $fileName $fileName.bak
	}
	file rename -- $tempName $fileName

	return
}

