#ifndef SI114X_DEFS_H
#define SI114X_DEFS_H
namespace Si114x
{
   enum {   // register values
      /* 0x00 */ PART_ID,
      REV_ID,
      SEQ_ID,
      INT_CFG,
      /* 0x04 */ IRQ_ENABLE,
      IRQ_MODE1,
      IRQ_MODE2,
      HW_KEY,
      /* 0x08 */ MEAS_RATE,
      ALS_RATE,
      PS_RATE,
      ALS_LOW_TH,
      /* 0x0C */ RESERVED_0C,
      ALS_HI_TH,
      RESERVED_0E,
      PS_LED21,
      /* 0x10 */ PS_LED3,
      PS1_TH,
      RESERVED_12,
      PS2_TH,
      /* 0x14 */ RESERVED_14,
      PS3_TH,
      RESERVED_16,
      PARAM_WR,
      /* 0x18 */ COMMAND,
      /* gap: 0x19..0x1F */ RESERVED_1F = 0x1F,
      /* 0x20 */ RESPONSE,
      IRQ_STATUS,
      ALS_VIS_DATA0,
      ALS_VIS_DATA1,
      /* 0x24 */ ALS_IR_DATA0,
      ALS_IR_DATA1,
      PS1_DATA0,
      PS1_DATA1,
      /* 0x28 */ PS2_DATA0,
      PS2_DATA1,
      PS3_DATA0,
      PS3_DATA1,
      /* 0x2C */ AUX_DATA0,
      AUX_DATA1,
      PARAM_RD,
      RESERVED_2F,
      /* 0x30 */ CHIP_STAT,
      /* gap: 0x31..0x3A */ RESERVED_3A = 0x3A,
      /* 0x3B */ ANA_IN_KEY1,
      ANA_IN_KEY2,
      ANA_IN_KEY3,
      ANA_IN_KEY4,
   };

   enum {   // Parmeter RAM values
      // Parameter Offsets
      PARAM_I2C_ADDR           = 0x00,
      PARAM_CH_LIST            = 0x01,
      PARAM_PSLED12_SELECT     = 0x02,
      PARAM_PSLED3_SELECT      = 0x03,
      PARAM_FILTER_EN          = 0x04,
      PARAM_PS_ENCODING        = 0x05,
      PARAM_ALS_ENCODING       = 0x06,
      PARAM_PS1_ADCMUX         = 0x07,
      PARAM_PS2_ADCMUX         = 0x08,
      PARAM_PS3_ADCMUX         = 0x09,
      PARAM_PS_ADC_COUNTER     = 0x0A,
      PARAM_PS_ADC_CLKDIV      = 0x0B,
      PARAM_PS_ADC_GAIN        = 0x0B,
      PARAM_PS_ADC_MISC        = 0x0C,
      PARAM_ALS1_ADC_MUX       = 0x0D,
      PARAM_ALS2_ADC_MUX       = 0x0E,
      PARAM_ALS3_ADC_MUX       = 0x0F,
      PARAM_ALSVIS_ADC_COUNTER = 0x10,
      PARAM_ALSVIS_ADC_CLKDIV  = 0x11,
      PARAM_ALSVIS_ADC_GAIN    = 0x11,
      PARAM_ALSVIS_ADC_MISC    = 0x12,
      PARAM_ALS_HYST           = 0x16,
      PARAM_PS_HYST            = 0x17,
      PARAM_PS_HISTORY         = 0x18,
      PARAM_ALS_HISTORY        = 0x19,
      PARAM_ADC_OFFSET         = 0x1A,
      PARAM_SLEEP_CTRL         = 0x1B,
      PARAM_LED_RECOVERY       = 0x1C,
      PARAM_ALSIR_ADC_COUNTER  = 0x1D,
      PARAM_ALSIR_ADC_CLKDIV   = 0x1E,
      PARAM_ALSIR_ADC_GAIN     = 0x1E,
      PARAM_ALSIR_ADC_MISC     = 0x1F
   };

   enum {                    // Command Register Values
      NOP_cmd = 0,           // Forces a zero into the RESPONSE register
      RESET_cmd,             // Performs a software reset of the firmware
      BUSADDR_cmd,           // Modifies I2C address
      PS_FORCE_cmd    = 5,   // Forces a single PS measurement
      PSALS_FORCE_cmd = 7,   // Forces a single PS and ALS measurement
      PS_PAUSE_cmd    = 9,   // Pauses autonomous PS
      ALS_PAUSE_cmd,         // Pauses autonomous ALS
      PSALS_PAUSE_cmd,       // Pauses PS and ALS
      PS_AUTO_cmd = 13,      // Starts/Restarts an autonomous PS Loop
      ALS_AUTO_cmd,          // Starts/Restarts an autonomous ALS Loop
      PSALS_AUTO_cmd         // Starts/Restarts autonomous ALS and PS loop
   };
}
#endif