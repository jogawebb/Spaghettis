#! /usr/bin/env bash

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Spaghettis automagically built.
#
# Tested on:
#
#   - Debian / Jessie (i386) / Tk 8.6
#

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

[ "${BASH_VERSION%.*}" \> "3.1" ] || { echo >&2 "${0##*/}: Bash 3.1 or higher only"; exit 1; }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Script must be executed at the same level.

rep=$(pwd)

[ "${0%/*}" = "." ] || { echo >&2 "${0##*/}: Must be executed at the same level"; exit 1; }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Test depedencies ( http://stackoverflow.com/a/22592801 ).

isAlsa=$(dpkg-query -W -f='${Status}' libasound2 2>/dev/null | grep -c "ok installed")
isTcl_5=$(dpkg-query -W -f='${Status}' tcl8.5 2>/dev/null | grep -c "ok installed")
isTk_5=$(dpkg-query -W -f='${Status}' tk8.5 2>/dev/null | grep -c "ok installed")
isTcl_6=$(dpkg-query -W -f='${Status}' tcl8.6 2>/dev/null | grep -c "ok installed")
isTk_6=$(dpkg-query -W -f='${Status}' tk8.6 2>/dev/null | grep -c "ok installed")
isJack=$(dpkg-query -W -f='${Status}' libjack-dev 2>/dev/null | grep -c "ok installed")

[ ${isTk_5} -eq 1 ]  || [ ${isTk_6} -eq 1 ]  || { echo >&2 "${0##*/}: tk8.* package required";  exit 1; }
[ ${isTcl_5} -eq 1 ] || [ ${isTcl_6} -eq 1 ] || { echo >&2 "${0##*/}: tcl8.* package required"; exit 1; }
[ ${isAlsa} -eq 1 ]  || { echo >&2 "${0##*/}: libasound2 package required"; exit 1; }
[ ${isJack} -eq 1 ]  || { echo >&2 "${0##*/}: libjack-dev package required"; exit 1; }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Paths.

destination="${rep}/build"
bin="${rep}/bin"
tcl="${rep}/tcl"
help="${rep}/resources/help"
patches="${rep}/resources/patches"

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Do not overwrite previous build.

[ -e "${destination}" ] && { echo >&2 "${0##*/}: ${destination} already exist"; exit 1; }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Default compiler architecture ( https://stackoverflow.com/questions/246007 ).

foo=$(mktemp build.XXXXXXXXXXXXXXXX)
echo 'int main() { return 0; }' | cc -x c - -o ${foo}
test=$(file ${foo})
rm ${foo}

# Externals suffix.

extension=".pdobject32"

[[ "${test}" =~ "64-bit" ]] && { echo "Build 64-bit ..."; extension=".pdobject64"; }

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Build the binaries (for now with JACK API only).

cd "${rep}/src"                                                 || exit 1
echo "Build MIDI with ALSA ... " 
echo "Build Audio with JACK ... "
make -f makefile.gnu                                            || exit 1
cd "${rep}"                                                     || exit 1

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Build the hello examples. 

cd "${rep}/resources/examples"                                  || exit 1
make -f makefile.gnu "EXTENSION=${extension}"                   || exit 1
cd "${rep}"                                                     || exit 1

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Create the directory.

echo "Create directory ..."
mkdir "${destination}"                                          || exit 1
cp -R "${bin}" "${destination}"                                 || exit 1
cp -R "${tcl}" "${destination}"                                 || exit 1
cp -R "${help}" "${destination}"                                || exit 1

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Install materials.

echo "Install patches ..."
cp -R "${patches}" "${destination}"                             || exit 1

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Clean the build.

echo "Clean ..."
cd "${rep}/src"                                                 || exit 1
make -f makefile.gnu clean                                      || exit 1
cd "${rep}"                                                     || exit 1

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# End.

echo "SUCCEEDED"

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------
