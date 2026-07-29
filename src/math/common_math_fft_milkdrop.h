/*
  LICENSE
  -------
Copyright 2005-2013 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Nullsoft nor the names of its contributors may be used
to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef COMMON_MATH_FFT_MILKDROP_H
#define COMMON_MATH_FFT_MILKDROP_H

#include <complex>
#include <vector>

#include "ace/Assert.h"
#include "ace/Global_Macros.h"

#include "common_math_fft.h"

//////////////////////////////////////////
// specialization

template <typename ValueType>
class Common_Math_FFT_T<ValueType,
                        FFT_ALGORITHM_MILKDROP>
{
 public:
  Common_Math_FFT_T (unsigned int,     // #channels
                     unsigned int,     // #slots (must be a power of 2)
                     unsigned int,     // sample rate (Hz)
                     bool = true,      // equalize ?
                     ValueType = 1.0); // envelope power
  inline virtual ~Common_Math_FFT_T () {}

  // simply copy buffer to working set
  void Setup (unsigned int); // channel
  void Compute (unsigned int); // channel

  typedef Common_Math_FFT_SampleIterator_T<ValueType> ITERATOR_T;
  void CopyIn (unsigned int, // channel
               unsigned int, // #samples
               ITERATOR_T&); // sample iterator
  bool Initialize (unsigned int,     // #channels
                   unsigned int,     // #slots (must be a power of 2)
                   unsigned int,     // sample rate (Hz)
                   bool = true,      // equalize ?
                   ValueType = 1.0); // envelope power

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
  { ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? std::norm (X_[channel_in][slot_in]) / sqMaxValue_
                         : std::norm (X_[channel_in][slot_in]));
  }
  inline ValueType Magnitude (unsigned int slot_in,
                              unsigned int channel_in,
                              bool normalize_in = true) const
  { ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? std::sqrt (std::norm (X_[channel_in][slot_in])) / maxValue_
                         : std::sqrt (std::norm (X_[channel_in][slot_in])) / sqrtSlots_);
  }
  inline ValueType Magnitude2 (unsigned int slot_in,
                               unsigned int channel_in,
                               bool normalize_in = true) const
  { ACE_ASSERT (slot_in > 0 && slot_in < slots_);
    ACE_ASSERT (channel_in < channels_);
    return (normalize_in ? equalize_[slot_in - 1] * std::abs (X_[channel_in][slot_in]) / sqrtSlots_
                         : equalize_[slot_in - 1] * std::abs (X_[channel_in][slot_in]));
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
  inline void ComputeMaxValue (int = -1) {} // channel# (-1: all)

 protected:
  bool                                                isInitialized_;

  std::vector<std::vector<ValueType> >                buffer_; // sample data [/channel]
  std::vector<std::vector<std::complex<ValueType> > > X_; // 'in-place' working buffer [/channel]

  unsigned int                                        channels_;      // #channels
  unsigned int                                        halfSlots_;     // #slots / 2
  unsigned int                                        slots_;         // #buffered samples / channel
  unsigned int                                        sampleRate_;
  ValueType                                           maxValue_;      // only required for normalization (see above)
  ValueType                                           sqMaxValue_;    // only required for normalization (see above)

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T ())
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T (const Common_Math_FFT_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_FFT_T& operator= (const Common_Math_FFT_T&))

  // helper methods
  void InitEnvelopeTable (ValueType); // power
  void InitEqualizeTable (bool); // equalize ?
  void InitBitRevTable ();
  void InitCosSinTable ();

  std::vector<size_t>                                 bitRevTable_;
  std::vector<ValueType>                              envelope_;
  std::vector<ValueType>                              equalize_;
  std::vector<std::complex<ValueType> >               cosSinTable_;

  ValueType                                           sqrtSlots_; // sqrt (#slots)
};

//////////////////////////////////////////

// include template definition
#include "common_math_fft_milkdrop.inl"

#endif
