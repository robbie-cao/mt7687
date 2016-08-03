/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_platform.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#include "hal_audio_fw_sherif.h"
#include "hal_audio_internal_afe.h"


#include "assert.h"
#include <string.h>

#define VM_WRITE_TO_FILE 0

#define VM_CTRL_UL         1
#define VM_CTRL_DL         2
#define VM_CTRL_SC_START   2

#define VM_CTRL_SD_START   6



#define VM_STATE_RECORD       0x1
#define VM_STATE_IDLE         0x10

#define VM_STATE_RECORD_STOP  0x20 // entering vmStop

#define VM_BASIC_VM_DATA_SIZE    37 //SyncWord(1), header(1), vm.control(1), counter(1), counter(1), 16*2

#define VM_2G_CTRL_DL_DEBUG_SIZE 44



#define VM_3G_DSP_DEBUG_SIZE     22 //DSP 15 + L1_info 2 + crc_result|DCH_On/Off 1 + s_value 1 + tpc_SIR_lta 1 + dpdch_SIR_lta 1 + TFCI_max_corr 1
#define VM_3G_MCU_DEBUG_SIZE     37 //buffer status 1 + UL 17 + DL 19
#define VM_3G_DEBUG_SIZE         ( VM_3G_DSP_DEBUG_SIZE + VM_3G_MCU_DEBUG_SIZE )

#define VM_3G324M_DSP_DEBUG_SIZE 22 //DSP 15 + L1_info 2 + crc_result|DCH_On/Off 1 + s_value 1 + tpc_SIR_lta 1 + dpdch_SIR_lta 1 + TFCI_max_corr 1
#define VM_3G324M_MCU_DEBUG_SIZE 37 //buffer status 1 + UL 17 + DL 19
#define VM_3G324M_DEBUG_SIZE     ( VM_3G324M_DSP_DEBUG_SIZE + VM_3G324M_MCU_DEBUG_SIZE )

#define VM_MAXIMUM_SAVE_SIZE     91 //VM_3G_MCU_DEBUG_SIZE + VM_BASIC_VM_DATA_SIZE


#define FIVE_PCM_SAVE_SIZE               (2+320+320+2+320+320+2+320+2)   //UL 320+320+2; DL 320+320+2; REF 320+2+2

#define VM_BUFFER_SIZE           513

// #ifndef UPPER_BOUND
// #define UPPER_BOUND(in,up) ((in) > (up) ? (up) : (in))
// #endif


#define VM_MAGIC_HEADER 0xAA88


/* ------------------------------------------------------------------------------ */

static struct {

    // void (*vm_hdlr)(void); // callback function for vm logging


    uint16_t         control;    /* control word for FR/HR/EFR                         */
    /* bit  0-1:   Channel usage(bit0:UL,bit1:DL)         */
    /* bit  2-5:   UL Speech mode(0->FR, 1->HR, 2->EFR)   */
    /* bit  6-9:   DL Speech mode(0->FR, 1->HR, 2->EFR)   */
    /* bit  10:    DP_TX_TCH_S0_0 data bit 1              */
    /* bit 11-15:  DP_RX_TCH_S0_0 data bit 1-5            */
    /* control word for AMR                               */
    /* bit  0-1:   Channel usage(bit0:UL,bit1:DL)         */
    /* bit  2-5:   UL Speech mode(0->FR, 1->HR, 2->EFR)   */
    /* bit  6-9:   DL Speech mode(0->FR, 1->HR, 2->EFR)   */
    /* bit 10-11:  DP_TX_TCH_S0_0 data bit 0-1            */
    /* bit 12-15:  DP_RX_TCH_S0_0 data bit 1-3,5          */

    uint16_t         control_1;
    uint16_t         control_2;
    // uint32_t         rb_data_len;  /* actual data size (in word) in ring buffer */

    uint16_t         *vmBuf;    /* point to buffer for VM                              */
    uint16_t         *pcmBuf;   /* point to buffer for PCM data         */
    uint16_t         debug_info; /* 0  : vm (speech codec data) only                   */
    /* b1 : record 1st set of UL-DL PCM buffer for SPE    */
    /* b2 : record 2nd set of UL-DL PCM buffer for SPE    */
    uint8_t          state;
    uint8_t          vm_lost;
    uint8_t          vm_lost_count;
    uint8_t          pcm_lost_count;
    uint8_t          sc_mode;
    uint8_t          sd_mode;

    bool           isVmLOn; // only use under call AM_IsSpeechOn()

    uint32_t         pcm_save_size; // EPL PCM buffer size

} vm;

#if VM_WRITE_TO_FILE
FIL vmFileHdl;
#endif

static const uint8_t SpeechPatternLength[] = {
    (uint8_t)((260 + 15) / 16),           /*  FR       */
    (uint8_t)((112 + 15) / 16 + 1),       /*  HR       */
    (uint8_t)((244 + 15) / 16),           /* EFR       */
    (uint8_t)((244 + 15) / 16),           /* AMR 12.2K */
    (uint8_t)((204 + 15) / 16),           /* AMR 10.2K */
    (uint8_t)((159 + 15) / 16),           /* AMR 7.95K */
    (uint8_t)((148 + 15) / 16),           /* AMR 7.4K  */
    (uint8_t)((134 + 15) / 16),           /* AMR 6.7K  */
    (uint8_t)((118 + 15) / 16),           /* AMR 5.9K  */
    (uint8_t)((103 + 15) / 16),           /* AMR 5.15K */
    (uint8_t)(( 95 + 15) / 16),           /* AMR 4.75K */
//#if defined(AMRWB_DECODE) || defined(__AMRWB_LINK_SUPPORT__)
    (uint8_t)((132 + 15) / 16),           /* AMR-WB 6.60  */
    (uint8_t)((177 + 15) / 16),           /* AMR-WB 8.85  */
    (uint8_t)((253 + 15) / 16),           /* AMR-WB 12.65 */
    (uint8_t)((285 + 15) / 16),           /* AMR-WB 14.25 */
    (uint8_t)((317 + 15) / 16),           /* AMR-WB 15.85 */
    (uint8_t)((365 + 15) / 16),           /* AMR-WB 18.25 */
    (uint8_t)((397 + 15) / 16),           /* AMR-WB 19.85 */
    (uint8_t)((461 + 15) / 16),           /* AMR-WB 23.05 */
    (uint8_t)((477 + 15) / 16),           /* AMR-WB 23.85 */
};


uint8_t   vm_counter;

#define VM_PCM_1ST_SET_RECORD_FLAG  0x1
#define VM_PCM_2ND_SET_RECORD_FLAG  0x2
#define VM_VM_RECORD_FLAG           0x4
#define VM_PCM_REFMIC_RECORD_FLAG   0x8

// #define TCH_VM_INT_FLAG             0x1
// #define TCH_PCM_INT_FLAG            0x2

// #define VMREC_OUTPUT_BUF_SIZE (5120+2000)
// static uint16_t vmRecOutputBuf[VMREC_OUTPUT_BUF_SIZE];
static uint16_t vmCodecBuf[VM_BUFFER_SIZE];
static uint16_t eplInputBuf[FIVE_PCM_SAVE_SIZE];

/* ------------------------------------------------------------------------------ */

uint16_t vmRec_GetSpeechPatternLength( uint8_t speech_mode )
{
    if ( speech_mode >= 0x20 ) {
        speech_mode = speech_mode - 0x20 + 0x0B;
    }
    if ( speech_mode < (sizeof(SpeechPatternLength) / sizeof(SpeechPatternLength[0]))) {
        return (uint16_t)SpeechPatternLength[speech_mode];
    } else {
        return 0;
    }
}


// #pragma arm section code="SECONDARY_ROCODE"


//use this function instead of Media_WriteDataDone to avoid media.waiting false usage

static bool vmWriteVmToMB(uint32_t len, uint16_t *pBuf)
{
#if VM_WRITE_TO_FILE
    UINT written;
    uint8_t *tempPtr;
    FRESULT result;

    len = len * sizeof(uint16_t);
    written = 0;
    tempPtr = (uint8_t *)(pBuf);

    while (len > 0) {
        result = f_write(&vmFileHdl, tempPtr + written, len, &written); //  vmWriteVmToMB((uint32_t)I, vmBuf);
        if (FR_OK != result) {
            return false;
        }

        tempPtr += written;
        len = len - written;
    }
#endif
    return true;
}


typedef struct {

    uint16_t enc_mode;
    uint16_t dec_mode;
    uint16_t enc_hdr;
    uint16_t dec_hdr;

    volatile uint16_t *enc_hb_addr;
    volatile uint16_t *dec_hb_addr;
    volatile uint16_t *dbgInfo_addr;
    volatile uint16_t *enh_dbgInfo_addr;

} Sal_VM_Frame;



Sal_VM_Frame vmfrm_t;

#define SAL_VM_DBGINFO_LEN 56
#define SAL_VM_ENH_DBGINFO_LEN 69
#define SAL_VM_DRV_DBGINFO_LEN 20

void SALI_VM_GetFrame(uint8_t rat, Sal_VM_Frame *vmfrm)

{
    // rat == 0 -> 2g

    vmfrm->enc_mode     = *DSP_SPH_Encoder_Used_Mode;
    vmfrm->dec_mode     = *DSP_SPH_Decoder_Used_Mode;
    vmfrm->dbgInfo_addr = DSP_SPH_VM_DBG_INFO;

    if (rat == 0) {
        // 2g
        vmfrm->enc_hdr     = *DSP_SPH_2G_SE_DATA_HDR;
        vmfrm->enc_hb_addr = DSP_SPH_2G_SE_DATA_HB;
        vmfrm->dec_hdr     = *DSP_SPH_2G_SD_DATA_HDR;
        vmfrm->dec_hb_addr = DSP_SPH_2G_SD_DATA_HB;
    } else if (rat == 1) {
        // 3g

        vmfrm->enc_hdr     = *DSP_SPH_3G_SE_DATA_HDR;
        vmfrm->enc_hb_addr = DSP_SPH_3G_SE_DATA_HB;
        vmfrm->dec_hdr     = *DSP_SPH_3G_SD_DATA_HDR;
        vmfrm->dec_hb_addr = DSP_SPH_3G_SD_DATA_HB;
    }

    vmfrm->enh_dbgInfo_addr = 0;    //DSP_DM_ADDR(6, *DSP_SPH_ENH_DEBUG_ADDR);

    return;

}


#define AA88_GET_VM_LENGTH(vmctrl_1, vmctrl_2) vmRec_GetSpeechPatternLength((vmctrl_1>> 1) & 0x3F) \
+ ((vmctrl_2 & 1)?(vmRec_GetSpeechPatternLength((vmctrl_2 >> 1) & 0x3F)):0) + 7



void audio_vm_vmRecordHisr(void *param)

{

    volatile uint16_t   *addr;

    uint16_t sc_mode, sd_mode, sc_len, sd_len;
    uint16_t *vmBuf;
    // uint32_t   vmLen = 0;
    uint32_t I, J;


    if ( vm.state != VM_STATE_RECORD ) {
        return;
    }


    vmBuf = vm.vmBuf;

    if (vmBuf[0] == VM_MAGIC_HEADER) {
        // Already buffer VM
        // uint8_t *tempPtr;

        I = (uint16_t)(vmBuf[1] >> 3)
            + AA88_GET_VM_LENGTH(vm.control_1, vm.control_2);
        // + vmRec_GetSpeechPatternLength((vmBuf[5]>> 1) & 0x3F)
        // + ((vm.control_2 & 1)?(vmRec_GetSpeechPatternLength((vmBuf[6] >> 1) & 0x3F)):0)
        // + 7; // vm header len

        // write
        if (false == vmWriteVmToMB(I, vm.vmBuf))	{
            vm.vm_lost_count ++;
        } else {
            vm.vm_lost_count = 0;
        }

        vm.vmBuf[0] = 0;
        vm.vmBuf[1] = 0;
        vm.vmBuf[2] = 0;
    }


    // begin put information to vmBuf
    vmBuf = vm.vmBuf;

    *vmBuf = VM_MAGIC_HEADER;
    vmBuf++;

    J = 0; // L1SP_GetState();
    *vmBuf = ((uint16_t)( ( (SAL_VM_DBGINFO_LEN + SAL_VM_ENH_DBGINFO_LEN + SAL_VM_DRV_DBGINFO_LEN) << 3 ) | J));
    vmBuf++;
    *vmBuf = VM_VM_RECORD_FLAG;
    vmBuf++;


    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &J); // J = L1I_GetTimeStamp();
    J = (J >> 10); // i.e. j= j/1024 around ms
    *vmBuf = ((uint16_t)(J & 0xFFFF));
    vmBuf++;
    *vmBuf = ((uint16_t)((J >> 16) + ((uint32_t)vm_counter << 8)));
    vmBuf++;

    vm_counter++;
    // ======= end ======


    SALI_VM_GetFrame(0, &vmfrm_t);
    sc_mode = vmfrm_t.enc_mode;
    sd_mode = vmfrm_t.dec_mode;

    // For catcher log codec information
    /*
    	if (   vm.sc_mode != sc_mode
    		|| vm.sd_mode != sd_mode)
    	{
    		L1Audio_Msg_SPEECH_CODEC(L1SP_Speech_Codec_Mode(sc_mode), L1SP_Speech_Codec_Mode(sd_mode));
    		vm.sc_mode = sc_mode;
    		vm.sd_mode = sd_mode;
    	}
    */

    assert( (sc_len = vmRec_GetSpeechPatternLength(sc_mode)) > 0 );
    assert( (sd_len = vmRec_GetSpeechPatternLength(sd_mode)) > 0 );

    vm.control_1 = (sc_mode << 1) | (vm.control_1 & 1);
    vm.control_2 = (sd_mode << 1) | (vm.control_2 & 1);


    if (sc_mode > 2) {
        // I = vmfrm_t.enc_hdr & 3;//bit0, bit1
        vm.control_1 = vm.control_1 | ((vmfrm_t.enc_hdr & 3) << 7); //Tx
    } else {
        // I = (vmfrm_t.enc_hdr & 2);//sp_flag
        vm.control_1 = vm.control_1 | ((vmfrm_t.enc_hdr & 2) << 10);
        //add 3G_Mode here, where is dsp's 3g mode indicator
    }



    if (sd_mode > 2) {
        // I = (vmfrm_t.dec_hdr & 0xe) >> 1;//bit1, bit2, bit3
        vm.control_2 = vm.control_2 | (((vmfrm_t.dec_hdr & 0xe) >> 1) << 7); // Rx
    } else {
        // I = (vmfrm_t.dec_hdr & 0x3E);
        vm.control_2 = vm.control_2 | ((vmfrm_t.dec_hdr & 0x3E) << 10);
    }


    *vmBuf++ = vm.control_1;
    *vmBuf++ = vm.control_2;


    if ( vm.control_1 & 1 ) {
        addr = vmfrm_t.enc_hb_addr;
        // vmLen += sc_len;
        for ( I = 0; I < sc_len; I++ ) {
            *vmBuf++ = *addr++;
        }
    }

    if ( vm.control_2 & 1 ) {
        addr = vmfrm_t.dec_hb_addr;
        // vmLen += sc_len;
        for ( I = 0; I < sd_len; I++ ) {
            *vmBuf++ = *addr++;
        }
    }

    addr = vmfrm_t.dbgInfo_addr;
    for ( I = 0; I < SAL_VM_DBGINFO_LEN; I++ ) {
        *vmBuf++ = *addr++;
    }

    //addr = vmfrm_t.enh_dbgInfo_addr;
    for ( I = 0; I < SAL_VM_ENH_DBGINFO_LEN; I++ ) {
        *vmBuf++ = 0;
    }

    for (I = 0; I < SAL_VM_DRV_DBGINFO_LEN; I++) {
        *vmBuf++ = 0;
    }

}


// #pragma arm section

#define NB_FRAME_BUF_SIZE (160)
#define WB_FRAME_BUF_SIZE (320)

// #pragma arm section code="SECONDARY_ROCODE"

#define VM_PCM_BAND_FLAG_UL_PRE		0x0010
#define VM_PCM_BAND_FLAG_UL_POS		0x0020
#define VM_PCM_BAND_FLAG_DL_PRE		0x0040
#define VM_PCM_BAND_FLAG_DL_POS		0x0080
#define VM_PCM_BAND_FLAG_UL2_POS	0x0100


typedef struct {

    uint16_t ul_pre_len;
    uint16_t ul_pos_len;
    uint16_t dl_pre_len;
    uint16_t dl_pos_len;
    uint16_t ul2_pos_len;

    volatile uint16_t *ul_pre_buf;
    volatile uint16_t *ul_pos_buf;
    volatile uint16_t *dl_pre_buf;
    volatile uint16_t *dl_pos_buf;
    volatile uint16_t *ul2_pos_buf;

} Sal_EPL_Frame;



Sal_EPL_Frame eplfrm_t;



void SALI_EPL_GetFrame(Sal_EPL_Frame *eplfrm)

{
    eplfrm->ul_pre_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_UL_PRE_BUF);
    eplfrm->ul_pos_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_UL_POS_BUF);
    eplfrm->dl_pre_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_DL_PRE_BUF);
    eplfrm->dl_pos_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_DL_POS_BUF);
    eplfrm->ul2_pos_buf = DSP_DM_ADDR(5, *DSP_DM_ADDR(6, DSP_SPH_SE2_PTR));

    eplfrm->ul_pre_len  = *DSP_SPH_EPL_BND & 0x0001 ? 320 : 160;
    eplfrm->ul_pos_len  = *DSP_SPH_EPL_BND & 0x0002 ? 320 : 160;
    eplfrm->dl_pre_len  = *DSP_SPH_EPL_BND & 0x0004 ? 320 : 160;
    eplfrm->dl_pos_len  = *DSP_SPH_EPL_BND & 0x0008 ? 320 : 160;
    eplfrm->ul2_pos_len = *DSP_SPH_EPL_BND & 0x0010 ? 320 : 160;

    return;
}


void audio_vm_vmEplPcmRecordHisr(void *param)

{
    uint32_t I, tmp = 0;
    // uint16_t dummy;
    uint16_t *buf; // , *mBuf;
    uint16_t pcmsize = 0;
    uint16_t *pcmBuf;

    if (vm.state != VM_STATE_RECORD) {
        return;
    }

    // get the size
    SALI_EPL_GetFrame(&eplfrm_t);
    pcmsize += eplfrm_t.ul_pre_len + eplfrm_t.dl_pre_len;
    pcmsize += eplfrm_t.ul_pos_len + eplfrm_t.dl_pos_len;

    // for second mic
    // pcmsize += eplfrm_t.ul2_pos_len;

    // vm Buf header process
    buf = vm.vmBuf;
    if (buf[0] == VM_MAGIC_HEADER) {
        // Already buffer VM
        tmp = (buf[1] >> 3) + pcmsize;  // Debug size
        I = tmp + AA88_GET_VM_LENGTH(vm.control_1, vm.control_2);
    } else {
        tmp = pcmsize;          // Debug size(only PCM)
        I = pcmsize + 3 + 2;    // Plus sync word and format and 2 timestamp
    }
    I -= pcmsize; // vm header + en/de debugInfo

    buf[2] &= 0xfe0f;   // Clean the epl band flag
    if (eplfrm_t.ul_pre_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_UL_PRE;
    }

    if (eplfrm_t.ul_pos_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_UL_POS;
    }

    if (eplfrm_t.dl_pre_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_DL_PRE;
    }

    if (eplfrm_t.dl_pos_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_DL_POS;
    }

    if (eplfrm_t.ul2_pos_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_UL2_POS;
    }

    // EPL PCM buffer handling
    pcmBuf = vm.pcmBuf;

    buf[0] = VM_MAGIC_HEADER;
    buf[1] = (uint16_t)((SAL_VM_DBGINFO_LEN + SAL_VM_ENH_DBGINFO_LEN + SAL_VM_DRV_DBGINFO_LEN) << 3) | 0; // ( L1SP_GetState());
    buf[2] |= ( vm.debug_info & (VM_PCM_REFMIC_RECORD_FLAG + VM_PCM_1ST_SET_RECORD_FLAG + VM_PCM_2ND_SET_RECORD_FLAG) );

    vmWriteVmToMB(I, buf);

    // Header for record 1st set of UL-DL PCM data
    *pcmBuf++ = (vm.vm_lost_count << 8) + vm.pcm_lost_count;
    *pcmBuf++ = vm_counter;

    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.ul_pre_buf, eplfrm_t.ul_pre_len);	// Uplink
    pcmBuf += (eplfrm_t.ul_pre_len);
    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.dl_pre_buf, eplfrm_t.dl_pre_len);	// Downlink
    pcmBuf += (eplfrm_t.dl_pre_len);

    // Header for record 2nd set of UL-DL PCM data
    *pcmBuf++ = (vm.vm_lost_count << 8) + vm.pcm_lost_count;
    *pcmBuf++ = vm_counter;

    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.ul_pos_buf, eplfrm_t.ul_pos_len);	// Uplink
    pcmBuf += (eplfrm_t.ul_pos_len);
    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.dl_pos_buf, eplfrm_t.dl_pos_len);	// Downlink
    pcmBuf += (eplfrm_t.dl_pos_len);

    // Heade for second mic
    *pcmBuf++ = (vm.vm_lost_count << 8) + vm.pcm_lost_count;
    *pcmBuf++ = vm_counter ;

    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.ul2_pos_buf, eplfrm_t.ul2_pos_len);	// Uplink2
    pcmBuf += (eplfrm_t.ul2_pos_len);

    *pcmBuf++ = *DSP_SPH_AGC_SW_GAIN1;
    *pcmBuf++ = *DSP_SPH_AGC_SW_GAIN2;

    // write all
    vmWriteVmToMB((2 + eplfrm_t.ul_pre_len + eplfrm_t.dl_pre_len + 2 + eplfrm_t.ul_pos_len + eplfrm_t.dl_pos_len + 4 + eplfrm_t.ul2_pos_len), vm.pcmBuf);
    // vm4WayPcmRecord(eplfrm_t.ul_pre_buf, eplfrm_t.ul_pre_len, eplfrm_t.dl_pre_buf, eplfrm_t.dl_pre_len);
    // vm4WayPcmRecord(eplfrm_t.ul_pos_buf, eplfrm_t.ul_pos_len, eplfrm_t.dl_pos_buf, eplfrm_t.dl_pos_len);


    // Reset
    vm.vmBuf[0] = 0;
    vm.vmBuf[1] = 0;
    vm.vmBuf[2] = 0;
    vm.pcm_lost_count = 0;

    return;

}


// #pragma arm section


void audio_vm_Start(void)
{
#if VM_WRITE_TO_FILE
    FRESULT result;
    FATFS fatfs;
#endif

    log_hal_info("%s() enter", __func__);

   if (VM_STATE_RECORD == vm.state) { //already on
        return;
    }

    assert(true == afe_is_audio_enable());

    // vm.vm_hdlr = vm_hdlr;

#if VM_WRITE_TO_FILE
    // open file handler
    result = f_mount(&fatfs, "0:", 1);
    if ( !result ) {
        log_hal_info("fmount ok \n");
        result = f_open(&(vmFileHdl), "SD:/vm_dump.vm", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
        if (!result) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error \n");
            return;
        }
    } else {
        log_hal_info("fmount error \n");
        return;
    }
#endif

    vm.state = VM_STATE_RECORD;

    vm.vmBuf = vmCodecBuf;
    memset( vm.vmBuf, 0, sizeof(uint16_t)*VM_BUFFER_SIZE );

    vm.control = 0x0003;
    vm.control_1 = 0x0001;
    vm.control_2 = 0x0001;
   vm.debug_info = 0x0F;

    // config EPLs buffer
    vm.pcm_save_size = FIVE_PCM_SAVE_SIZE;
    memset(eplInputBuf, 0 , sizeof(uint16_t)*vm.pcm_save_size);
    vm.pcmBuf = eplInputBuf; //(int16*)get_ctrl_buffer( vm.pcm_save_size*sizeof(uint16_t) );
    vm.pcm_lost_count = 0;
    // vm.vm_lost  = 1; // to force save VM first
    vm.vm_lost_count = 0;

    // turn on and hook hisr
    audio_service_hook_isr( DSP_VMEPL_REC_INT, audio_vm_vmEplPcmRecordHisr, 0 );
    *DSP_SPH_PCM_REC_CTRL |= 0x0002;

    audio_service_hook_isr( DSP_VM_REC_INT, audio_vm_vmRecordHisr, 0 );
    *DSP_SPH_COD_CTRL |= 0x0800;
    *DSP_SPH_SCH_IMPROVE_CTRL |= 0x08;

    log_hal_info("%s() done", __func__);

}



void audio_vm_Stop(void)
{
#if VM_WRITE_TO_FILE
    FRESULT result;
#endif
    log_hal_info("%s() enter", __func__);

    if (VM_STATE_RECORD == vm.state) {
        vm.state = VM_STATE_RECORD_STOP;
    } else {
        log_hal_info("%s(): stop without start", __func__);
        return;
    }

    assert(true == afe_is_audio_enable());

    // turn off
    *DSP_SPH_PCM_REC_CTRL &= ~0x0002;
    audio_service_unhook_isr(DSP_VMEPL_REC_INT);

    *DSP_SPH_COD_CTRL &= ~0x0800;
    *DSP_SPH_SCH_IMPROVE_CTRL &= ~0x08;
    audio_service_unhook_isr(DSP_VM_REC_INT);

    // buffer clean
    vm.pcmBuf = NULL; // free_ctrl_buffer(vm.pcmBuf);
    vm.vmBuf = NULL;
    vm.debug_info = 0;

#if VM_WRITE_TO_FILE
    // close file
    result = f_close(&vmFileHdl);
    if (!result) {
        log_hal_info("fclose success \n");
    } else {
        log_hal_info("fclose error \n");
    }
#endif

    vm.state = VM_STATE_IDLE;

}

#endif  // defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)
