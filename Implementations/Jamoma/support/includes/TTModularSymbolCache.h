/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief TTBlue Class for caching common symbols for speed
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright Copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_MODULAR_SYMBOL_CACHE_H__
#define __TT_MODULAR_SYMBOL_CACHE_H__

#include "TTSymbol.h"

// object classe name
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Application;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_ApplicationManager;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Container;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Cue;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_CueManager;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Data;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Explorer;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Input;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_InputAudio;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Mapper;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_MapperManager;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Mirror;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_OpmlHandler;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Output;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_OutputAudio;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Preset;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_PresetManager;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Ramp;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Receiver;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Sender;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Script;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Subscriber;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_TextHandler;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Viewer;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_XmlHandler;

// notifications
extern TTMODULAR_EXPORT TTSymbol	kTTSym_SchedulerDurationChanged;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_SchedulerOffsetChanged;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_SchedulerSpeedChanged;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_SchedulerRunningChanged;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_SchedulerTicked;

// attribute, message or any word often used
extern TTMODULAR_EXPORT TTSymbol	kTTSym_active;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_activity;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_activityIn;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_activityOut;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_address;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_addresses;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_alias;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_alphabetic;
extern TTMODULAR_EXPORT TTSymbol    kTTSym_attributes;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_array;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_boolean;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_brothers;

extern TTMODULAR_EXPORT TTSymbol    kTTSym_children;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_command;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Command;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_comment;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_content;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_ConvertToAppName;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_ConvertToTTName;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_created;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_dash;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_dataspace;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_dataspaceUnit;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_debug;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_decimal;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_descendants;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_description;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_destroyed;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_directory;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Dump;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_end;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_external;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_Fill;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_filter;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_flag;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Flatten;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_flattened;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_freeze;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_generic;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Get;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_global;
extern TTMODULAR_EXPORT TTSymbol    kTTSym_Go;
extern TTMODULAR_EXPORT TTSymbol    kTTSym_Grab;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_highlight;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_Init;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_initialized;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_inputUnit;
extern TTMODULAR_EXPORT TTSymbol    kTTSym_instances;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_integer;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_life;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_lines;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_local;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_lookfor;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_Map;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_message;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_mix;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_mirror;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_model;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_mute;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_names;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_namespace;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_nodeAddress;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_object;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_objectCache;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_offset;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_parameter;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_preview;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_proxy;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_rangeBounds;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_rangeClipmode;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_rampDrive;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_rampFunction;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_RampGo;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_RampSet;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_RampSlide;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_rampStatus;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_RampTarget;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Read;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_ReadAgain;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_readonly;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Recall;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_receiver;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_repetitionsFilter;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_return;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_returnedValue;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Run;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_Send;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_script;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_service;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_sharp;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_signal;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_start;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_stop;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Stop;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Subscribe;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_tag;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_target;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Tick;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_unit;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_Update;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_value;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_valueDefault;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_valueStepsize;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_viewFreeze;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_view;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_Write;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_WriteAgain;

extern TTMODULAR_EXPORT TTSymbol	kTTSym_xmlHandlerReadingComment;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_xmlHandlerReadingEnds;
extern TTMODULAR_EXPORT TTSymbol	kTTSym_xmlHandlerReadingStarts;


#endif // __TT_MODULAR_SYMBOL_CACHE_H__

