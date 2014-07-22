/* 
 * TTBlue Class for caching common symbols for speed
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_SYMBOL_CACHE_H__
#define __TT_SYMBOL_CACHE_H__

#include "TTSymbol.h"


extern TTFOUNDATION_EXPORT TTSymbolTable	gTTSymbolTable;

// osc namespace
extern TTFOUNDATION_EXPORT TTSymbol	kTTSymEmpty;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSymSlash;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSymDot;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSymColon;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSymStar;

// vectors
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_setVector32;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_getVector32;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_setVector64;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_getVector64;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_alloc;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_attribute;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_baton;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_both;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_bypass;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_clear;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_clipmode;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_clip;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_cliplow;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_cliphigh;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_exclude;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_wrap;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_fold;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_function;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_high;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_include;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_instance;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_low;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_maxNumChannels;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_mode;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_name;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_none;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_notification;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_notify;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_numChannels;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_object;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_parent;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_part;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_priority;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_processInPlace;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_range;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_resonance;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_restrict;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_updateMaxNumChannels;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_updateSampleRate;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_value;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_vectorSize;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_sampleRate;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_schema;

// networking
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_tcp;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_udp;

// waveforms
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_sine;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_sineMod;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_cosine;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_cosineMod;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_square;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_squareMod;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_triangle;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_triangleMod;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ramp;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_rampMod;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_sawtooth;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_sawtoothMod;

// object names
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_adsr;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_audiosignal;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_audiosignalarray;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_dcblock;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_gain;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_operator;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_phasor;

// matrix related
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_matrix;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_matrixarray;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_dimensions;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_type;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_elementCount;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_samplematrix;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_buffer;

// data types - 3 defined above: none, object, value
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_float32;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_float64;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_int8;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_uint8;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_int16;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_uint16;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_int32;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_uint32;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_int64;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_uint64;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_boolean;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_symbol;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_pointer;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_string;

#if 0
#pragma mark -
#pragma mark Modular symbols
#endif

// object classe name
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Application;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ApplicationManager;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Container;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Cue;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_CueManager;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Data;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Explorer;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Input;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_InputAudio;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Mapper;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_MapperManager;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Mirror;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_NodeInfo;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_OpmlHandler;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Output;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_OutputAudio;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Preset;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_PresetManager;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Ramp;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Receiver;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Sender;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Script;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Subscriber;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_TextHandler;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Viewer;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_XmlHandler;

// notifications
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_SchedulerDurationChanged;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_SchedulerOffsetChanged;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_SchedulerSpeedChanged;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_SchedulerRunningChanged;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_SchedulerTicked;

// attribute, message or any word often used
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_active;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_activity;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_activityIn;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_activityOut;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_address;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_addresses;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_alias;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_alphabetic;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_attributes;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_array;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_brothers;

extern TTFOUNDATION_EXPORT TTSymbol kTTSym_children;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_command;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Command;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_comment;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_content;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ConvertToAppName;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ConvertToTTName;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_created;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_dash;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_dataspace;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_dataspaceUnit;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_debug;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_decimal;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_descendants;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_description;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_destroyed;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_directory;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Dump;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_end;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_external;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Fill;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_filter;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_flag;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Flatten;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_flattened;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_freeze;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_generic;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Get;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_global;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_Go;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_Grab;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_highlight;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Init;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_initialized;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_inputUnit;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_instances;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_integer;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_life;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_lines;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_local;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_lookfor;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Map;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_message;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_mix;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_mirror;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_model;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_mute;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_names;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_namespace;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_nodeAddress;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_objectCache;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_offset;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_parameter;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_preview;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_proxy;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_rangeBounds;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_rangeClipmode;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_rampDrive;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_rampFunction;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_RampGo;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_RampSet;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_RampSlide;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_rampStatus;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_RampTarget;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Read;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ReadAgain;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_readonly;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Recall;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_receiver;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_repetitionsFilter;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_return;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_returnedValue;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Run;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Send;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_script;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_service;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_sharp;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_signal;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_start;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_stop;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Stop;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Subscribe;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_tag;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_target;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Tick;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_unit;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Update;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_valueDefault;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_valueStepsize;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_viewFreeze;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_view;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Write;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_WriteAgain;

extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_xmlHandlerReadingComment;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_xmlHandlerReadingEnds;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_xmlHandlerReadingStarts;

// Symbols used for the Score project
// théo : they used to be stored inside a dedicated symbol cache but it is not possible to do that anymore.
// TODO : sort them with the other symbol above

// object classe names
extern TTFOUNDATION_EXPORT TTSymbol  kTTSym_TimeEvent;

// notifications
extern TTFOUNDATION_EXPORT TTSymbol  kTTSym_ConditionReadyChanged;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_EventDateChanged;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_EventStatusChanged;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ProcessStarted;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ProcessEnded;

// attribute, message or any names
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_case;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_color;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Compile;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_compiled;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_condition;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_date;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Dispose;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_duration;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_durationMin;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_durationMax;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_endDate;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_event;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_eventDisposed;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_eventHappened;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_eventPending;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_eventWaiting;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_expression;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Goto;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Happen;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Limit;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Move;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Pause;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Process;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ProcessStart;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_ProcessEnd;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_ready;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_redundancy;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Resume;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_recorded;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_rigid;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_sampled;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_samples;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_speed;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_startDate;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_status;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_Trigger;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_verticalPosition;
extern TTFOUNDATION_EXPORT TTSymbol	kTTSym_verticalSize;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_viewPosition;
extern TTFOUNDATION_EXPORT TTSymbol kTTSym_viewZoom;


#endif // __TT_SYMBOL_CACHE_H__

