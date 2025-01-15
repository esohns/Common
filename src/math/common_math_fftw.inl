
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename ValueType>
Common_Math_FFTW_SampleIterator_T<ValueType>::Common_Math_FFTW_SampleIterator_T (uint8_t* buffer_in)
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
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_SampleIterator_T_T::Common_Math_FFTW_SampleIterator_T_T"));

}

template <typename ValueType>
ValueType
Common_Math_FFTW_SampleIterator_T<ValueType>::get (unsigned int index_in,
                                                   unsigned int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_SampleIterator_T::get"));

  // sanity check(s)
  ACE_ASSERT (buffer_);
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
Common_Math_FFTW_SampleIterator_T<ValueType>::initialize (unsigned int dataSampleSize_in,
                                                          unsigned int soundSampleSize_in,
                                                          bool isSignedSampleFormat_in,
                                                          bool isFloatingPointFormat_in,
                                                          int sampleByteOrder_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_SampleIterator_T::initialize"));

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
Common_Math_FFTW_T<ValueType>::Common_Math_FFTW_T (unsigned int channels_in,
                                                   unsigned int slots_in,
                                                   unsigned int sampleRate_in)
 : isInitialized_ (false)
 , buffer_ (NULL)
 , X_ (NULL)
 , Y_ (NULL)
 , plans_ (NULL)
 , channels_ (channels_in)
 , slots_ (slots_in)
 , sampleRate_ (sampleRate_in)
 , sqrtSlots_ (0.0)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_T::Common_Math_FFTW_T"));

  isInitialized_ = Initialize (channels_in,
                               slots_in,
                               sampleRate_in);
}

template <typename ValueType>
Common_Math_FFTW_T<ValueType>::~Common_Math_FFTW_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_T::~Common_Math_FFTW_T"));

  if (buffer_)
    for (unsigned int i = 0; i < channels_; ++i)
      delete [] buffer_[i];
  delete [] buffer_;
  if (X_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      delete[] X_[i];
    delete[] X_;
  } // end IF
  if (Y_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      delete[] Y_[i];
    delete[] Y_;
  } // end IF
  if (plans_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      fftwf_destroy_plan (plans_[i]);
    delete[] plans_;
  } // end IF
}

template <typename ValueType>
bool
Common_Math_FFTW_T<ValueType>::Initialize (unsigned int channels_in,
                                           unsigned int slots_in,
                                           unsigned int sampleRate_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_T::Initialize"));

  // sanity check(s)
  if (slots_in % 2)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("buffer size must be a power of 2 (was: %d), continuing\n"),
                slots_in));

  if (isInitialized_)
  {
    isInitialized_ = false;

    if (buffer_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] buffer_[i];
      delete [] buffer_; buffer_ = NULL;
    } // end IF
    if (X_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] X_[i];
      delete [] X_; X_ = NULL;
    } // end IF
    if (Y_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] Y_[i];
      delete [] Y_; Y_ = NULL;
    } // end IF
    if (plans_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        fftwf_destroy_plan (plans_[i]);
      delete[] plans_; plans_ = NULL;
    } // end IF
    channels_ = 0;
    halfSlots_ = 0;
    slots_ = 0;
    sampleRate_ = 0;
    maxValue_ = 0.0;
    sqMaxValue_ = 0.0;
    sqrtSlots_ = 0.0;
  } // end IF

  ACE_NEW_NORETURN (buffer_,
                    ValueType*[channels_in]);
  if (!buffer_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (buffer_[i],
                      ValueType[slots_in]);
    if (!buffer_[i])
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      return false;
    } // end IF
  } // end FOR
#if 0
  // 1 kHz calibration wave
  for (unsigned int i = 0; i < slots_in; ++i)
    buffer_[i][j] = 1600 * sin (2.0 * M_PI * 1000.0 * j / sampleRate_in);
#else
  for (unsigned int i = 0; i < channels_in; ++i)
    for (unsigned int j = 0; j < slots_in; ++j)
      buffer_[i][j] = 0.0;
#endif // 0

  ACE_NEW_NORETURN (X_,
                    fftwf_complex*[channels_in]);
  if (unlikely (!X_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (X_[i],
                      fftwf_complex[slots_in]);
    if (unlikely (!X_[i]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
  } // end FOR

  ACE_NEW_NORETURN (Y_,
                    fftwf_complex*[channels_in]);
  if (unlikely (!Y_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (Y_[i],
                      fftwf_complex[slots_in]);
    if (unlikely (!Y_[i]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
  } // end FOR

  ACE_NEW_NORETURN (plans_,
                    fftwf_plan[channels_in]);
  if (unlikely (!plans_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    plans_[i] =
      fftwf_plan_dft_1d (slots_in, X_[i], Y_[i], FFTW_FORWARD, FFTW_ESTIMATE);
    if (unlikely (!plans_[i]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
  } // end FOR

  channels_ = channels_in;
  halfSlots_ = slots_in / 2;
  slots_ = slots_in;
  sampleRate_ = sampleRate_in;
  sqrtSlots_ = std::sqrt (static_cast<ValueType> (slots_in));

  isInitialized_ = true;

  return true;

error:
  if (buffer_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete[] buffer_[i];
    delete[] buffer_; buffer_ = NULL;
  } // end IF
  if (X_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete[] X_[i];
    delete[] X_; X_ = NULL;
  } // end IF
  if (Y_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete[] Y_[i];
    delete[] Y_; Y_ = NULL;
  } // end IF
  if (plans_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      fftwf_destroy_plan (plans_[i]);
    delete[] plans_; plans_ = NULL;
  } // end IF

  return false;
}

template <typename ValueType>
void
Common_Math_FFTW_T<ValueType>::CopyIn (unsigned int channel_in,
                                       unsigned int samples_in,
                                       ITERATOR_T& iterator_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_T::CopyIn"));

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
}

template <typename ValueType>
std::vector<ValueType>
Common_Math_FFTW_T<ValueType>::Spectrum (bool normalize_in)
{
  // COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_T::Spectrum"));

  std::vector<ValueType> result_a;

  // sanity check(s)
  if (unlikely (!isInitialized_))
    return result_a;

  if (unlikely (normalize_in))
    ComputeMaxValue ();

  // *IMPORTANT NOTE*: - the first ('DC'-)slot does not contain frequency
  //                     information --> i = 1
  //                   - the slots N/2 - N are mirrored and do not contain
  //                     additional information
  //                     --> there are only N/2 - 1 meaningful values
  for (int i = 1; i < halfSlots_; ++i)
  {
    ValueType real_f = Y_[0][i][0];
    ValueType imag_f = Y_[0][i][1];
    ValueType value_f = std::sqrt (real_f * real_f + imag_f * imag_f);
    for (int j = 1; j < channels_; ++j)
    {
      real_f = Y_[j][i][0];
      imag_f = Y_[j][i][1];
      value_f += std::sqrt (real_f * real_f + imag_f * imag_f);
    } // end FOR
    value_f /= static_cast<ValueType> (channels_);
    result_a.push_back (value_f);
  } // end FOR

  if (unlikely (normalize_in))
    for (std::vector<ValueType>::iterator iterator = result_a.begin ();
         iterator != result_a.end ();
         ++iterator)
      *iterator /= maxValue_;

  return result_a;
}

template <typename ValueType>
void
Common_Math_FFTW_T<ValueType>::ApplyHammingWindow (unsigned int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_T::ApplyHammingWindow"));

  // sanity check(s)
  ACE_ASSERT (buffer_);

  for (unsigned int i = 0; i < slots_; ++i)
    buffer_[channel_in][i] *= (0.54 - 0.46 * std::cos ((2.0 * M_PI * i) / static_cast<ValueType> (slots_)));
}

template <typename ValueType>
void
Common_Math_FFTW_T<ValueType>::ComputeMaxValue ()
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFTW_T::ComputeMaxValue"));

  // sanity check(s)
  ACE_ASSERT (Y_);

  ValueType temp = 0.0;

  for (unsigned int j = 0; j < channels_; ++j)
    for (unsigned int i = 1; i < halfSlots_; ++i)
    {
      ValueType real_f = Y_[j][i][0];
      ValueType imag_f = Y_[j][i][1];
      ValueType magnitude = std::sqrt (real_f * real_f + imag_f * imag_f);
      temp = std::max (temp, magnitude);
    } // end FOR

  maxValue_ = temp;
  sqMaxValue_ = maxValue_ * maxValue_;
}
