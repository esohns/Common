#ifndef COMMON_MATH_FFT_H
#define COMMON_MATH_FFT_H

#include <complex>
#include <vector>

#include "ace/Assert.h"
#include "ace/Global_Macros.h"

template <typename ValueType> 
class Common_Math_FFT_SampleIterator_T
{
 public:
  Common_Math_FFT_SampleIterator_T (uint8_t*); // buffer
  inline virtual ~Common_Math_FFT_SampleIterator_T () {}

  bool initialize (unsigned int, // bytes / 'data sample' (i.e. sizeof ('sound sample') * channels)
                   unsigned int, // resolution: bytes per 'sound sample'
                   bool,         // signed 'sound sample' format ?
                   bool,         // floating point format ? : integer format
                   int);         // 'sound sample' byte order (ACE-style, 0: N/A)
  ValueType get (unsigned int,  // index (i.e. #sample into buffer)
                 unsigned int); // channel index (i.e. 0: mono/stereo left,
                                //                     1: stereo right, ...)

  bool         isInitialized_;
  uint8_t*     buffer_;
  unsigned int dataSampleSize_; // soundSampleSize_ * #channels
  bool         isSignedSampleFormat_;
  bool         reverseEndianness_;
  unsigned int soundSampleSize_; // mono-

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_SampleIterator_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_SampleIterator_T (const Common_Math_FFT_SampleIterator_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_SampleIterator_T& operator= (const Common_Math_FFT_SampleIterator_T&))

  bool         isFloatingPointFormat_;
  int          sampleByteOrder_; // ACE-style, 0: N/A
};

//////////////////////////////////////////

template <typename ValueType>
class Common_Math_FFT_T
{
 public:
  Common_Math_FFT_T (unsigned int,  // #channels
                     unsigned int,  // #slots (must be a power of 2)
                     unsigned int); // sample rate (Hz)
  virtual ~Common_Math_FFT_T ();

  void Compute (unsigned int); // channel
  //void CopyIn (unsigned int, // channel
  //             ITERATOR_T*); // sample iterator
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
    ACE_ASSERT (slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? (slot_in ? std::norm (X_[channel_in][slot_in]) 
                                    : 0.0) / (sqMaxValue_ ? sqMaxValue_ : 1.0)
                         : (slot_in ? std::norm (X_[channel_in][slot_in])
                                    : 0.0));
  }
  inline ValueType Magnitude (unsigned int slot_in,
                              unsigned int channel_in,
                              bool normalize_in = true) const
  { ACE_ASSERT (X_);
    ACE_ASSERT (slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? (slot_in ? std::sqrt (std::norm (X_[channel_in][slot_in])) 
                                    : 0.0) / (maxValue_ ? maxValue_ : 1.0)
                         : (slot_in ? std::sqrt (std::norm (X_[channel_in][slot_in]))
                                    : 0.0));
  }
  inline ValueType Magnitude2 (unsigned int slot_in,
                               unsigned int channel_in,
                               bool normalize_in = true) const
  { ACE_ASSERT (X_);
    ACE_ASSERT (slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? (slot_in ? std::sqrt (std::norm (X_[channel_in][slot_in]))
                                    : 0.0) / (2.0 * slots_)
                         : (slot_in ? std::sqrt (std::norm (X_[channel_in][slot_in]))
                                    : 0.0));
  }

  std::vector<ValueType> Spectrum () const
  {
    std::vector<ValueType> result_a;

    // sanity check(s)
    if (unlikely (!isInitialized_))
      return result_a;

    // *IMPORTANT NOTE*: - the first ('DC'-)slot does not contain frequency
    //                     information --> i = 1
    //                   - the slots N/2 - N are mirrored and do not contain
    //                     additional information
    //                     --> there are only N/2 - 1 meaningful values
    for (int i = 1; i < slots_ / 2; ++i)
    {
      ValueType value_f = std::sqrt (std::norm (X_[0][i]));
      for (int j = 1; j < channels_; ++j)
        value_f += std::sqrt (std::norm (X_[j][i]));
      value_f /= static_cast<ValueType> (channels_);
      result_a.push_back (value_f);
    } // end FOR
    return result_a;
  }

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
  void ComputeMaxValue (); // -of the working buffer X_

 protected:
  bool                      isInitialized_;
  ValueType**               buffer_;        // sample data [/channel]
  std::complex<ValueType>** X_;             // 'in-place' working buffer [/channel]

  int*                      bitReverseMap_; // bit-reverse vector mapping
  unsigned int              channels_;      // #channels
  unsigned int              halfSlots_;     // #slots / 2
  unsigned int              slots_;         // #buffered samples / channel
  unsigned int              sampleRate_;
  ValueType                 maxValue_;      // only required for normalization (see above)
  ValueType                 sqMaxValue_;    // only required for normalization (see above)

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T (const Common_Math_FFT_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T& operator= (const Common_Math_FFT_T&))

  void ApplyHammingWindow (unsigned int); // channel

  int                       logSlots_;
  double                    sqrtSlots_;
  std::complex<ValueType>** W_;             // exponentials
};

// include template definition
#include "common_math_fft.inl"

/////////////////////////////////////////

typedef Common_Math_FFT_T<float> Common_Math_FFT_Float_t;
typedef Common_Math_FFT_T<double> Common_Math_FFT_Double_t;

#endif
