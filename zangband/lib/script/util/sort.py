#####################################################################
#
# util/sort.py
#
# Helper functions for sorting
#
#####################################################################


#####################################################################
#
# Helper function (for "sort_by_number")
#
#####################################################################
def _sort_by_number(class1, class2):
	if class1.number < class2.number:
		return -1
	elif class1.number > class2.number:
		return 1
	else:
		return 0


#####################################################################
#
# Sort by number
#
#####################################################################
def sort_by_number(list):
	list.sort(_sort_by_number)

