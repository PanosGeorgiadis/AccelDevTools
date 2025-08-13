#pragma once

#include <stdint.h>
#include "CeosProtocol/Constants.h"

namespace Server {
namespace CeosProtocol {
namespace Control {

const uint32_t MINORNUMBER = 2;
const uint32_t BACKWARDNUMBER = 0;
const uint32_t CLIENTTYPE = 6; // Product version

const uint32_t OBJECT_TYPE_OFFSET                            = CeosProtocol::LOCALCODES + CeosProtocol::RESULTCODES + CeosProtocol::DATATYPES + CeosProtocol::MESSAGETYPES;

const uint32_t OBJECT_TYPE_STATUS_CHANGED                    = 106 + OBJECT_TYPE_OFFSET; // 2001206    
const uint32_t OBJECT_TYPE_RE_CHANGED                        = 203 + OBJECT_TYPE_OFFSET; // 2001303
const uint32_t OBJECT_TYPE_SELECTED_SETTING_RESET            = 204 + OBJECT_TYPE_OFFSET; // 2001304
const uint32_t OBJECT_TYPE_SUPPLY_STATUS_CHANGED            = 207 + OBJECT_TYPE_OFFSET; // 2001307
const uint32_t OBJECT_TYPE_SELECTED_SETTING_RESTORED        = 208 + OBJECT_TYPE_OFFSET; // 2001308
const uint32_t OBJECT_TYPE_SELECTED_SETTING_CHANGED            = 209 + OBJECT_TYPE_OFFSET; // 2001309
const uint32_t OBJECT_TYPE_SETTING_PROPERTIES_ARRAY_CHANGED = 210 + OBJECT_TYPE_OFFSET; // 2001310

const uint32_t COMMAND_TYPE_OFFSET                            = OBJECT_TYPE_OFFSET + CeosProtocol::OBJECTTYPES;

const uint32_t COMMAND_GET_STATUS                        = 301 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_GET_DESCRIPTIVE_NAME                = 302 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_GET_MODEL                        = 303 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_IS_TECHNIQUE_SUPPORTED            = 310 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_SET_ENERGY                        = 311 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_GET_ENERGY                        = 312 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_GET_LIMITS                        = 313 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_NULL_TECHNIQUE                    = 314 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_GET_SPECTRUM_CALIBRATION            = 315 + COMMAND_TYPE_OFFSET;
const uint32_t COMMAND_SET_RE                            = 407 + COMMAND_TYPE_OFFSET; // -> 2011507
const uint32_t COMMAND_GET_RE                            = 408 + COMMAND_TYPE_OFFSET; // -> 2011508
const uint32_t COMMAND_LOAD_SDB_FILE                    = 409 + COMMAND_TYPE_OFFSET; // -> 2011509
const uint32_t COMMAND_SAVE_SDB_FILE                    = 410 + COMMAND_TYPE_OFFSET; // -> 2011510
const uint32_t COMMAND_RESET_SELECTED_SETTING            = 411 + COMMAND_TYPE_OFFSET; // -> 2011511
const uint32_t COMMAND_LOAD_SDB_STRING                    = 412 + COMMAND_TYPE_OFFSET; // -> 2011512
const uint32_t COMMAND_GET_SDB_STRING                    = 413 + COMMAND_TYPE_OFFSET; // -> 2011513
const uint32_t COMMAND_GET_RE_ARRAY                        = 416 + COMMAND_TYPE_OFFSET; // -> 2011516
const uint32_t COMMAND_NULL_SUPPLY                        = 417 + COMMAND_TYPE_OFFSET; // -> 2011517
const uint32_t COMMAND_GET_DAC_ARRAY                    = 418 + COMMAND_TYPE_OFFSET; // -> 2011518
const uint32_t COMMAND_SET_AT_VALUE                        = 419 + COMMAND_TYPE_OFFSET; // -> 2011519
const uint32_t COMMAND_GET_AT_ARRAY                        = 420 + COMMAND_TYPE_OFFSET; // -> 2011520
const uint32_t COMMAND_APPLY_CALIBRATION_VALUES            = 421 + COMMAND_TYPE_OFFSET; // -> 2011521
const uint32_t COMMAND_ABORT_PROCEDURE                    = 426 + COMMAND_TYPE_OFFSET; // -> 2011526
const uint32_t COMMAND_START_RE_WOBBLE_PROCEDURE        = 427 + COMMAND_TYPE_OFFSET; // -> 2011527
const uint32_t COMMAND_APPLY_AAT                        = 428 + COMMAND_TYPE_OFFSET; // -> 2011528
const uint32_t COMMAND_GET_AAT_ARRAY                    = 429 + COMMAND_TYPE_OFFSET; // -> 2011529
const uint32_t COMMAND_GET_RE_MAX                        = 430 + COMMAND_TYPE_OFFSET; // -> 2011530
const uint32_t COMMAND_GET_RE_MIN                        = 431 + COMMAND_TYPE_OFFSET; // -> 2011531
const uint32_t COMMAND_GET_RE_RESOLUTION                = 432 + COMMAND_TYPE_OFFSET; // -> 2011532
const uint32_t COMMAND_CONNECT_SUPPLY                    = 433 + COMMAND_TYPE_OFFSET; // -> 2011533
const uint32_t COMMAND_DISCONNECT_SUPPLY                = 434 + COMMAND_TYPE_OFFSET; // -> 2011534
const uint32_t COMMAND_GET_SUPPLY_STATUS                = 435 + COMMAND_TYPE_OFFSET; // -> 2011535

const uint32_t COMMAND_GET_SELECTED_PROPERTY            = 436 + COMMAND_TYPE_OFFSET; // -> 2011536
const uint32_t COMMAND_SET_SELECTED_PROPERTY            = 437 + COMMAND_TYPE_OFFSET; // -> 2011537
const uint32_t COMMAND_GET_PROPERTY_VALUE_ARRAY            = 438 + COMMAND_TYPE_OFFSET; // -> 2011538
const uint32_t COMMAND_STORE_SELECTED_SETTING            = 439 + COMMAND_TYPE_OFFSET; // -> 2011539
const uint32_t COMMAND_RESTORE_SELECTED_SETTING            = 440 + COMMAND_TYPE_OFFSET; // -> 2011540
const uint32_t COMMAND_GET_SETTING_ARRAY = 441 + COMMAND_TYPE_OFFSET; // -> 2011541
const uint32_t COMMAND_GET_SETTING_PROPERTIES_ARRAY = 442 + COMMAND_TYPE_OFFSET; // -> 2011542
const uint32_t COMMAND_SET_SELECTED_SETTING = 443 + COMMAND_TYPE_OFFSET; // -> 2011543
const uint32_t COMMAND_GET_SELECTED_SETTING = 444 + COMMAND_TYPE_OFFSET; // -> 2011544

// Filter status
const uint32_t STATUS_UNDEFINED        = 0;
const uint32_t STATUS_STARTED        = 1;
const uint32_t STATUS_STOPPED        = 2;

// Filter mode
const uint32_t CEFID_FILTER_MODE_UNDEFINED    = 0;
const uint32_t CEFID_FILTER_MODE_ESI        = 1;
const uint32_t CEFID_FILTER_MODE_EELS        = 2;

// Filter model
const uint32_t MODEL_UNDEFINED                = 0;
const uint32_t MODEL_CEOS_PROTOTYPE_VERSION = 1;
const uint32_t MODEL_CEOS_PRODUCT_VERSION    = 2;

// Supply Status
const uint32_t SUPPLY_DISCONNECTED_SIMULATION_USED    = 0;
const uint32_t SUPPLY_CONNECTED                        = 1;
/*
// REs
        
// Pre-slit
const int32            RE_ID_MP1_1x = 0;
const int32            RE_ID_MP1_1y = 1;
const int32            RE_ID_MP1_2x = 2;
const int32            RE_ID_MP1_2y = 3;
const int32            RE_ID_MP1_3x = 4;
const int32            RE_ID_MP1_3y = 5;
const int32            RE_ID_MP1_4y = 6;
const int32            RE_ID_MP1_4x = 7;
const int32            RE_ID_MP1_5y = 8;
const int32            RE_ID_MP1_6y = 9;

const int32            RE_ID_MP2_1x = 11 - 1;
const int32            RE_ID_MP2_1y = 12 - 1;
const int32            RE_ID_MP2_2x = 13 - 1;
const int32            RE_ID_MP2_2y = 14 - 1;
const int32            RE_ID_MP2_3x = 15 - 1;
const int32            RE_ID_MP2_3y = 16 - 1;
const int32            RE_ID_MP2_4x = 17 - 1;
const int32            RE_ID_MP2_4y = 18 - 1;
const int32            RE_ID_MP2_5y = 19 - 1;

const int32            RE_ID_HP1_3y = 20 - 1; // Previously "MP3_3y"
const int32            RE_ID_QP1_1x = 21 - 1; // Previously "MP4_1x"
const int32            RE_ID_QP1_1y = 22 - 1; // Previously "MP4_1y"
const int32            RE_ID_QP1_2y = 23 - 1; // Previously "MP4_2y" 


    // Post-slit
const int32            RE_ID_QP2_1x = 24 - 1; // Previously "MP5_1x" 

const int32            RE_ID_QP2_1y = 25 - 1; // Previously "QP2_1y" 
const int32            RE_ID_QP2_2x = 26 - 1; // Previously "QP2_2x" 
const int32            RE_ID_QP2_2y = 27 - 1; // Previously "QP2_2y" 
const int32            RE_ID_QP3_1x = 28 - 1; // Previously "QP3_1x" 
const int32            RE_ID_QP3_1y = 29 - 1; // Previously "QP3_1y" 
const int32            RE_ID_QP3_2y = 30 - 1; // Previously "QP3_2y" 
const int32            RE_ID_QP3_4y = 31 - 1; // Previously "QP3_4y" 
const int32            RE_ID_QP4_1x = 32 - 1; // Previously "QP4_1x" 
const int32            RE_ID_QP4_1y = 33 - 1; // Previously "QP4_1y" 
const int32            RE_ID_QP4_2y = 34 - 1; // Previously "QP4_2y" 

const int32            RE_ID_HP2_2x = 35 - 1; // Previously "HP2_2x" 
const int32            RE_ID_HP2_3y = 36 - 1; // Previously "HP2_3y" 
const int32            RE_ID_QP5_1x = 37 - 1; // Previously "QP5_1x" 
const int32            RE_ID_QP5_1y = 38 - 1; // Previously "QP5_1y" 
const int32            RE_ID_QP5_2y = 39 - 1; // Previously "MP9_2y"
const int32            RE_ID_HP3_3y = 40 - 1; // Previously "MP10_3y"
const int32            RE_ID_HP4_3y = 41 - 1; // Previously "MP11_3y"
const int32            RE_ID_HOPD_Sector = 42 - 1;
const int32            RE_ID_HOPD_HT_adjust = 43 - 1;
const int32            RE_ID_DTD = 44 - 1;
*/
const uint32_t CommandPort = 7053;
const uint32_t EventPort = 7054;

} // namespace Control
} // namespace CeosProtocol
} // namespace Server
