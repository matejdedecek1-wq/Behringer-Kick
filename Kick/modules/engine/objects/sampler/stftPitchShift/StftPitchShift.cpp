#include "StftPitchShift.h"
#include "StftPitchShiftCore.h"

#include "FFT.h"
#include "RFFT.h"

#include "SDFT.h"
#include "STFT.h"

namespace stftpitchshift {

    StftPitchShift::StftPitchShift(
        const size_t framesize,
        const size_t hopsize,
        const double samplerate,
        const bool normalization,
        const bool chronometry ) :
        StftPitchShift( std::make_shared<RFFT>(), framesize, hopsize, samplerate, normalization, chronometry ) {}

    StftPitchShift::StftPitchShift(
        const std::shared_ptr<FFT> fft,
        const size_t framesize,
        const size_t hopsize,
        const double samplerate,
        const bool normalization,
        const bool chronometry ) :
        fft( fft ),
        framesize( framesize ),
        hopsize( hopsize ),
        samplerate( samplerate ),
        normalization( normalization ),
        chronometry( chronometry ) {
        pCore = std::make_unique<StftPitchShiftCore<float>>( fft, framesize, hopsize, samplerate );
    }

    void StftPitchShift::shiftpitch(
        const std::vector<float>& input,
        std::vector<float>& output,
        const std::vector<double>& factors,
        const double quefrency,
        const double distortion ) {
        shiftpitch(
            input.size(),
            input.data(),
            output.data(),
            factors,
            quefrency,
            distortion );
    }

    void StftPitchShift::shiftpitch(
        const size_t size,
        const float* input,
        float* const output,
        const std::vector<double>& factors,
        const double quefrency,
        const double distortion ) {

        pCore->factors( factors );
        pCore->quefrency( quefrency );
        pCore->distortion( distortion );
        pCore->normalization( normalization );

        if( hopsize == 1 ) {
            SDFT<float> sdft( framesize, /* latency */ 1, chronometry );

            sdft( size, input, output, [&]( std::vector<std::complex<float>>& dft ) {
                pCore->shiftpitch( dft );
                  } );
        } else {
            STFT<float> stft( fft, framesize, hopsize, chronometry );

            stft( size, input, output, [&]( std::vector<std::complex<float>>& dft ) {
                pCore->shiftpitch( dft );
                  } );
        }
    }

}