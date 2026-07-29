#ifndef COMMON_MATH_FFT_COOLEY_TUKEY_H
#define COMMON_MATH_FFT_COOLEY_TUKEY_H

#include <complex>
#include <valarray>

#include "ace/Assert.h"
#include "ace/Global_Macros.h"

#include "common_math_fft.h"

//////////////////////////////////////////
// specialization

template <typename ValueType>
class Common_Math_FFT_T<ValueType,
                        FFT_ALGORITHM_COOLEY_TUKEY>
{
 public:
  Common_Math_FFT_T (unsigned int,  // #channels
                     unsigned int,  // #slots (must be a power of 2)
                     unsigned int); // sample rate (Hz)
  virtual ~Common_Math_FFT_T ();

  // simply copy buffer to working set
  void Setup (unsigned int); // channel
  void Compute (unsigned int); // channel

  typedef Common_Math_FFT_SampleIterator_T<ValueType> ITERATOR_T;
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
  { ACE_ASSERT (X_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? std::norm (X_[channel_in][slot_in]) / sqMaxValue_
                         : std::norm (X_[channel_in][slot_in]));
  }
  inline ValueType Magnitude (unsigned int slot_in,
                              unsigned int channel_in,
                              bool normalize_in = true) const
  { ACE_ASSERT (X_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? std::sqrt (std::norm (X_[channel_in][slot_in])) / maxValue_
                         : std::sqrt (std::norm (X_[channel_in][slot_in])) / sqrtSlots_);
  }
  inline ValueType Magnitude2 (unsigned int slot_in,
                               unsigned int channel_in,
                               bool normalize_in = true) const
  { ACE_ASSERT (X_);
    ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? std::sqrt (std::norm (X_[channel_in][slot_in])) / sqrtSlots_
                         : std::sqrt (std::norm (X_[channel_in][slot_in])));
  }

  std::vector<ValueType> Spectrum (int = -1,      // channel# (-1: all)
                                   bool = false); // normalize values ?

  // return frequency in Hz of a given slot
  inline unsigned int Frequency (unsigned int slot_in) const
  { ACE_ASSERT (slot_in < slots_);
    return static_cast<unsigned int> (((sampleRate_ / 2) * slot_in) / static_cast<float> (slots_));
  }
  inline unsigned int MaxFrequency () const { return sampleRate_ / 2; }
  inline unsigned int HzToSlot (unsigned int frequency_in) const
  {
    return static_cast<unsigned int> ((slots_ * frequency_in) / (sampleRate_ / 2.0f));
  }

  // *NOTE*: only required when retrieving normalized (!) magnitudes (see above)
  void ComputeMaxValue (int = -1); // channel# (-1: all)

 protected:
  bool                                     isInitialized_;
  ValueType**                              buffer_;        // sample data [/channel]
  std::valarray<std::complex<ValueType> >* X_;             // 'in-place' working buffer [/channel]

  unsigned int                             channels_;      // #channels
  unsigned int                             halfSlots_;     // #slots / 2
  unsigned int                             slots_;         // #buffered samples / channel
  unsigned int                             sampleRate_;
  ValueType                                maxValue_;      // only required for normalization (see above)
  ValueType                                sqMaxValue_;    // only required for normalization (see above)

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T (const Common_Math_FFT_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T& operator= (const Common_Math_FFT_T&))

  //void fft (std::valarray<std::complex<ValueType> >&); // (complex) values

  ValueType                                sqrtSlots_;     // sqrt (#slots)
};

//////////////////////////////////////////

// include template definition
#include "common_math_fft_cooley_tukey.inl"

#endif
