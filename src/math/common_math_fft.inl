#include <cmath>

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"

template <typename ValueType>
Common_Math_FFT_SampleIterator_T<ValueType>::Common_Math_FFT_SampleIterator_T (uint8_t* buffer_in)
 : isInitialized_ (false)
 , buffer_ (buffer_in)
 , dataSampleSize_ (0)
 , isSignedSampleFormat_ (true)
 , reverseEndianness_ (false)
 , soundSampleSize_ (0)
 /////////////////////////////////////////
 , isFloatingPointFormat_ (false)
 , sampleByteOrder_ (ACE_BYTE_ORDER)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_SampleIterator_T::Common_Math_FFT_SampleIterator_T"));

}

template <typename ValueType>
ValueType
Common_Math_FFT_SampleIterator_T<ValueType>::get (unsigned int index_in,
                                                  unsigned int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_SampleIterator_T::get"));

  // sanity check(s)
  ACE_ASSERT (isInitialized_);

  switch (soundSampleSize_)
  {
    case 1: // --> data is single-byte (possibly non-integer)
      return buffer_[(index_in * dataSampleSize_) + channel_in];
    case 2:
      return (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_WORD (*reinterpret_cast<int16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                          : ACE_SWAP_WORD (*reinterpret_cast<uint16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                 : (isSignedSampleFormat_ ? *reinterpret_cast<int16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                          : *reinterpret_cast<uint16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])));
    case 4:
    { ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
      return (isFloatingPointFormat_ ? *reinterpret_cast<float*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                     : (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_LONG (*reinterpret_cast<int32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                                                    : ACE_SWAP_LONG (*reinterpret_cast<uint32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                                           : (isSignedSampleFormat_ ? *reinterpret_cast<int32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                                                    : *reinterpret_cast<uint32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))));
    }
    case 8:
    { ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
      return (isFloatingPointFormat_ ? *reinterpret_cast<double*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                     : (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_LONG_LONG (*reinterpret_cast<int64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                                                    : ACE_SWAP_LONG_LONG (*reinterpret_cast<uint64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                                           : (isSignedSampleFormat_ ? *reinterpret_cast<int64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                                                    : *reinterpret_cast<uint64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))));
    }
    case 16:
    { ACE_ASSERT (ACE_SIZEOF_LONG_DOUBLE == 16);
      ACE_ASSERT (isFloatingPointFormat_);
      return *reinterpret_cast<long double*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]);
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("unknown/invalid sound sample size (was: %d), aborting\n"),
                  soundSampleSize_));
      break;
    }
  } // end SWITCH

  return 0.0;
}

template <typename ValueType>
bool
Common_Math_FFT_SampleIterator_T<ValueType>::initialize (unsigned int dataSampleSize_in,
                                                         unsigned int soundSampleSize_in,
                                                         bool isSignedSampleFormat_in,
                                                         bool isFloatingPointFormat_in,
                                                         int sampleByteOrder_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_SampleIterator_T::initialize"));

  dataSampleSize_ = dataSampleSize_in;
  soundSampleSize_ = soundSampleSize_in;
  isSignedSampleFormat_ = isSignedSampleFormat_in;
  isFloatingPointFormat_ = isFloatingPointFormat_in;
  sampleByteOrder_ = sampleByteOrder_in;

  reverseEndianness_ =
    sampleByteOrder_ && (ACE_BYTE_ORDER != sampleByteOrder_);

  isInitialized_ = true;

  return true;
}

//////////////////////////////////////////

template <typename ValueType>
Common_Math_FFT_T<ValueType>::Common_Math_FFT_T (unsigned int channels_in,
                                                 unsigned int slots_in,
                                                 unsigned int sampleRate_in)
 : isInitialized_ (false)
 , buffer_ (NULL)
 , X_ (NULL)
 , bitReverseMap_ (NULL)
 , channels_ (channels_in)
 , halfSlots_ (slots_in / 2)
 , slots_ (slots_in)
 , sampleRate_ (sampleRate_in)
 , maxValue_ (0.0)
 , sqMaxValue_ (0.0)
 /////////////////////////////////////////
 , logSlots_ (0)
 , sqrtSlots_ (0.0)
 , W_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Common_Math_FFT_T"));

  isInitialized_ = Initialize (channels_in,
                               slots_in,
                               sampleRate_in);
}

template <typename ValueType>
Common_Math_FFT_T<ValueType>::~Common_Math_FFT_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::~Common_Math_FFT_T"));

  if (buffer_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      delete [] buffer_[i];
    delete [] buffer_;
  } // end IF
  if (bitReverseMap_)
    delete [] bitReverseMap_;
  if (W_)
  {
    for (unsigned int l = 1; l <= static_cast<unsigned int> (logSlots_); ++l)
      delete [] W_[l];
    delete [] W_;
  } // end IF
  if (X_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      delete [] X_[i];
    delete [] X_;
  } // end IF
}

template <typename ValueType>
bool
Common_Math_FFT_T<ValueType>::Initialize (unsigned int channels_in,
                                          unsigned int slots_in,
                                          unsigned int sampleRate_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Initialize"));

  // sanity check(s)
  ACE_ASSERT (channels_in > 0);
  ACE_ASSERT (slots_in > 0 && (slots_in % 2) == 0);
  ACE_ASSERT (sampleRate_in);

  if (isInitialized_)
  {
    isInitialized_ = false;

    if (buffer_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] buffer_[i];
      delete [] buffer_; buffer_ = NULL;
    } // end IF
    channels_ = 0;
    halfSlots_ = 0;
    slots_ = 0;
    sampleRate_ = 0;
    maxValue_ = 0.0;
    sqMaxValue_ = 0.0;

    if (bitReverseMap_)
    {
      delete [] bitReverseMap_; bitReverseMap_ = NULL;
    } // end IF
    if (W_)
    {
      for (int l = 1; l <= logSlots_; ++l)
        delete [] W_[l];
      delete [] W_; W_ = NULL;
    } // end IF
    logSlots_ = 0;
    sqrtSlots_ = 0.0;
    if (X_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] X_[i];
      delete [] X_; X_ = NULL;
    } // end IF
  } // end IF

  int rev = 0;
  int _2_l = 2;

  ACE_NEW_NORETURN (buffer_,
                    ValueType*[channels_in]);
  if (unlikely (!buffer_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (buffer_[i],
                      ValueType[slots_in]);
    if (unlikely (!buffer_[i]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
  } // end FOR
#if 0
  // 1 kHz calibration wave
  for (unsigned int i = 0; i < channels_in; ++i)
    for (unsigned int j = 0; j < slots_in; ++j)
        buffer_[i][j] = 1600 * sin (2.0 * M_PI * 1000.0 * j / sampleRate_in);
#else
  for (unsigned int i = 0; i < channels_in; ++i)
    ACE_OS::memset (buffer_[i], 0, slots_in * sizeof (ValueType));
#endif // 0
  channels_ = channels_in;
  halfSlots_ = slots_in / 2;
  slots_ = slots_in;
  sampleRate_ = sampleRate_in;

  // set up bit-reverse mapping
  ACE_NEW_NORETURN (bitReverseMap_,
                    int[slots_]);
  if (unlikely (!bitReverseMap_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < slots_ - 1; ++i)
  {
    bitReverseMap_[i] = rev;
    int mask = halfSlots_;
    // add 1 backwards
    while (rev >= mask)
    {
      rev -= mask; // turn off this bit
      mask >>= 1;
    } // end WHILE
    rev += mask;
  } // end FOR
  bitReverseMap_[slots_ - 1] = static_cast<int> (slots_ - 1);

  // calculate binary log
  slots_in--;
  while (slots_in)
  {
    slots_in >>= 1;
    logSlots_++;
  } // end WHILE
  sqrtSlots_ = std::sqrt (static_cast<ValueType> (slots_));

  // precompute complex exponentials
  ACE_NEW_NORETURN (W_,
                    std::complex<ValueType>*[logSlots_ + 1]);
  //W_ =
  //  reinterpret_cast<std::complex<ValueType>**> (ACE_OS::malloc (sizeof (std::complex<ValueType>*) * (logSlots_ + 1)));
  if (unlikely (!W_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (int l = 1; l <= logSlots_; ++l)
  {
    ACE_NEW_NORETURN (W_[l],
                      std::complex<ValueType>[slots_]);
    if (unlikely (!W_[l]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
    for (unsigned int i = 0; i < slots_; ++i)
      W_[l][i] = std::complex<ValueType> ( std::cos (static_cast<ValueType> (2.0 * M_PI) * i / static_cast<ValueType> (_2_l)),
                                          -std::sin (static_cast<ValueType> (2.0 * M_PI) * i / static_cast<ValueType> (_2_l)));
    _2_l *= 2;
  } // end FOR

  ACE_NEW_NORETURN (X_,
                    std::complex<ValueType>*[channels_]);
  if (unlikely (!X_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_; ++i)
  {
    ACE_NEW_NORETURN (X_[i],
                      std::complex<ValueType>[slots_]);
    if (unlikely (!X_[i]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
  } // end FOR
  for (unsigned int j = 0; j < channels_; ++j)
    for (int i = 0; i < slots_; ++i)
      X_[j][bitReverseMap_[i]] = std::complex<ValueType> (buffer_[j][i], 0);

  isInitialized_ = true;

  return true;

error:
  if (buffer_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete [] buffer_[i];
    delete [] buffer_; buffer_ = NULL;
  } // end IF
  if (bitReverseMap_)
  {
    delete [] bitReverseMap_; bitReverseMap_ = NULL;
  } // end IF
  if (W_)
  {
    for (int l = 1; l <= logSlots_; ++l)
      delete [] W_[l];
    delete [] W_; W_ = NULL;
  } // end IF
  if (X_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete [] X_[i];
    delete [] X_; X_ = NULL;
  } // end IF

  return false;
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType>::CopyIn (unsigned int channel_in,
                                      unsigned int samples_in,
                                      ITERATOR_T& iterator_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::CopyIn"));

  // sanity check(s)
  ACE_ASSERT (channel_in > 0);
  ACE_ASSERT (samples_in <= slots_);
  //if (unlikely (samples_in > slots_))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("buffer too small (expected: %u, is: %u), returning\n"),
  //              samples_in, slots_));
  //  return;
  //} // end IF

  // make space for inbound samples at the end of the buffer,
  // shifting previous samples towards the beginning
  ACE_OS::memmove (&buffer_[channel_in][0], &buffer_[channel_in][samples_in],
                   (slots_ - samples_in) * sizeof (ValueType));

  // copy the sample data to the tail end of the buffer
  int tail_slot = slots_ - samples_in;
  for (int i = 0; i < samples_in; ++i)
    buffer_[channel_in][tail_slot + i] = iterator_in.get (i, channel_in);

  // initialize the FFT working set buffer
  // *NOTE*: do this once only per update, before Compute()
  //for (int i = 0; i < slots_; ++i)
  //  X_[channel_in][bitReverseMap_[i]] = std::complex<ValueType> (buffer_[channel_in][i], 0.0);
}

//
//               0   1   2   3   4   5   6   7
//  level   1
//  step    1                                     0
//  increm  2                                   W 
//  j = 0        <--->   <--->   <--->   <--->   1
//  level   2
//  step    2
//  increm  4                                     0
//  j = 0        <------->       <------->      W      1
//  j = 1            <------->       <------->   2   W
//  level   3                                         2
//  step    4
//  increm  8                                     0
//  j = 0        <--------------->              W      1
//  j = 1            <--------------->           3   W      2
//  j = 2                <--------------->            3   W      3
//  j = 3                    <--------------->             3   W
//                                                              3
//

template <typename ValueType>
void
Common_Math_FFT_T<ValueType>::Compute (unsigned int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Compute"));

  // step = 2 ^ (level - 1)
  // increment = 2 ^ level;
  unsigned int step = 1;
  for (unsigned int level = 1; level <= static_cast<unsigned int> (logSlots_); ++level)
  {
    unsigned int increment = step * 2;
    for (unsigned int j = 0; j < step; ++j)
    {
      // U = exp ( - 2 PI j / 2 ^ level )
      std::complex<ValueType> U = W_[level][j];
      for (unsigned int i = j; i < slots_; i += increment)
      {
        // butterfly
        std::complex<ValueType> T = U;
        T *= X_[channel_in][i + step];
        X_[channel_in][i + step]  = X_[channel_in][i];
        X_[channel_in][i + step] -= T;
        X_[channel_in][i]        += T;
      } // end FOR
    } // end FOR
    step *= 2;
  } // end FOR
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType>::ApplyHammingWindow (unsigned int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::ApplyHammingWindow"));

  // sanity check(s)
  ACE_ASSERT (buffer_);

  for (unsigned int i = 0; i < slots_; ++i)
    buffer_[channel_in][i] *= (0.54 - 0.46 * std::cos ((2.0 * M_PI * i) / static_cast<ValueType> (slots_)));
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType>::ComputeMaxValue ()
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::ComputeMaxValue"));

  // sanity check(s)
  ACE_ASSERT (X_);

  ValueType temp = 0.0;

  for (unsigned int j = 0; j < channels_; ++j)
    for (unsigned int i = 1; i < halfSlots_; ++i)
    {
      ValueType magnitude = std::sqrt (std::norm (X_[j][i]));
      temp = std::max (temp, magnitude);
    } // end FOR

  maxValue_ = temp;
  sqMaxValue_ = maxValue_ * maxValue_;
}
