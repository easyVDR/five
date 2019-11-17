/*
 * Simple MPEG/DVB parser to achieve network/service information without initial tuning data
 *
 * Copyright (C) 2006, 2007, 2008, 2009 Winfried Koehler 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * The author can be reached at: handygewinnspiel AT gmx DOT de
 *
 * The project's page is http://wirbel.htpc-forum.de/w_scan/index2.html
 */



/* this file is shared between w_scan and the VDR plugin wirbelscan.
 * For details on both of them see http://wirbel.htpc-forum.de
 */


#ifndef _EXTENDED_DVBFRONTEND_H_
#define _EXTENDED_DVBFRONTEND_H_

#include <linux/dvb/frontend.h>
#include <linux/dvb/version.h>

#define DVB_API ( 10 * DVB_API_VERSION + DVB_API_VERSION_MINOR )

/******************************************************************************
 * definitions which are missing in <linux/dvb/frontend.h>
 *
 *****************************************************************************/

#ifndef fe_polarization // 300468 v181  6.2.13.2 Satellite delivery system descriptor
typedef enum fe_polarization {
        POLARIZATION_HORIZONTAL,
        POLARIZATION_VERTICAL,
        POLARIZATION_CIRCULAR_LEFT,
        POLARIZATION_CIRCULAR_RIGHT,
} fe_polarization_t;
#endif

#ifndef fe_west_east_flag       // 300468 v181  6.2.13.2 Satellite delivery system descriptor
typedef enum fe_west_east_flag {
        EAST_FLAG,
        WEST_FLAG,
} fe_west_east_flag_t;
#endif

#ifndef fe_interleave   // 300468 v181  6.2.13.4 Terrestrial delivery system descriptor
typedef enum fe_interleaver {
        INTERLEAVE_NATIVE,
        INTERLEAVE_IN_DEPTH,
        INTERLEAVE_AUTO,
} fe_interleave_t;
#endif

#ifndef fe_alpha        // 300468 v181  6.2.13.4 Terrestrial delivery system descriptor
typedef enum fe_alpha {
        ALPHA_1,
        ALPHA_2,
        ALPHA_4,
        ALPHA_AUTO,
} fe_alpha_t;
#endif

#ifndef fe_priority     // 300468 v181  6.2.13.4 Terrestrial delivery system descriptor
typedef enum fe_priority {
        PRIORITY_HP,
        PRIORITY_LP,
        PRIORITY_AUTO,
} fe_priority_t;
#endif

#ifndef fe_time_slicing // 300468 v181  6.2.13.4 Terrestrial delivery system descriptor
typedef enum fe_time_slicing {
        TIME_SLICING_ON,
        TIME_SLICING_OFF,
        TIME_SLICING_AUTO,
} fe_time_slicing_t;
#endif

#ifndef fe_mpe_fce      // 300468 v181  6.2.13.4 Terrestrial delivery system descriptor
typedef enum fe_mpe_fce {
        MPE_FCE_ON,
        MPE_FCE_OFF,
        MPE_FCE_AUTO,
} fe_mpe_fce_t;
#endif

typedef enum fe_siso_miso {
        SISO,
        MISO,
        SISO_MISO_RESERVED1,
        SISO_MISO_RESERVED2,
} fe_siso_miso_t;

#ifndef SYS_DVBC2                              //// \BEGIN   FIX_ME: _REALLY_ DIRTY HACK. JUST FOR TESTING THIS CODE. REMOVE LATER!!!!!
#define SYS_DVBC2 (SYS_TURBO+1)
#ifndef QAM_512
#define QAM_512   (DQPSK + 1)
#endif
#ifndef QAM_1024
#define QAM_1024   (QAM_512 + 1)
#endif
#ifndef QAM_4096
#define QAM_4096   (QAM_1024 + 1)
#endif
#ifndef GUARD_INTERVAL_1_64
#define GUARD_INTERVAL_1_64 (GUARD_INTERVAL_19_256 + 1)
#endif
#endif                                         //// \END     REMOVE UP TO HERE!!!

#ifndef SYS_TURBO  // remove later.
#define SYS_TURBO (SYS_DVBT2 + 1)
#endif

#ifndef DTV_ENUM_DELSYS // remove later.
#define DTV_ENUM_DELSYS 44
#endif

typedef enum {
        DATA_SLICE_TUNING_FREQUENCY,
        C2_SYSTEM_CENTER_FREQUENCY,
        INITIAL_TUNING_FOR_STATIC_DATA_SLICE,
} fe_frequency_type_t;

typedef enum {
        FFT_4K_8MHZ,
        FFT_4K_6MHZ,
} fe_ofdm_symbol_duration_t;

/* don't use FE_OFDM, FE_QAM, FE_QPSK, FE_ATSC any longer.
 *
 * With evolving delivery systems, cable uses also ofdm (DVB-C2), satellites doesnt
 * use only qpsk (DVB-S2), 2nd gen terrestrial and cable use physical layer pipes.
 * Use of delivery_system doesnt fit for me either, since i dont want to distinguish
 * between DVB-S <-> DVB-S2 or DVB-C <-> DVB-C2 or DVB-T <-> DVB-T2.
 * Therefore, name it on physical path for now.
 * 20120107 wirbel
 */
typedef enum {
        SCAN_UNDEFINED,
        SCAN_SATELLITE,
        SCAN_CABLE,
        SCAN_TERRESTRIAL,
        SCAN_TERRCABLE_ATSC,   /* I dislike this mixture of terr and cable. fix later, as it leads to problems now. */
        SCAN_PVRINPUT,
        SCAN_PVRINPUT_FM,
        SCAN_NO_DEVICE,
        SCAN_TRANSPONDER=999
} scantype_t;


/* since Linux DVB API v5 'struct dvb_qpsk_parameters' in frontend.h
 * is no longer able to store all information related to a
 * DVB-S frontend. Some information still missing at all in v5.
 */
struct satellite_transponder {
        __u32                   symbol_rate;                    /* symbols per second */
        fe_code_rate_t          fec_inner;                      /* inner forward error correction */
        fe_modulation_t         modulation_type;
        fe_pilot_t              pilot;                          /* not shure about this one. */
        fe_rolloff_t            rolloff;
        fe_delivery_system_t    modulation_system;
        fe_polarization_t       polarization;                   /* urgently missing in frontend.h */
        __u32                   orbital_position;
        fe_west_east_flag_t     west_east_flag;
        __u8                    scrambling_sequence_selector;   /* 6.2.13.3 S2 satellite delivery system descriptor */
        __u8                    multiple_input_stream_flag;     /* 6.2.13.3 S2 satellite delivery system descriptor */
        __u32                   scrambling_sequence_index;      /* 6.2.13.3 S2 satellite delivery system descriptor */
        __u8                    input_stream_identifier;        /* 6.2.13.3 S2 satellite delivery system descriptor */
};

/* since Linux DVB API v5 'struct dvb_qam_parameters' in frontend.h
 * is no longer able to store all information related to a
 * DVB-C frontend.
 */
struct cable_transponder {
        __u32                   symbol_rate;    /* symbols per second */
        fe_code_rate_t          fec_inner;      /* inner forward error correction */
        fe_modulation_t         modulation;     /* modulation type */
        fe_delivery_system_t    delivery_system;
        __u32                   fec_outer;      /* not supported at all */
        /*********************************** below: prepare for DVB-C2 *********************************************/
        fe_frequency_type_t     C2_tuning_frequency_type;
        fe_ofdm_symbol_duration_t active_OFDM_symbol_duration;
        fe_guard_interval_t     guard_interval;
};

/* since Linux DVB API v5 'struct dvb_vsb_parameters' in frontend.h
 * is no longer able to store all information related to a
 * ATSC frontend.
 */
struct atsc_terrestrial_transponder {
        fe_modulation_t         modulation;     /* modulation type */
        fe_code_rate_t          fec_inner;      /* forward error correction */
        fe_delivery_system_t    delivery_system;
        __u32                   fec_outer;      /* not supported at all */
};

/* since Linux DVB API v5 'struct dvb_ofdm_parameters' in frontend.h
 * is no longer able to store all information related to a
 * DVB-T frontend. Some information still missing at all in v5.
 */
struct  terrestrial_transponder {
        __u32                   bandwidth;
        fe_code_rate_t          code_rate_HP;   /* high priority stream code rate */
        fe_code_rate_t          code_rate_LP;   /* low priority stream code rate */
        fe_modulation_t         constellation;  /* modulation type */
        fe_transmit_mode_t      transmission_mode;
        fe_guard_interval_t     guard_interval;
        fe_hierarchy_t          hierarchy_information;
        fe_delivery_system_t    delivery_system;
        fe_alpha_t              alpha;          /* only defined in w_scan (see above) */
        fe_interleave_t         interleaver;    /* only defined in w_scan (see above) */
        fe_priority_t           priority;       /* only defined in w_scan (see above) */
        fe_time_slicing_t       time_slicing;   /* only defined in w_scan (see above) */
        fe_mpe_fce_t            mpe_fce;        /* only defined in w_scan (see above) */
        /*********************************** below: prepare for DVB-T2 *********************************************/
        __u8                    extended_info; /* 1 if the data below is present, 0 otherwise                      */
        fe_siso_miso_t          SISO_MISO;     /* 0 = single input single output, 1 = multiple input single output */
        __u8                    tfs_flag;      /* 1 if TFS arrangement used, 0 otherwise                           */
};

/******************************************************************************
 * intended to be used similar to struct dvb_frontend_parameter
 *
 *****************************************************************************/
struct  tuning_parameters {
        __u32 frequency;                        /* cable/terrestrial/ATSC: abs. frequency in Hz */
                                                /* satellite: intermediate frequency in kHz     */
        fe_spectral_inversion_t inversion;
        union {
                struct satellite_transponder        sat;
                struct cable_transponder            cable;
                struct terrestrial_transponder      terr;
                struct atsc_terrestrial_transponder atsc;
        } u;
};

#endif
