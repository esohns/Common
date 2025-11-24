#ifndef COMMON_MATH_FFTW_T
#define COMMON_MATH_FFTW_T

#include <cmath>
#include <vector>

// #include <complex.h>
// #undef howmany
#include "fftw3.h"

#include "ace/Global_Macros.h"

// *TODO*: 'macroize' this code. Data access should really have (nearly) zero
//         overhead. If that is not possible, sub-class ASAP
template <typename ValueType>
class Common_Math_FFTW_SampleIterator_T
{
 public:
  Common_Math_FFTW_SampleIterator_T (uint8_t*); // buffer
  inline virtual ~Common_Math_FFTW_SampleIterator_T () {}

  // *TODO*: needs more serious consideration
  bool initialize (unsigned int, // bytes / 'data sample' (i.e. sizeof ('sound sample') * channels)
                   unsigned int, // resolution: bytes per 'sound sample'
                   bool,         // signed 'sound sample' format ?
                   bool,         // floating point format ? : integer format
                   int);         // 'sound sample' byte order (ACE-style, 0: N/A)
  ValueType get (unsigned int,  // index (i.e. #sample into buffer)
                 unsigned int); // channel# (e.g. 0: mono/stereo left,
                                //                1: stereo right, ...)

  bool         isInitialized_;
  uint8_t*     buffer_;
  unsigned int dataSampleSize_; // soundSampleSize_ * #channels
  bool         isSignedSampleFormat_;
  bool         reverseEndianness_;
  unsigned int soundSampleSize_; // mono-

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFTW_SampleIterator_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFTW_SampleIterator_T (const Common_Math_FFTW_SampleIterator_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFTW_SampleIterator_T& operator= (const Common_Math_FFTW_SampleIterator_T&))

  bool         isFloatingPointFormat_;
  int          sampleByteOrder_; // ACE-style, -1: N/A
};

//////////////////////////////////////////

template <typename ValueType>
class Common_Math_FFTW_T
{
 public:
  Common_Math_FFTW_T (unsigned int,  // #channels
                      unsigned int,  // #buffered samples ('magnitude' of the buffer)
                      unsigned int); // source sample rate (Hz)
  virtual ~Common_Math_FFTW_T ();

  inline void Compute (unsigned int channel_in) { fftwf_execute (plans_[channel_in]); }

  typedef Common_Math_FFTW_SampleIterator_T<ValueType> ITERATOR_T;
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
  inline ValueType SqMagnitude (unsigned int slot_in,
                                unsigned int channel_in,
                                bool normalize_in)
  { ACE_ASSERT (Y_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);

    ValueType real_f = Y_[channel_in][slot_in][0];
    ValueType imag_f = Y_[channel_in][slot_in][1];
    return (normalize_in ? (real_f * real_f + imag_f * imag_f) / sqMaxValue_
                         : real_f * real_f + imag_f * imag_f);
  }
  inline ValueType Magnitude (unsigned int slot_in,
                              unsigned int channel_in,
                              bool normalize_in = true) const
  { ACE_ASSERT (Y_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);

    ValueType real_f = Y_[channel_in][slot_in][0];
    ValueType imag_f = Y_[channel_in][slot_in][1];
    return (normalize_in ? std::sqrt (real_f * real_f + imag_f * imag_f) / maxValue_
                         : std::sqrt (real_f * real_f + imag_f * imag_f) / sqrtSlots_);
  }
  inline ValueType Magnitude2 (unsigned int slot_in,
                               unsigned int channel_in,
                               bool normalize_in = true) const
  { ACE_ASSERT (Y_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);

    ValueType real_f = Y_[channel_in][slot_in][0];
    ValueType imag_f = Y_[channel_in][slot_in][1];
    return (normalize_in ? std::sqrt (real_f * real_f + imag_f * imag_f) / sqrtSlots_
                         : std::sqrt (real_f * real_f + imag_f * imag_f));
  }

  std::vector<ValueType> Spectrum (int = -1,      // channel# (-1: all)
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
  bool                      isInitialized_;
  ValueType**               buffer_;        // sample data [/channel]
  fftwf_complex**           X_;             // input working buffer [/channel]
  fftwf_complex**           Y_;             // output working buffer [/channel]
  fftwf_plan*               plans_;

  unsigned int              channels_;      // #channels
  unsigned int              halfSlots_;     // #slots / 2
  unsigned int              slots_;         // #buffered samples / channel
  unsigned int              sampleRate_;
  ValueType                 maxValue_;      // only required for normalization (see above)
  ValueType                 sqMaxValue_;    // only required for normalization (see above)

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFTW_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFTW_T (const Common_Math_FFTW_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFTW_T& operator= (const Common_Math_FFTW_T&))

  void ApplyHammingWindow (unsigned int); // channel

  ValueType                 sqrtSlots_; // sqrt (#slots)
};

// include template definition
#include "common_math_fftw.inl"

#endif
