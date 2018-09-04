#!/bin/sh

######################################################################
#
# Build executable utility: util_shared using module compile standard
#
######################################################################

LMOD_EXACT_MATCH=no
module load prod_util
machine=$(getsystem.pl -t)

if [ "$machine" = "IBM" ] || [ "$machine" = "Cray" ] || [ "$machine" = "Dell" ]; then
   echo " "
   echo " You are on WCOSS:  $(getsystem.pl -p)"
else
   echo " "
   echo " Your machine is $machine is not recognized as a WCOSS machine."
   echo " The script $0 can not continue.  Aborting!"
   echo " "
   exit
fi
echo " "

machine_lc=${machine,,} # Get lower case

# Load required modules
module use ../modulefiles
module load build_util_shared/$machine_lc
module list

set -x
mkdir -p ../../exec
make
mv faxmakrx ../../exec
make clean