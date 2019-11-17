#ifndef __WIRBELSCAN_SI_EXT_H_
#define __WIRBELSCAN_SI_EXT_H_
namespace SI_EXT {

/**********************************************************************************************************
*  table   pid           table_id    defined_by
*  PAT     0x00          0x00        MPEG-2  500msec         Program association section
*  CAT     0x01          0x01        MPEG-2  ?               Conditional access section
*  PMT     0x10..0x1FFE  0x02        MPEG-2  500msec         Program Map Table
*  TSDT    0x02          0x03        MPEG-2  10sec           Transport Stream Description Section
***********************************************************************************************************
*  NIT     0x10          0x40        DVB     25msec..10sec   Network Information section(actual)                       (mandatory)
*  SDT     0x11          0x42        DVB     25msec..2sec    Service description section(actual_transport_stream)      (mandatory)
*  EIT     0x12          0x4E        DVB     25msec..2sec    Event Information Section  (actual ts, present/following) (mandatory)
*  TDT     0x14          0x70        DVB     25msec..30sec   Time and date section                                     (mandatory)
***********************************************************************************************************
*  NIT     0x10          0x41        DVB     25msec..10sec   Other Network Information section                         (optional)
*  BAT     0x11          0x4A        DVB     25msec..10sec   Bouquet Association Table                                 (optional)
*  SDT     0x11          0x46        DVB     25msec..10sec   Other Service description section                         (optional)
*  EIT     0x12          0x4F        DVB     25msec..2sec    Event Information Section  (other ts, present/following)  (optional)
*  EIT     0x12          0x50..0x5F  DVB     25msec..10sec   Event Information Section  (actual ts, schedule)          (optional)
*  EIT     0x12          0x60..0x6F  DVB     25msec..10sec   Event Information Section  (other ts,  schedule)          (optional)
*  RST     0x13          0x71        DVB                     Running status section                                    (optional)
*  TOT     0x14          0x73        DVB     25msec..30sec   Time offset section                                       (optional)
*  MIP     0x15
*  ST      0x10..0x14    0x72        DVB     ----            Stuffing section                                          (optional)
*  DIT     0x1E          0x7E        DVB                     Discontinuity Information Section                         (optional)
*  SIT     0x1F          0x7F        DVB                     Selection Information Section                             (optional)
*  UNT     0x10..0x1FFE  0x4B        DVB
*  INT     0x10..0x1FFE  0x4C        DVB                     INT per PMT In PMT: data_broadcast_id = 0x000B
**********************************************************************************************************
*  AIT     0x74                                              AIT Used for MHP
*  DSM-CC  0x10..0x1FFE  0x39        DVB                     DSM-CC addressable section per PMT
*  DSM-CC  0x10..0x1FFE  0x3A        DVB                     DSM-CC, MPE per PMT
*  DSM-CC  0x10..0x1FFE  0x3B        DVB                     DSM-CC, U-N messages, except DDM per PMT
*  DSM-CC  0x10..0x1FFE  0x3C        DVB                     DSM-CC, DDM per PMT
*  DSM-CC  0x10..0x1FFE  0x3D        DVB                     DSM-CC, stream descriptors per PMT
*  DSM-CC  0x10..0x1FFE  0x3E        DVB                     DSM-CC, private data, IP-Datagram per PMT used for DVB-H
*  DSM-CC  0x10..0x1FFE  0x3F        DVB                     DSM-CC addressable section per PMT
**********************************************************************************************************
*          0x10..0x1FFE  0x78                                MPE-FEC Per PMT DVB-H, changed from 0x75
*          0x79                                              RNT
*  CA      0x01          0x80..0x8F  DVB                     CA EMM, CA ECM  (0x80, 0x81 = ECM)
**********************************************************************************************************/

/**************  DESCRIPTORS  ****************************************************************************
 *
 ########################### ISO 13181-1 ########################################################################
 * value    PAT    CAT    PMT    TSDT    NIT    BAT    SDT    EIT    TOT    PMT    SIT    Descriptor_Tag
 **********************************************************************************************************
 * 0x00    Reserved
 * 0x01    Reserved
 **********************************************************************************************************
 * 0x02    *    *    *    *    -    -    -    -    -    -    -    video_stream_descriptor
 * 0x03    *    *    *    *    -    -    -    -    -    -    -    audio_stream_descriptor
 * 0x04    *    *    *    *    -    -    -    -    -    -    -    hierarchy_descriptor
 * 0x05    *    *    *    *    -    -    -    -    -    -    -    registration_descriptor
 * 0x06    *    *    *    *    -    -    -    -    -    -    -    data_stream_alignment_descriptor
 * 0x07    *    *    *    *    -    -    -    -    -    -    -    target_background_grid_descriptor
 * 0x08    *    *    *    *    -    -    -    -    -    -    -    Video_window_descriptor
 * 0x09    *    *    *    *    -    -    -    -    -    -    -    CA_descriptor
 * 0x0A    *    *    *    *    -    -    -    -    -    -    -    ISO_639_language_descriptor
 * 0x0B    *    *    *    *    -    -    -    -    -    -    -    System_clock_descriptor
 * 0x0C    *    *    *    *    -    -    -    -    -    -    -    Multiplex_buffer_utilization_descriptor
 * 0x0D    *    *    *    *    -    -    -    -    -    -    -    Copyright_descriptor
 * 0x0E    *    *    *    *    -    -    -    -    -    -    -    Maximum_bitrate_descriptor
 * 0x0F    *    *    *    *    -    -    -    -    -    -    -    Private_data_indicator_descriptor
 * 0x10    *    *    *    *    -    -    -    -    -    -    -    Smoothing_buffer_descriptor
 * 0x11    *    *    *    *    -    -    -    -    -    -    -    STD_descriptor
 * 0x12    *    *    *    *    -    -    -    -    -    -    -    IBP_descriptor
 **********************************************************************************************************
 * 0x13    -    -    -    -                                CarouselIdentifierDescriptorTag
 * 0x14
 * ...
 * 0x1A    *    *    *    *    -    -    -    -    -    -    -    Defined    in ISO/IEC 13818-6 (DSM-CC)
 **********************************************************************************************************
 * 0x1B    *    *    *    *    -    -    -    -    -    -    -    MPEG-4_video_descriptor
 * 0x1C    *    *    *    *    -    -    -    -    -    -    -    MPEG-4_audio_descriptor
 * 0x1D    *    *    *    *    -    -    -    -    -    -    -    IOD_descriptor
 * 0x1E    *    *    *    *    -    -    -    -    -    -    -    SL_descriptor
 * 0x1F    *    *    *    *    -    -    -    -    -    -    -    FMC_descriptor
 * 0x20    *    *    *    *    -    -    -    -    -    -    -    External_ES_ID_descriptor
 * 0x21    *    *    *    *    -    -    -    -    -    -    -    MuxCode_descriptor
 * 0x22    *    *    *    *    -    -    -    -    -    -    -    FmxBufferSize_descriptor
 * 0x23    *    *    *    *    -    -    -    -    -    -    -    MultiplexBuffer_descriptor
 **********************************************************************************************************
 * 0x24
 * ...
 * 0x3F    ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
 **********************************************************************************************************
 ########################### EN 300 468 Version 1.8.1 ########################################################################
 * value    PAT    CAT    PMT    TSDT    NIT    BAT    SDT    EIT    TOT    PMT    SIT    Descriptor_Tag
 * 0x40    -    -    -    -    *    -    -    -    -    -    -    network_name_descriptor
 * 0x41    -    -    -    -    *    *    -    -    -    -    -    service_list_descriptor
 * 0x42    -    -    -    -    *    *    *    *    -    -    *    stuffing_descriptor
 * 0x43    -    -    -    -    *    -    -    -    -    -    -    satellite_delivery_system_descriptor
 * 0x44    -    -    -    -    *    -    -    -    -    -    -    cable_delivery_system_descriptor
 * 0x45    -    -    -    -    -    -    -    -    -    *    -    VBI_data_descriptor
 * 0x46    -    -    -    -    -    -    -    -    -    *    -    VBI_teletext_descriptor
 * 0x47    -    -    -    -    -    *    *    -    -    -    *    bouquet_name_descriptor
 * 0x48    -    -    -    -    -    -    *    -    -    -    *    service_descriptor
 * 0x49    -    -    -    -    -    *    *    -    -    -    *    country_availability_descriptor
 * 0x4A    -    -    -    -    *    *    *    *    -    -    *    linkage_descriptor
 * 0x4B    -    -    -    -    -    -    *    -    -    -    *    NVOD_reference_descriptor
 * 0x4C    -    -    -    -    -    -    *    -    -    -    *    time_shifted_service_descriptor
 * 0x4D    -    -    -    -    -    -    -    *    -    -    *    short_event_descriptor
 * 0x4E    -    -    -    -    -    -    -    *    -    -    *    extended_event_descriptor
 * 0x4F    -    -    -    -    -    -    -    *    -    -    *    time_shifted_event_descriptor
 * 0x50    -    -    -    -    -    -    *    *    -    -    *    component_descriptor
 * 0x51    -    -    -    -    -    -    *    -    -    *    *    mosaic_descriptor
 * 0x52    -    -    -    -    -    -    -    -    -    *    -    stream_identifier_descriptor
 * 0x53    -    -    -    -    -    *    *    *    -    -    *    CA_identifier_descriptor
 * 0x54    -    -    -    -    -    -    -    *    -    -    *    content_descriptor
 * 0x55    -    -    -    -    -    -    -    *    -    -    *    parental_rating_descriptor
 * 0x56    -    -    -    -    -    -    -    -    -    *    -    teletext_descriptor
 * 0x57    -    -    -    -    -    -    *    *    -    -    *    telephone_descriptor
 * 0x58    -    -    -    -    -    -    -    -    *    -    -    local_time_offset_descriptor
 * 0x59    -    -    -    -    -    -    -    -    -    *    -    subtitling_descriptor
 * 0x5A    -    -    -    -    *    -    -    -    -    -    -    terrestrial_delivery_system_descriptor
 * 0x5B    -    -    -    -    *    -    -    -    -    -    -    multilingual_network_name_descriptor
 * 0x5C    -    -    -    -    -    *    -    -    -    -    -    multilingual_bouquet_name_descriptor
 * 0x5D    -    -    -    -    -    -    *    -    -    -    *    multilingual_service_name_descriptor
 * 0x5E    -    -    -    -    -    -    -    *    -    -    *    multilingual_component_descriptor
 * 0x5F    -    -    -    -    *    *    *    *    -    *    *    private_data_specifier_descriptor
 * 0x60    -    -    -    -    -    -    -    -    -    *    -    service_move_descriptor
 * 0x61    -    -    -    -    -    -    -    *    -    -    *    short_smoothing_buffer_descriptor
 * 0x62    -    -    -    -    *    -    -    -    -    -    -    frequency_list_descriptor
 * 0x63    -    -    -    -    -    -    -    -    -    -    *    partial_transport_stream_descriptor
 * 0x64    -    -    -    -    -    -    *    *    -    -    *    data_broadcast_descriptor
 * 0x65    -    -    -    -    -    -    -    -    -    *    -    scrambling_descriptor
 * 0x66    -    -    -    -    -    -    -    -    -    *    -    data_broadcast_id_descriptor
 * 0x67    -    -    -    -    -    -    -    -    -    -    -    transport_stream_descriptor
 * 0x68    -    -    -    -    -    -    -    -    -    -    -    DSNG_descriptor
 * 0x69    -    -    -    -    -    -    -    *    -    -    -    PDC_descriptor
 * 0x6A    -    -    -    -    -    -    -    -    -    *    -    AC-3_descriptor
 * 0x6B    -    -    -    -    -    -    -    -    -    *    -    ancillary_data_descriptor
 * 0x6C    -    -    -    -    *    -    -    -    -    -    -    cell_list_descriptor
 * 0x6D    -    -    -    -    *    -    -    -    -    -    -    cell_frequency_link_descriptor
 * 0x6E    -    -    -    -    -    -    *    -    -    -    -    announcement_support_descriptor
 * 0x6F    -    -    -    -    -    -    -    -    -    *    -    application_signalling_descriptor
 * 0x70    -    -    -    -    -    -    -    -    -    *    -    adaptation_field_data_descriptor
 * 0x71    -    -    -    -    -    -    *    -    -    -    -    service_identifier_descriptor
 * 0x72    -    -    -    -    -    -    *    -    -    -    -    service_availability_descriptor
 * 0x73    -    -    -    -    *    *    *    -    -    -    -    default_authority_descriptor
 * 0x74    -    -    -    -    -    -    -    -    -    *    -    related_content_descriptor
 * 0x75    -    -    -    -    -    -    -    *    -    -    -    TVA_id_descriptor
 * 0x76    -    -    -    -    -    -    -    *    -    -    -    content_identifier_descriptor
 * 0x77    -    -    -    -    *    -    -    -    -    -    -    time_slice_fec_identifier_descriptor
 * 0x78    -    -    -    -    -    -    -    -    -    *    -    ECM_repetition_rate_descriptor
 * 0x79    -    -    -    -    *    -    -    -    -    -    -    S2_satellite_delivery_system_descriptor
 * 0x7A    -    -    -    -    -    -    -    -    -    *    -    enhanced_AC-3_descriptor
 * 0x7B    -    -    -    -    -    -    -    -    -    *    -    DTS_descriptor
 * 0x7C    -    -    -    -    -    -    -    -    -    *    -    AAC_descriptor
 * 0x7D    -    -    -    -    -    -    -    -    -    -    -    reserved
 * 0x7E    -    -    -    -    -    -    -    -    -    -    -    reserved
 * 0x7F    -    -    -    -    *    *    *    *    *    *    *    extension_descriptor
 **************************************************************************************************************************************
 * 0x80
 * ..
 * 0xFE   user_defined
 * 0xFF   forbidden
 *
 **********************************************************************************************************/

enum pid_codes {
  PID_PAT                                      = 0x0,
  PID_CAT                                      = 0x1,
  PID_PMT                                      = 0x2,
  PID_TSDT                                     = 0x3,
  PID_NIT                                      = 0x10,
  PID_SDT                                      = 0x11,
  PID_BAT                                      = 0x11,
  PID_EIT                                      = 0x12,
  PID_RST                                      = 0x13,
  PID_TDT                                      = 0x14,
  PID_DIT                                      = 0x1E,
  PID_SIT                                      = 0x1F,
  };
enum table_codes {
  TABLE_ID_PAT                                 = 0x0,  // program allocation table                                  (mandatory)
  TABLE_ID_CAT                                 = 0x1,  // conditional access section                                (mandatory)
  TABLE_ID_PMT                                 = 0x2,  // program map section                                       (mandatory)
  TABLE_ID_TSDT                                = 0x3,  // transport stream description section                      (mandatory)
  TABLE_ID_NIT_ACTUAL                          = 0x40, // network information section, *actual* network             (mandatory)
  TABLE_ID_NIT_OTHER                           = 0x41, // network information section, *other*  network             (optional)
  TABLE_ID_SDT_ACTUAL                          = 0x42, // service description section, *actual* transport stream    (mandatory)
  TABLE_ID_SDT_OTHER                           = 0x46, // service description section, *other*  transport stream    (optional)
  TABLE_ID_BAT                                 = 0x4A, // bouquet association section                               (optional)
  TABLE_ID_EIT_ACTUAL_PRESENT                  = 0x4E, // Event Information Section  (actual ts, present/following) (mandatory)
  TABLE_ID_EIT_OTHER_PRESENT                   = 0x4F, // Event Information Section  (other ts,  present/following) (optional)
  TABLE_ID_EIT_ACTUAL_SCHEDULE_START           = 0x50, // Event Information Section  (actual ts, schedule)          (optional)
  TABLE_ID_EIT_ACTUAL_SCHEDULE_STOP            = 0x5F, // Event Information Section  (actual ts, schedule)          (optional)
  TABLE_ID_EIT_OTHER_SCHEDULE_START            = 0x60, // Event Information Section  (other ts,  schedule)          (optional)
  TABLE_ID_EIT_OTHER_SCHEDULE_STOP             = 0x6F, // Event Information Section  (other ts,  schedule)          (optional)
  TABLE_ID_SIT                                 = 0x7F, // service information section                               (optional)
  TABLE_ID_CIT_PREM                            = 0xA0, // premiere content information section                      (optional, undefined in 13818/300468)
  };
enum _stream_type {
  STREAMTYPE_UNDEFINED                         = 0x0,  // ITU-T | ISO/IEC Reserved
  STREAMTYPE_11172_VIDEO                       = 0x1,  // ISO/IEC 11172 Video
  STREAMTYPE_13818_VIDEO                       = 0x2,  // ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream
  STREAMTYPE_11172_AUDIO                       = 0x3,  // ISO/IEC 11172 Audio
  STREAMTYPE_13818_AUDIO                       = 0x4,  // ISO/IEC 13818-3 Audio
  STREAMTYPE_13818_PRIVATE                     = 0x5,  // ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections
  STREAMTYPE_13818_PES_PRIVATE                 = 0x6,  // ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data
  STREAMTYPE_13522_MHEG                        = 0x7,  // ISO/IEC 13522 MHEG
  STREAMTYPE_H222_0_DSMCC                      = 0x8,  // ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM CC
  STREAMTYPE_H222_1                            = 0x9,  // ITU-T Rec. H.222.1
  STREAMTYPE_13818_6_TYPE_A                    = 0xA,  // ISO/IEC 13818-6 type A
  STREAMTYPE_13818_6_TYPE_B                    = 0xB,  // ISO/IEC 13818-6 type B
  STREAMTYPE_13818_6_TYPE_C                    = 0xC,  // ISO/IEC 13818-6 type C
  STREAMTYPE_13818_6_TYPE_D                    = 0xD,  // ISO/IEC 13818-6 type D
  STREAMTYPE_13818_1_AUX                       = 0xE,  // ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary
  STREAMTYPE_13818_AUDIO_ADTS                  = 0xF,  // ISO/IEC 13818-7 Audio with ADTS transport syntax
  STREAMTYPE_14496_VISUAL                      = 0x10, // ISO/IEC 14496-2 Visual
  STREAMTYPE_14496_AUDIO_LATM                  = 0x11, // ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1
  STREAMTYPE_14496_FLEX_PES                    = 0x12, // ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets
  STREAMTYPE_14496_FLEX_SECT                   = 0x13, // ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections.
  STREAMTYPE_13818_6_DOWNLOAD                  = 0x14, // ISO/IEC 13818-6 Synchronized Download Protocol
  STREAMTYPE_META_PES                          = 0x15, // Metadata carried in PES packets using the Metadata Access Unit Wrapper defined in 2.12.4.1
  STREAMTYPE_META_SECT                         = 0x16, // Metadata carried in metadata_sections
  STREAMTYPE_DSMCC_DATA                        = 0x17, // Metadata carried in ISO/IEC 13818-6 (DSM-CC) Data Carousel
  STREAMTYPE_DSMCC_OBJ                         = 0x18, // Metadata carried in ISO/IEC 13818-6 (DSM-CC) Object Carousel
  STREAMTYPE_META_DOWNLOAD                     = 0x19, // Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol using the Metadata Access Unit Wrapper defined in 2.12.4.1
  STREAMTYPE_14496_H264_VIDEO                  = 0x1B, // 0x1A-0x7F -> ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved
  STREAMTYPE_23008_H265_VIDEO                  = 0x24, // ISO/IEC23008-2 HEVC (H.265)
  STREAMTYPE_13818_USR_PRIVATE_80              = 0x80, // 0x80-0xFF User Private
  STREAMTYPE_13818_USR_PRIVATE_81              = 0x81,
  };
enum DescriptorTag {
  EnhancedAC3DescriptorTag                     = 0x7A,
  DTSDescriptorTag                             = 0x7B,
  AACDescriptorTag                             = 0x7C,
  };
enum stream_content_code {
  MPEG_2_video                                 = 0x1, 
  MPEG_1_Layer_2_audio                         = 0x2, 
  VBI_data                                     = 0x3, 
  AC3_audio_modes                              = 0x4, 
  H264_AVC_video                               = 0x5, 
  HE_AAC_audio                                 = 0x6, 
  DTS_audio                                    = 0x7,
  };
enum service_type_code {
  digital_television_service                   = 0x1,
  digital_radio_sound_service                  = 0x2,
  Teletext_service                             = 0x3,
  digital_television_NVOD_reference_service    = 0x4,
  digital_television_NVOD_timeshifted_service  = 0x5,
  mosaic_service                               = 0x6,
  advanced_codec_digital_radio_sound_service   = 0xA,
  advanced_codec_mosaic_service                = 0xB,
  data_broadcast_service                       = 0xC,
  reserved_Common_Interface_Usage_EN50221      = 0xD,
  RCS_Map_EN301790                             = 0xE,
  RCS_FLS_EN301790                             = 0xF,
  DVB_MHP_service                              = 0x10,
  MPEG2_HD_digital_television_service          = 0x11,
  advanced_codec_SD_digital_television_service = 0x16,
  advanced_codec_SD_NVOD_timeshifted_service   = 0x17,
  advanced_codec_SD_NVOD_reference_service     = 0x18,
  advanced_codec_HD_digital_television_service = 0x19,
  advanced_codec_HD_NVOD_timeshifted_service   = 0x1A,
  advanced_codec_HD_NVOD_reference_service     = 0x1B,
  };





#include <endian.h>
struct descr_t2_delivery_system {
   u_char descriptor_tag                         :8;
   u_char descriptor_length                      :8;
   u_char descriptor_tag_extension               :8;
   u_char plp_id                                 :8;
   u_char t2_system_id_hi                        :8;
   u_char t2_system_id_lo                        :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char siso_miso                              :2;
   u_char bandwidth                              :4;
   u_char reserved                               :2;
   u_char guard_interval                         :3;
   u_char transmission_mode                      :3;
   u_char other_frequency_flag                   :1;
   u_char tfs_flag                               :1;
#else
   u_char reserved                               :2;
   u_char bandwidth                              :4;
   u_char siso_miso                              :2;
   u_char tfs_flag                               :1;
   u_char other_frequency_flag                   :1;
   u_char transmission_mode                      :3;
   u_char guard_interval                         :3;
#endif
};

//#include <libsi/util.h>
#include <libsi/headers.h>
#include <libsi/descriptor.h>


#define DESCR_TERRESTRIAL_DELIVERY_SYSTEM_LEN XX
struct descr_terrestrial_delivery {
   u_char descriptor_tag                         :8;
   u_char descriptor_length                      :8;
   u_char frequency_hi_hi                        :8;
   u_char frequency_hi_lo                        :8;
   u_char frequency_lo_hi                        :8;
   u_char frequency_lo_lo                        :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char bandwidth                              :3;
   u_char priority                               :1;
   u_char time_slicing_indicator                 :1;
   u_char mpe_fec_indicator                      :1;
   u_char reserved1                              :2;
#else
   u_char reserved1                              :2;
   u_char mpe_fec_indicator                      :1;
   u_char time_slicing_indicator                 :1;
   u_char priority                               :1;
   u_char bandwidth                              :3;
#endif
#if BYTE_ORDER == BIG_ENDIAN
   u_char constellation                          :2;
   u_char hierarchy                              :3;
   u_char code_rate_HP                           :3;
#else
   u_char code_rate_HP                           :3;
   u_char hierarchy                              :3;
   u_char constellation                          :2;
#endif
#if BYTE_ORDER == BIG_ENDIAN
   u_char code_rate_LP                           :3;
   u_char guard_interval                         :2;
   u_char transmission_mode                      :2;
   u_char other_frequency_flag                   :1;
#else
   u_char other_frequency_flag                   :1;
   u_char transmission_mode                      :2;
   u_char guard_interval                         :2;
   u_char code_rate_LP                           :3;
#endif
   u_char reserver2                              :8;
   u_char reserver3                              :8;
   u_char reserver4                              :8;
   u_char reserver5                              :8;
};

class T2DeliverySystemDescriptor : public SI::Descriptor {
public:
   int getExtendedDataFlag()       const { return extended_data_flag; };
   int getExtensionDescriptorTag() const { return s->descriptor_tag_extension; };
   int getPlpId()                  const { return s->plp_id; };
   int getT2SystemId()             const { return HILO(s->t2_system_id); };
   int getSisoMiso()               const { return extended_data_flag ? s->siso_miso : -1; };
   int getBandwidth()              const { return extended_data_flag ? s->bandwidth : -1; };
   int getGuardInterval()          const { return extended_data_flag ? s->guard_interval : -1; };
   int getTransmissionMode()       const { return extended_data_flag ? s->transmission_mode : -1; };
   int getOtherFrequencyFlag()     const { return extended_data_flag ? s->other_frequency_flag : -1; };
   int getTfsFlag()                const { return extended_data_flag ? s->tfs_flag : -1; };
   u_char * getPointer()           const { return (uchar *) s; };
protected:
   virtual void Parse() {
     int offset=0;
     data.setPointerAndOffset<const descr_t2_delivery_system>(s, offset);
     extended_data_flag = s->descriptor_length > 0x04;
     };
private:
   const descr_t2_delivery_system *s;
   int extended_data_flag;
};

class TerrestrialDeliverySystemDescriptor : public SI::Descriptor {
public:
   int getFrequency()              const { return (HILO(s->frequency_hi) << 16) | HILO(s->frequency_lo); };
   int getBandwidth()              const { return s->bandwidth; };
   int getPriority()               const { return s->priority; };
   int getTimeSlicingIndicator()   const { return s->time_slicing_indicator; };
   int getMpeFecIndicator()        const { return s->mpe_fec_indicator; };
   int getConstellation()          const { return s->constellation; };
   int getHierarchy()              const { return s->hierarchy; };
   int getCodeRateHP()             const { return s->code_rate_HP; };
   int getCodeRateLP()             const { return s->code_rate_LP; };
   int getGuardInterval()          const { return s->guard_interval; };
   int getTransmissionMode()       const { return s->transmission_mode; };
   bool getOtherFrequency()        const { return s->other_frequency_flag; };
   u_char * getPointer()           const { return (uchar *) s; };
protected:
   virtual void Parse() { s=data.getData<const descr_terrestrial_delivery>(); };
private:
   const descr_terrestrial_delivery *s;
};








/************* do not touch ***************/
} //end of namespace
#endif
