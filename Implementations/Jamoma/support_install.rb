#!/usr/bin/env ruby -wKU
# encoding: utf-8

puts "Clear usr/local/jamoma folder"

# Clean /usr/local/jamoma folder
`rm /usr/local/jamoma/extensions/*.*`
`rm /usr/local/jamoma/includes/*.*`
`rm /usr/local/jamoma/lib/*.*`

puts "Clear usr/local/lib aliases"
`rm /usr/local/lib/*.ttdylib`
`rm /usr/local/lib/Jamoma*.dylib`

# Prepare and copy directories
FileUtils.mkdir_p("/usr/local/jamoma/extensions") unless File.exist?("/usr/local/jamoma/extensions")
FileUtils.mkdir_p("/usr/local/jamoma/includes") unless File.exist?("/usr/local/jamoma/includes")
FileUtils.mkdir_p("/usr/local/jamoma/lib") unless File.exist?("/usr/local/jamoma/lib")

puts "Copy jamoma includes, libraries and extensions into usr/local/jamoma folder"

`cp ./support/extensions/* /usr/local/jamoma/extensions`
`cp ./support/includes/* /usr/local/jamoma/includes`
`cp ./support/lib/* /usr/local/jamoma/lib`

puts "Create alias into usr/local/lib folder"

# Create aliases
`ln -s /usr/local/jamoma/lib/JamomaFoundation.dylib /usr/local/lib/JamomaFoundation.dylib`
`ln -s /usr/local/jamoma/lib/JamomaDSP.dylib /usr/local/lib/JamomaDSP.dylib`
`ln -s /usr/local/jamoma/lib/JamomaModular.dylib /usr/local/lib/JamomaModular.dylib`

`ln -s /usr/local/jamoma/extensions/AnalysisLib.ttdylib /usr/local/lib/AnalysisLib.ttdylib`
`ln -s /usr/local/jamoma/extensions/FunctionLib.ttdylib /usr/local/lib/FunctionLib.ttdylib`
`ln -s /usr/local/jamoma/extensions/NetworkLib.ttdylib /usr/local/lib/NetworkLib.ttdylib`
`ln -s /usr/local/jamoma/extensions/System.ttdylib /usr/local/lib/System.ttdylib`
`ln -s /usr/local/jamoma/extensions/DataspaceLib.ttdylib /usr/local/lib/DataspaceLib.ttdylib`
`ln -s /usr/local/jamoma/extensions/MIDI.ttdylib /usr/local/lib/MIDI.ttdylib`
`ln -s /usr/local/jamoma/extensions/Minuit.ttdylib /usr/local/lib/Minuit.ttdylib`
`ln -s /usr/local/jamoma/extensions/OSC.ttdylib /usr/local/lib/OSC.ttdylib`

`ln -s /usr/local/jamoma/lib/JamomaScore.dylib /usr/local/lib/JamomaScore.dylib`
`ln -s /usr/local/jamoma/extensions/Automation.ttdylib /usr/local/lib/Automation.ttdylib`
`ln -s /usr/local/jamoma/extensions/Interval.ttdylib /usr/local/lib/Interval.ttdylib`
`ln -s /usr/local/jamoma/extensions/Loop.ttdylib /usr/local/lib/Loop.ttdylib`
`ln -s /usr/local/jamoma/extensions/Scenario.ttdylib /usr/local/lib/Scenario.ttdylib`

# Clean includes .orig files
`rm /usr/local/jamoma/includes/*.orig`

puts "done"
puts

