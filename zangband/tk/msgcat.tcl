# File: msgcat.tcl

# Purpose: internationalization support for AngbandTk

#
# Copyright (c) 1997-2001 Tim Baker
#
# This software may be copied and distributed for educational, research, and
# not for profit purposes provided that this copyright and statement are
# included in all such copies.
#

# Load the "msgcat" package
package require msgcat

# Set to 1 for Japanese translations etc
set JAPANESE 0

if {!$JAPANESE} {

	# English
	::msgcat::mclocale en

} else {

	# Japanese
	::msgcat::mclocale ja
}

# MsgCatInit --
#
#	Sets up msgcat support for a module.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc MsgCatInit {args} {

	global Angband

	# Import all the msgcat commands into the calling namespace, so we
	# can write "mc Foo" instead of "::msgcat::mc Foo".
	uplevel {namespace import ::msgcat::*}

	# XXX Hack -- Always include the "common" translations
	uplevel ::msgcat::mcload [list [file join $::Angband(dirTk,msgs) common]]

	# Load the message file from each given directory
	foreach dir $args {
		uplevel ::msgcat::mcload [list [file join $::Angband(dirTk,msgs) $dir]]
	}
	
	return
}

# msgcat::mcunknown --
#
#	Report an error if no translation exists for the given string.
#	Return the given string just for fun.
#
# Arguments:
#	arg1					about arg1
#
# Results:
#	What happened.

proc msgcat::mcunknown {locale src} {

	if {[winfo exists [focus]]} {
		set parent [winfo toplevel [focus]]
	} else {
		set parent .
	}
	tk_messageBox -title "Msgcat Error" -parent $parent \
		-message "\"$src\"\nTranslation not found. Update your msg file."

    return $src
}

# Internationalization
MsgCatInit startup
