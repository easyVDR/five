/***************************************************************************
 *   Copyright (C) 2012 R. Geigenberger                                    *
 *   Y.A.R.D.2 USB                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef COMMANDS_H
#define COMMANDS_H

// Global Defines
#define YARD_UNDEFINED   0x0A
#define YARD_ACK	     0x0F
#define YARD_GET_VERSION 0x10
#define SYS_IR_LOG   	 0x11
#define SYS_TestMBSW	 0x20
#define SYS_TestData	 0x30

#define SYS_EEPROM_R_BYTE 0x50
#define SYS_EEPROM_W_BYTE 0x51
#define SYS_BUTTON		 0x60
#define SYS_SPINBUTTON	 0x61
#define SYS_RTC_TIME_R	 0x6A
#define SYS_RTC_WU_R	 0x6B
#define SYS_RTC_TIME_W	 0x6C
#define SYS_RTC_WU_W	 0x6D
#define SYS_RTC_CMOS_R	 0x6E
#define SYS_RTC_CMOS_W	 0x6F
#define SYS_LCD_SET		 0x71
#define SYS_LCD_BL		 0x72
#define SYS_EEPROM_R	 0x73
#define SYS_EEPROM_W	 0x74
#define SYS_READ_LS      0x75
#define SYS_READ_CONFIG  0x76
#define SYS_LCD_W		 0x77
#define SYS_LCD_W_XY	 0x78
#define SYS_LCD_GOTO_XY  0x79
#define SYS_LCD_CLEAR    0x7A
#define SYS_LCD_SET_CC   0x7B
#define SYS_START_UP	 0x7C
#define SYS_DUMP_EEPROM  0x7D
#define SYS_IR_REC   	 0x80
#define SYS_IR_SEND      0x81
#define SYS_WAKEUP_IR    0x82
#define SYS_LCD_CLEAR_G  0x83
#define SYS_LCD_CLEAR_T  0x84

#define SYS_GLCD_PIXEL_XY 0x85
#define SYS_GLCD_WR_BYTE  0x87 //write byte to ram
#define SYS_GLCD_GOTO_RAM 0x88
#define SYS_GLCD_WR_BYTES 0x89 //write byte to ram


#define SYS_IR_REC_RAW	 0xF1
#define SYS_RESET_DummyFE    0xFE // Dummy for fix dualboot
#define SYS_RESET            0xFF //FF

  //EEprom adresen
  #define EE_LCD8BitMode    0; //Bit 0
  #define EE_LCD_show_time_off      0; //Bit 1
  #define EE_LCDshowwelcome         0; //Bit 2
  #define EE_LCDTestmode            0; //Bit 3
  #define EE_NO_POR                 0; //Bit 4
  #define EE_UseAutoBL              0; //Bit 5
  #define EE_NO_POR_IF_RTC_OK       0; //Bit 6
  #define EE_WAS_RESET_BY_SW        0; //Bit 7 // do not set this bit, set by FW if RESET is called


  #define EE_LCD_config_type        1; //00111111

  #define EE_LCD_config_row         2; //LCD zeile
  #define EE_LCD_config_col         3; //LCD zeile

  #define EE_LCD_max_backlight      4; //maximale helligkeit
  #define EE_LCD_min_backlight      5; //min helligkeit
  #define EE_LS_max                 6; //maximaler LS wert
  #define EE_LS_min                 7; //min LS wert
  #define EE_LCD_LS_high            8; //High 2 10Bit values
									//00000000
									//      10LCDmax
									//    32LCDmin
									//  54LSmax
									//76LSmin
  #define EE_LCD_Display_Type       9;

  #define EE_IRRepeatdelay          10; //ganzes byte
  #define EE_FontSize_ENC_Type      11; //1111xxxx  font xxxx1111  EncType

  #define EE_IRForceOff             16; // 8 ganze bytes
// Byte 16-23 gesperrt

  #define EE_IRWakeup               24; // 8 ganze bytes
// Byte 24-31 gesperrt

  #define EE_LCDWelcomestring       32; // 16 ganze bytes
// Byte 33-47 gesperrt

  #define EE_YardFWupdate           254; //$FE;

#endif

