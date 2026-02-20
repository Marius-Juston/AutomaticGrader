//
// Created by Marius on 2/20/2026.
//

#ifndef AUTOMATICGRADER_FPU_RFFT_H
#define AUTOMATICGRADER_FPU_RFFT_H

typedef struct {
    float *InBuf; //!< Pointer to the input buffer
    float *OutBuf; //!< Pointer to the output buffer
    float *CosSinBuf; //!< Pointer to the twiddle factors
    float *MagBuf; //!< Pointer to the magnitude buffer
    float *PhaseBuf; //!< Pointer to the phase buffer
    uint16_t FFTSize; //!< Size of the FFT (number of real data points)
    uint16_t FFTStages; //!< Number of FFT stages
} RFFT_F32_STRUCT;

typedef RFFT_F32_STRUCT *RFFT_F32_STRUCT_Handle;


static inline void RFFT_f32_mag_TMU0(RFFT_F32_STRUCT_Handle hndRFFT_F32) {
}

static inline void RFFT_f32_sincostable(RFFT_F32_STRUCT_Handle hndRFFT_F32) {
}

static inline void RFFT_f32(RFFT_F32_STRUCT_Handle hndRFFT_F32) {
}

static inline void RFFT_f32_mag(RFFT_F32_STRUCT_Handle hndRFFT_F32) {
}

#endif //AUTOMATICGRADER_FPU_RFFT_H