#!/bin/bash -xue

#This script ports most of the syntax of Jamoma objects to this of the ossia-max library
#It can do this recursively on a set of .maxpat files, contained in a folder (and its subdolders) 
#usage: open Terminal, type the following command: sh j2ossia.sh /PathTo/YourFolder/ContainingPatches, press Enter

find "$1" -name '*.maxpat' -exec sed -E -i.bak '
    s/j\.remote/ossia.remote/g;
    s/j\.send/ossia.remote/g;    
    s/j\.receive/ossia.remote/g;    
    s/j\.model/ossia.model \#1/g;
    s/j\.message ([a-zA-Z0-9~().\/]+) /ossia.parameter \1 @access_mode set /g;
    s/j\.return ([a-zA-Z0-9~().\/]+) /ossia.parameter \1 @access_mode get /g;    
    s/j\.parameter/ossia.parameter/g;
    s/@clipmode none/@bounding_mode free/g;
    s/clipmode/bounding_mode/g;
    s/both/clip/g;    
    s/decimal/float/g;
    s/integer/int/g;
    s/boolean/bool/g;
    s/generic/tuple/g;
    s/symbol/string/g;
    s/@type none/@type impulse/g;
    s/repetitions\/filter/repetition_filter/g;    
    s/@dataspace ([a-zA-Z0-9~().\/]+) @unit ([a-zA-Z0-9~().\/]+)/@unit \1.\2/g
    ' {} \;