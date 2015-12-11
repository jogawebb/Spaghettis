
# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Copyright (c) 1997-2015 Miller Puckette and others ( https://opensource.org/licenses/BSD-3-Clause ).

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# PureData styles for GNU/Linux.

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# ( http://wiki.tcl-lang.org/14796 ).
# ( http://wiki.tcl-lang.org/37973 ).
# ( http://www.tkdocs.com/tutorial/styles.html ).

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

ttk::style configure PureData.TFrame
ttk::style configure PureData.TLabelframe
ttk::style configure PureData.TEntry
ttk::style configure PureData.TMenubutton
ttk::style configure PureData.TLabel 		-padding 3
ttk::style configure PureData.TCheckbutton  -padding 3
ttk::style configure PureData.TRadiobutton

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

ttk::style map PureData.TEntry -foreground [list disabled DarkGrey]

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

array set width {
    tiny    3
    small   6
    medium  12
    large   18
}

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc styleFontText {}           { return TkTextFont }
proc styleFontConsole {}        { return TkTextFont }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc styleFrame {}              { return "-style PureData.TFrame        -padding 10" }
proc styleLabelFrame {}         { return "-style PureData.TLabelframe   -padding 10" }
proc styleEntry {}              { return "-style PureData.TEntry" }
proc styleEntryNumber {}        { return "-style PureData.TEntry        -justify right"}
proc styleMenuButton {}         { return "-style PureData.TMenubutton"  }
proc styleLabel {}              { return "-style PureData.TLabel"       }
proc styleCheckButton {}        { return "-style PureData.TCheckbutton" }
proc styleRadioButton {}        { return "-style PureData.TRadiobutton" }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc packMain {}                { return "-side top -fill both -expand 1" }
proc packCategory {}            { return "-side top -fill both -expand 1" }
proc packCategoryNext {}        { return "-side top -fill both -expand 1 -pady {5 0}" }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

proc styleMinimumSize {}        { return "300 150" }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------
