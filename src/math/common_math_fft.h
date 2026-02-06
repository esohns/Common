#ifndef COMMON_MATH_FFT_HEADER_H
#define COMMON_MATH_FFT_HEADER_H

#include <complex>
#include <iostream>
#include <valarray>
#include <vector>

#include "ace/Assert.h"
#include "ace/Global_Macros.h"

enum Common_Math_FFT_AlgorithmType
{
  FFT_ALGORITHM_INVALID = -1,
  ////////////////////////////////////////
  FFT_ALGORITHM_COOLEY_TUKEY,
  FFT_ALGORITHM_FFTW,
  FFT_ALGORITHM_UNKNOWN, // found this somewhere on the internet
  ////////////////////////////////////////
  FFT_ALGORITHM_MAX
};

//////////////////////////////////////////

template <typename ValueType> 
class Common_Math_FFT_SampleIterator_T
{
 public:
  Common_Math_FFT_SampleIterator_T (uint8_t*); // buffer
  inline virtual ~Common_Math_FFT_SampleIterator_T () {}

  bool initialize (unsigned int, // bytes / 'data sample' (i.e. sizeof ('sound sample') * channels)
                   unsigned int, // resolution: bytes per (mono-)'sound sample'
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
  int          sampleByteOrder_; // ACE-style, -1: N/A
};

//////////////////////////////////////////

template <typename ValueType,
          enum Common_Math_FFT_AlgorithmType AlgorithmType = FFT_ALGORITHM_UNKNOWN>
class Common_Math_FFT_T
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

  int                       logSlots_;
  ValueType                 sqrtSlots_;     // sqrt (#slots)
  std::complex<ValueType>** W_;             // exponentials
};

//////////////////////////////////////////
// specializations

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

  void fft (std::valarray<std::complex<ValueType> >&); // (complex) values

  ValueType                                sqrtSlots_;     // sqrt (#slots)
};

template <typename ValueType>
class Common_Math_FFT_T<ValueType,
                        FFT_ALGORITHM_UNKNOWN>
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

  int                       logSlots_;
  ValueType                 sqrtSlots_;     // sqrt (#slots)
  std::complex<ValueType>** W_;             // exponentials
};

//////////////////////////////////////////

// include template definition
#include "common_math_fft.inl"

#endif
