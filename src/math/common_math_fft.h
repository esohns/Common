#ifndef COMMON_MATH_FFT_H
#define COMMON_MATH_FFT_H

#include <complex>

#include "ace/Assert.h"
#include "ace/Global_Macros.h"

class Common_Math_FFT_SampleIterator
{
 public:
  Common_Math_FFT_SampleIterator (uint8_t*); // buffer
  inline virtual ~Common_Math_FFT_SampleIterator () {}

  bool initialize (unsigned int, // bytes / 'data sample' (i.e. sizeof ('sound sample') * channels)
                   unsigned int, // resolution: bytes per 'sound sample'
                   bool,         // signed 'sound sample' format ?
                   bool,         // floating point format ? : integer format
                   int);         // 'sound sample' byte order (ACE-style, 0: N/A)
  double get (unsigned int,  // index (i.e. #sample into buffer)
              unsigned int); // channel index (i.e. 0: mono/stereo left,
                             //                     1: stereo right, ...)

  bool         isInitialized_;
  uint8_t*     buffer_;
  unsigned int dataSampleSize_; // soundSampleSize_ * #channels
  bool         isSignedSampleFormat_;
  bool         reverseEndianness_;
  unsigned int soundSampleSize_; // mono-

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_SampleIterator ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_SampleIterator (const Common_Math_FFT_SampleIterator&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_SampleIterator& operator= (const Common_Math_FFT_SampleIterator&))

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

  inline double       Magnitude (unsigned int slot_in,
                                 unsigned int channel_in,
                                 bool normalize_in = true) const
  { ACE_ASSERT (slot_in < slots_); ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? (slot_in ? std::sqrt (std::norm (X_[channel_in][slot_in])) 
                                    : 0.0) * (2.0 / static_cast<double> (slots_))
                         : (slot_in ? std::sqrt (std::norm (X_[channel_in][slot_in]))
                                    : 0.0));
  }
  //inline int          Value (unsigned int slot_in,
  //                           unsigned int channel_in) const
  //{ ACE_ASSERT (slot_in < slots_); ACE_ASSERT (channel_in < channels_);
  //  return static_cast<int> (buffer_[channel_in][slot_in]);
  //}

  // return frequency in Hz of a given slot
  inline unsigned int Frequency (unsigned int slot_in) const
  { ACE_ASSERT (slot_in < slots_);
    return static_cast<unsigned int> (((sampleRate_ / 2) * slot_in) / static_cast<double> (slots_));
  }
  inline unsigned int MaxFrequency () const { return sampleRate_ / 2; }
  inline unsigned int HzToSlot (unsigned int frequency_in) const
  {
    return static_cast<unsigned int> ((slots_ * frequency_in) / static_cast<double> (sampleRate_ / 2));
  }

 protected:
  bool                      isInitialized_;
  ValueType**               buffer_;        // sample data [/channel]
  std::complex<ValueType>** X_;             // 'in-place' working buffer [/channel]

  int*                      bitReverseMap_; // bit-reverse vector mapping
  unsigned int              channels_;      // #channels
  unsigned int              halfSlots_;     // #slots / 2
  unsigned int              slots_;         // #buffered samples / channel
  unsigned int              sampleRate_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T (const Common_Math_FFT_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T& operator= (const Common_Math_FFT_T&))

  int                       logSlots_;
  double                    sqrtSlots_;
  std::complex<ValueType>** W_;             // exponentials
};

// include template definition
#include "common_math_fft.inl"

#endif
