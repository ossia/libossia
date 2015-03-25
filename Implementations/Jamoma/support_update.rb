#!/usr/bin/env ruby -wKU
# encoding: utf-8
puts "update support folder"

JamomaCoreDir = "/Users/WALL-E/Documents/Jamoma/Core/"

# Foundation extensions, includes and lib
`cp "#{JamomaCoreDir}"/Foundation/extensions/DataspaceLib/build/DataspaceLib.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/Foundation/extensions/NetworkLib/build/NetworkLib.ttdylib ./support/extensions`

`cp "#{JamomaCoreDir}"/Foundation/library/includes/* ./support/includes`

`cp "#{JamomaCoreDir}"/Foundation/library/build/JamomaFoundation.dylib ./support/lib`

# DSP extensions, includes and lib
`cp "#{JamomaCoreDir}"/DSP/extensions/FunctionLib/build/FunctionLib.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/DSP/extensions/AnalysisLib/build/AnalysisLib.ttdylib ./support/extensions`

`cp "#{JamomaCoreDir}"/DSP/library/includes/* ./support/includes`

`cp "#{JamomaCoreDir}"/DSP/library/build/JamomaDSP.dylib ./support/lib`

# Modular extensions, includes and lib
`cp "#{JamomaCoreDir}"/Modular/extensions/MIDI/build/MIDI.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/Modular/extensions/Minuit/build/Minuit.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/Modular/extensions/OSC/build/OSC.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/Modular/extensions/System/build/System.ttdylib ./support/extensions`

`cp "#{JamomaCoreDir}"/Modular/library/includes/*.h ./support/includes`

`cp "#{JamomaCoreDir}"/Modular/library/build/JamomaModular.dylib ./support/lib`

# Score extensions, includes and lib
`cp "#{JamomaCoreDir}"/Score/extensions/Automation/build/Automation.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/Score/extensions/Interval/build/Interval.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/Score/extensions/Loop/build/Loop.ttdylib ./support/extensions`
`cp "#{JamomaCoreDir}"/Score/extensions/Scenario/build/Scenario.ttdylib ./support/extensions`

`cp "#{JamomaCoreDir}"/Score/library/includes/*.h ./support/includes`

`cp "#{JamomaCoreDir}"/Score/library/build/JamomaScore.dylib ./support/lib`

puts "done"
puts