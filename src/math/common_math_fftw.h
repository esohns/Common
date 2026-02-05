#ifndef COMMON_MATH_FFT_HEADER_2
#define COMMON_MATH_FFT_HEADER_2

#include <cmath>
#include <vector>

// #include <complex.h>
// #undef howmany
#include "fftw3.h"

#include "ace/Global_Macros.h"

#include "common_math_fft.h"

//////////////////////////////////////////

template <>
class Common_Math_FFT_T<float,
                        FFT_ALGORITHM_FFTW>
{
 public:
  Common_Math_FFT_T (unsigned int,  // #channels
                     unsigned int,  // #buffered samples ('magnitude' of the buffer)
                     unsigned int); // source sample rate (Hz)
  virtual ~Common_Math_FFT_T ();

  // simply copy buffer to working set
  void Setup (unsigned int); // channel
  inline void Compute (unsigned int channel_in) { fftwf_execute (plans_[channel_in]); }

  typedef Common_Math_FFT_SampleIterator_T<float> ITERATOR_T;
  void CopyIn (unsigned int, // channel
               unsigned int, // #samples
               ITERATOR_T&); // sample iterator
  bool Initialize (unsigned int,  // #channels
                   unsigned int,  // #slots (must be a power of 2)
                   unsigned int); // sample rate (Hz)

  inline unsigned int Channels () const { return channels_; }
  inline unsigned int Slots () const { return slots_; }

  //inline ValueType SqModulus (unsigned int slot_in,
  //                            unsigned int channel_in)
  //{ ACE_ASSERT (X_);
  //  ACE_ASSERT (slot_in < slots_);
  //  ACE_ASSERT (channel_in < channels_);
  //  ValueType modulus =
  //    (slot_in ? std::abs (X_[channel_in][slot_in]) : 0);
  //  return modulus * modulus;
  //}
  inline float SqMagnitude (unsigned int slot_in,
                            unsigned int channel_in,
                            bool normalize_in)
  { ACE_ASSERT (Y_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);

    float real_f = Y_[channel_in][slot_in][0];
    float imag_f = Y_[channel_in][slot_in][1];
    return (normalize_in ? (real_f * real_f + imag_f * imag_f) / sqMaxValue_
                         : real_f * real_f + imag_f * imag_f);
  }
  inline float Magnitude (unsigned int slot_in,
                          unsigned int channel_in,
                          bool normalize_in = true) const
  { ACE_ASSERT (Y_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);

    float real_f = Y_[channel_in][slot_in][0];
    float imag_f = Y_[channel_in][slot_in][1];
    return (normalize_in ? std::sqrt (real_f * real_f + imag_f * imag_f) / maxValue_
                         : std::sqrt (real_f * real_f + imag_f * imag_f) / sqrtSlots_);
  }
  inline float Magnitude2 (unsigned int slot_in,
                           unsigned int channel_in,
                           bool normalize_in = true) const
  { ACE_ASSERT (Y_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);

    float real_f = Y_[channel_in][slot_in][0];
    float imag_f = Y_[channel_in][slot_in][1];
    return (normalize_in ? std::sqrt (real_f * real_f + imag_f * imag_f) / sqrtSlots_
                         : std::sqrt (real_f * real_f + imag_f * imag_f));
  }

  std::vector<float> Spectrum (int = -1,      // channel# (-1: all)
                               bool = false); // normalize values ?

  // return frequency in Hz of a given slot
  inline unsigned int Frequency (unsigned int slot_in) const
  { ACE_ASSERT (slot_in < slots_);
    return static_cast<unsigned int> (((sampleRate_ / 2) * slot_in) / static_cast<double> (slots_));
  }
  inline unsigned int MaxFrequency () const { return sampleRate_ / 2; }
  inline unsigned int HzToSlot (unsigned int frequency_in) const
  {
    return static_cast<unsigned int> ((slots_ * frequency_in) / (sampleRate_ / 2.0));
  }

  // *NOTE*: only required when retrieving normalized (!) magnitudes (see above)
  void ComputeMaxValue (int = -1); // channel# (-1: all)

 protected:
  bool            isInitialized_;
  float**         buffer_;        // sample data [/channel]
  fftwf_complex** X_;             // input working buffer [/channel]
  fftwf_complex** Y_;             // output working buffer [/channel]
  fftwf_plan*     plans_;

  unsigned int    channels_;      // #channels
  unsigned int    halfSlots_;     // #slots / 2
  unsigned int    slots_;         // #buffered samples / channel
  unsigned int    sampleRate_;
  float           maxValue_;      // only required for normalization (see above)
  float           sqMaxValue_;    // only required for normalization (see above)

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T (const Common_Math_FFT_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T& operator= (const Common_Math_FFT_T&))

  float           sqrtSlots_;     // sqrt (#slots)
};

// include template definition
#include "common_math_fftw.inl"

#endif
