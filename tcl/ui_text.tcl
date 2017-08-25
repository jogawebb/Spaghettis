
# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Copyright (c) 1997-2017 Miller Puckette and others ( https://opensource.org/licenses/BSD-3-Clause ).

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Copyright (c) 2002-2012 krzYszcz and others.

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Text for [qlist] and [textfile] objects.

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

package provide ui_text 1.0

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

namespace eval ::ui_text:: {

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc show {top} {

    _create $top
}

proc release {top} {

    ::cancel $top
}

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc append {top contents} {

    $top.text insert end $contents
}

proc clear {top} {

    $top.text delete 1.0 end
}

proc dirty {top flag} {

    $top.text edit modified $flag
}

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc _create {top} {
    
    toplevel $top -class PdText
    wm title $top [_ "Text"]
    wm group $top .
    
    wm minsize  $top {*}[::styleMinimumSize]
    wm geometry $top [format "=600x400%s" [::rightNextTo $::var(windowFocused)]]
    
    text $top.text  -font [::styleFontText] \
                    -borderwidth 0 \
                    -highlightthickness 0
    
    pack $top.text  -side left -fill both -expand 1
    
    bind $top.text  <<SelectAll>>       "::ui_text::_selectAll $top"
    bind $top.text  <<Modified>>        "::ui_text::_modified $top"
    bind $top.text  <<Save>>            "::ui_text::_save $top"
    
    wm protocol $top WM_DELETE_WINDOW   "::ui_text::closed $top"
        
    focus $top.text
}

proc closed {top} {

    if {[$top.text edit modified]} { 
        ::ui_confirm::checkClose $top { ::ui_text::_save $top } {} { return -level 2 }
    }

    ::ui_interface::pdsend "$top close"
}

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc _format {x} {
    
    return [string map {"," " \\, " ";" " \\; " "$" "\\$" "{" "" "}" "" "\\" ""} $x]
}

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc _selectAll {top} {

    $top.text tag add sel 1.0 end
}

proc _modified {top} {

    if {[tk windowingsystem] eq "aqua"} { wm attributes $top -modified [$top.text edit modified] }
}

proc _save {top} {

    ::ui_interface::pdsend "$top clear"
        
    for {set i 1} {[$top.text compare $i.end < end]} {incr i 1} {
        set line [$top.text get $i.0 $i.end]
        if {$line != ""} {
            ::ui_interface::pdsend [concat $top _addline [::ui_text::_format $line]]
        }
    }
    
    if {[$top.text edit modified]} { ::ui_interface::pdsend "$top _modified" }
    
    ::ui_text::dirty $top 0
}

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

}

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------
