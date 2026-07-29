#include <cmath>

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"

template <typename ValueType>
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::Common_Math_FFT_T (unsigned int channels_in,
                                                              unsigned int slots_in,
                                                              unsigned int sampleRate_in,
                                                              bool equalize_in,
                                                              ValueType envelopePower_in)
 : isInitialized_ (false)
 , buffer_ ()
 , X_ ()
 , channels_ (channels_in)
 , halfSlots_ (slots_in / 2)
 , slots_ (slots_in)
 , sampleRate_ (sampleRate_in)
 , maxValue_ (0.0)
 , sqMaxValue_ (0.0)
 /////////////////////////////////////////
 , bitRevTable_ ()
 , envelope_ ()
 , equalize_ ()
 , cosSinTable_ ()
 , sqrtSlots_ (0.0)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Common_Math_FFT_T"));

  isInitialized_ = Initialize (channels_in,
                               slots_in,
                               sampleRate_in,
                               equalize_in,
                               envelopePower_in);
}

template <typename ValueType>
bool
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::Initialize (unsigned int channels_in,
                                                       unsigned int slots_in,
                                                       unsigned int sampleRate_in,
                                                       bool equalize_in,
                                                       ValueType envelopePower_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Initialize"));

  // sanity check(s)
  ACE_ASSERT (channels_in > 0);
  ACE_ASSERT (slots_in > 0 && (slots_in % 2) == 0);
  ACE_ASSERT (sampleRate_in);

  if (isInitialized_)
  {
    isInitialized_ = false;

    for (unsigned int i = 0; i < channels_; ++i)
      buffer_[i].clear ();
    buffer_.clear ();
    for (unsigned int i = 0; i < channels_; ++i)
      X_[i].clear ();
    X_.clear ();

    channels_ = 0;
    halfSlots_ = 0;
    slots_ = 0;
    sampleRate_ = 0;
    maxValue_ = 0.0;
    sqMaxValue_ = 0.0;

    bitRevTable_.clear ();
    envelope_.clear ();
    equalize_.clear ();
    cosSinTable_.clear ();

    sqrtSlots_ = 0.0;
  } // end IF

  buffer_.resize (channels_in);
  for (unsigned int i = 0; i < channels_in; ++i)
    buffer_[i].resize (slots_in, 0.0);
  X_.resize (channels_in);
  for (unsigned int i = 0; i < channels_in; ++i)
    X_[i].resize (slots_in, std::complex<ValueType> (0.0, 0.0));

  channels_ = channels_in;
  halfSlots_ = slots_in / 2;
  slots_ = slots_in;
  sampleRate_ = sampleRate_in;

  InitBitRevTable ();
  InitCosSinTable ();
  InitEnvelopeTable (envelopePower_in);
  InitEqualizeTable (equalize_in);

  sqrtSlots_ = std::sqrt (static_cast<ValueType> (slots_));

  isInitialized_ = true;

  return true;
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::CopyIn (unsigned int channel_in,
                                                   unsigned int samples_in,
                                                   ITERATOR_T& iterator_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::CopyIn"));

  // sanity check(s)
  ACE_ASSERT (channel_in > 0);
  ACE_ASSERT (samples_in <= slots_);

  // make space for inbound samples at the end of the buffer,
  // shifting previous samples towards the beginning
  ACE_OS::memmove (buffer_[channel_in].data (), &buffer_[channel_in][samples_in],
                   (slots_ - samples_in) * sizeof (ValueType));

  // copy the sample data to the tail end of the buffer
  int tail_slot = slots_ - samples_in;
  for (int i = 0; i < samples_in; ++i)
    buffer_[channel_in][tail_slot + i] = iterator_in.get (i, channel_in);
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::Setup (unsigned int channel_in)
{
  // COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Setup"));

  size_t index;
  for (unsigned int i = 0; i < slots_; i++)
  {
    index = bitRevTable_[i];
    X_[channel_in][i] =
      std::complex<ValueType> (buffer_[channel_in][index] * envelope_[index], static_cast<ValueType> (0.0));
  } // end FOR
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::Compute (unsigned int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Compute"));

  size_t dftSize = 2;
  size_t octave = 0;

  while (dftSize <= slots_)
  {
    std::complex<ValueType> w (static_cast<ValueType> (1.0), static_cast<ValueType> (0.0));
    std::complex<ValueType> wp = cosSinTable_[octave];
    size_t hdftsize = dftSize >> 1;

    for (size_t m = 0; m < hdftsize; m += 1)
    {
      for (size_t i = m; i < slots_; i += dftSize)
      {
        size_t j = i + hdftsize;
        std::complex<ValueType> tempNum = X_[channel_in][j] * w;
        X_[channel_in][j] = X_[channel_in][i] - tempNum;
        X_[channel_in][i] = X_[channel_in][i] + tempNum;
      } // end FOR

      w *= wp;
    } // end FOR

    dftSize <<= 1;
    octave++;
  } // end WHILE
}

template <typename ValueType>
std::vector<ValueType>
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::Spectrum (int channel_in,
                                                     bool normalize_in)
{
  // COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Spectrum"));

  // sanity check(s)
  ACE_ASSERT (normalize_in == false);

  std::vector<ValueType> result_a;

  // sanity check(s)
  if (unlikely (!isInitialized_))
  {
    result_a.resize (halfSlots_ - 1, 0);
    return result_a;
  } // end IF

  //if (unlikely (normalize_in))
  //  ComputeMaxValue (-1);

  result_a.resize (halfSlots_ - 1);
  ValueType value;
  if (likely (channel_in == -1))
  {
    for (size_t i = 1; i < halfSlots_; i++)
    {
      value = equalize_[i - 1] * std::abs (X_[0][i]);
      for (size_t j = 1; j < channels_; ++j)
        value += equalize_[i - 1] * std::abs (X_[j][i]);
      value /= static_cast<ValueType> (channels_);
      result_a[i - 1] = value;
    } // end FOR
  } // end IF
  else
  { ACE_ASSERT (channel_in < channels_);
    for (size_t i = 1; i < halfSlots_; ++i)
    {
      value = equalize_[i - 1] * std::abs (X_[channel_in][i]);
      result_a[i - 1] = value;
    } // end FOR
  } // end ELSE

  //if (unlikely (normalize_in))
  //  for (typename std::vector<ValueType>::iterator iterator = result_a.begin ();
  //       iterator != result_a.end ();
  //       ++iterator)
  //    *iterator /= maxValue_;

  return result_a;
}

//template <typename ValueType>
//void
//Common_Math_FFT_T<ValueType,
//                  FFT_ALGORITHM_MILKDROP>::ComputeMaxValue (int channel_in)
//{
//  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::ComputeMaxValue"));
//
//  // sanity check(s)
//  ACE_ASSERT (X_);
//  ACE_ASSERT (channel_in < static_cast<int> (channels_));
//
//  ValueType temp = 0.0;
//
//  if (likely (channel_in == -1))
//  {
//    for (unsigned int j = 0; j < channels_; ++j)
//      for (unsigned int i = 1; i < halfSlots_; ++i)
//      {
//        ValueType magnitude = std::sqrt (std::norm (X_[j][i]));
//        temp = std::max (temp, magnitude);
//      } // end FOR
//  } // end IF
//  else
//  {
//    for (unsigned int i = 1; i < halfSlots_; ++i)
//    {
//      ValueType magnitude = std::sqrt (std::norm (X_[channel_in][i]));
//      temp = std::max (temp, magnitude);
//    } // end FOR
//  } // end ELSE
//
//  maxValue_ = temp;
//  sqMaxValue_ = maxValue_ * maxValue_;
//}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::InitEnvelopeTable (ValueType envelopePower_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::InitEnvelopeTable"));

  if (envelopePower_in < static_cast<ValueType> (0.0))
  {
    envelope_.assign (slots_, static_cast<ValueType> (1.0));
    return;
  } // end IF

  ValueType const multiplier =
    static_cast<ValueType> (1.0) / static_cast<ValueType> (slots_) * static_cast<ValueType> (2.0 * M_PI);

  envelope_.resize (slots_);
  if (envelopePower_in == static_cast<ValueType> (1.0))
  {
    for (size_t i = 0; i < slots_; i++)
    {
      envelope_[i] =
        static_cast<ValueType> (0.5) +
        static_cast<ValueType> (0.5) * std::sin (static_cast<ValueType> (i) * multiplier - static_cast<ValueType> (M_PI * 0.5));
    } // end FOR
  } // end IF
  else
  {
    for (size_t i = 0; i < slots_; i++)
    {
      envelope_[i] =
        std::pow (static_cast<ValueType> (0.5) + static_cast<ValueType> (0.5) * std::sin (static_cast<ValueType> (i) * multiplier - static_cast<ValueType> (M_PI * 0.5)),
                  envelopePower_in);
    } // end FOR
  } // end ELSE
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::InitEqualizeTable (bool equalize_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::InitEqualizeTable"));

  if (!equalize_in)
  {
    equalize_.assign (halfSlots_ - 1, static_cast<ValueType> (1.0));
    return;
  } // end IF

  //ValueType const scaling = static_cast<ValueType> (-0.02);
  ValueType const scaling = static_cast<ValueType> (-1.0);
  ValueType const inverseHalfNumFrequencies =
    static_cast<ValueType> (1.0) / static_cast<ValueType> (halfSlots_ - 1);

  equalize_.resize (halfSlots_ - 1);
  for (size_t i = 0; i < halfSlots_ - 1; i++)
    equalize_[i] = scaling * std::log (static_cast<ValueType> ((halfSlots_ - 1) - i) * inverseHalfNumFrequencies);
}

template <typename ValueType> 
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::InitBitRevTable ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::InitBitRevTable"));

  bitRevTable_.resize (slots_);

  for (size_t i = 0; i < slots_; i++)
    bitRevTable_[i] = i;

  size_t j = 0, m, temp;
  for (size_t i = 0; i < slots_; i++)
  {
    if (j > i)
    {
      temp = bitRevTable_[i];
      bitRevTable_[i] = bitRevTable_[j];
      bitRevTable_[j] = temp;
    } // end IF

    m = slots_ >> 1;
    while (m >= 1 && j >= m)
    {
      j -= m;
      m >>= 1;
    } // end WHILE

    j += m;
  } // end FOR
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_MILKDROP>::InitCosSinTable ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::InitCosSinTable"));

  size_t tabsize = 0;
  size_t dftsize = 2;

  while (dftsize <= slots_)
  {
    tabsize++;
    dftsize <<= 1;
  } // end WHILE

  cosSinTable_.resize (tabsize);

  dftsize = 2;
  size_t index = 0;
  while (dftsize <= slots_)
  {
    ValueType theta =
      static_cast<ValueType> (-2.0 * M_PI) / static_cast<ValueType> (dftsize);
    cosSinTable_[index] = std::polar (static_cast<ValueType> (1.0), theta);
    index++;
    dftsize <<= 1;
  } // end WHILE
}
