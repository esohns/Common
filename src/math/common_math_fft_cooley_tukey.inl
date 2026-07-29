#include <cmath>

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"

template <typename ValueType>
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::Common_Math_FFT_T (unsigned int channels_in,
                                                                  unsigned int slots_in,
                                                                  unsigned int sampleRate_in)
 : isInitialized_ (false)
 , buffer_ (NULL)
 , X_ (NULL)
 , channels_ (channels_in)
 , halfSlots_ (slots_in / 2)
 , slots_ (slots_in)
 , sampleRate_ (sampleRate_in)
 , maxValue_ (0.0)
 , sqMaxValue_ (0.0)
 /////////////////////////////////////////
 , sqrtSlots_ (0.0)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Common_Math_FFT_T"));

  isInitialized_ = Initialize (channels_in,
                               slots_in,
                               sampleRate_in);
}

template <typename ValueType>
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::~Common_Math_FFT_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::~Common_Math_FFT_T"));

  if (buffer_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      delete [] buffer_[i];
    delete [] buffer_;
  } // end IF
  if (X_)
    delete [] X_;
}

template <typename ValueType>
bool
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::Initialize (unsigned int channels_in,
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
    if (X_)
    {
      delete [] X_; X_ = NULL;
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

  sqrtSlots_ = std::sqrt (static_cast<ValueType> (slots_));

  ACE_NEW_NORETURN (X_,
                    std::valarray<std::complex<ValueType> >[channels_]);
  if (unlikely (!X_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_; ++i)
    X_[i].resize (slots_);

  isInitialized_ = true;

  return true;

error:
  if (buffer_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete [] buffer_[i];
    delete [] buffer_; buffer_ = NULL;
  } // end IF
  if (X_)
  {
    delete [] X_; X_ = NULL;
  } // end IF

  return false;
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::CopyIn (unsigned int channel_in,
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

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::Setup (unsigned int channel_in)
{
  // COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Setup"));

  for (unsigned int j = 0; j < slots_; ++j)
    X_[channel_in][j] = std::complex<ValueType> (buffer_[channel_in][j], static_cast<ValueType> (0.0));
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::Compute (unsigned int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Compute"));

  //// divide
  //std::valarray<std::complex<ValueType> > even = X_[channel_in][std::slice (0, halfSlots_, 2)];
  //std::valarray<std::complex<ValueType> > odd = X_[channel_in][std::slice (1, halfSlots_, 2)];

  //// conquer
  //fft (even);
  //fft (odd);

  //// combine
  //for (size_t k = 0; k < halfSlots_; ++k)
  //{
  //  std::complex<ValueType> t = std::polar (static_cast<ValueType> (1.0), static_cast<ValueType> (-2.0 * M_PI) * k / slots_) * odd[k];
  //  X_[channel_in][k] = even[k] + t;
  //  X_[channel_in][k + halfSlots_] = even[k] - t;
  //} // end FOR

  unsigned int k = slots_, n;
  const ValueType thetaT = static_cast<ValueType> (M_PI) / slots_;
  const std::complex<ValueType> phiT (std::cos (thetaT), -std::sin (thetaT));
  std::complex<ValueType> T;

  while (k > 1)
  {
    n = k;
    k >>= 1;
    phiT = phiT * phiT;
    T = static_cast<ValueType> (1.0);
    for (unsigned int l = 0; l < k; l++)
    {
      for (unsigned int a = l; a < slots_; a += n)
      {
        unsigned int b = a + k;
        std::complex<ValueType> t = X_[channel_in][a] - X_[channel_in][b];
        X_[channel_in][a] += X_[channel_in][b];
        X_[channel_in][b] = t * T;
      } // end FOR
      T *= phiT;
    } // end FOR
  } // end WHILE

  // Decimate
  const unsigned int m = (unsigned int)log2 (slots_);
  for (unsigned int a = 0; a < slots_; a++)
  {
    unsigned int b = a;
    // Reverse bits
    b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
    b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
    b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
    b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
    b = ((b >> 16) | (b << 16)) >> (32 - m);
    if (b > a)
    {
      std::complex<ValueType> t = X_[channel_in][a];
      X_[channel_in][a] = X_[channel_in][b];
      X_[channel_in][b] = t;
    } // end IF
  } // end FOR
}

template <typename ValueType>
std::vector<ValueType>
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::Spectrum (int channel_in,
                                                         bool normalize_in)
{
  // COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Spectrum"));

  std::vector<ValueType> result_a;

  // sanity check(s)
  if (unlikely (!isInitialized_))
  {
    result_a.resize (halfSlots_ - 1, 0);
    return result_a;
  } // end IF

  if (unlikely (normalize_in))
    ComputeMaxValue (-1);

  // *IMPORTANT NOTE*: - the first ('DC'-)slot does not contain frequency
  //                     information --> i = 1
  //                   - the slots N/2 - N are mirrored and do not contain
  //                     additional information
  //                     --> there are only N/2 - 1 meaningful values
  ValueType value_f;
  if (likely (channel_in == -1))
  {
    for (int i = 1; i < halfSlots_; ++i)
    {
      value_f = std::sqrt (std::norm (X_[0][i]));
      for (int j = 1; j < channels_; ++j)
        value_f += std::sqrt (std::norm (X_[j][i]));
      value_f /= static_cast<ValueType> (channels_);
      result_a.push_back (value_f);
    } // end FOR
  } // end IF
  else
  { ACE_ASSERT (channel_in < channels_);
    for (int i = 1; i < halfSlots_; ++i)
    {
      value_f = std::sqrt (std::norm (X_[channel_in][i]));
      result_a.push_back (value_f);
    } // end FOR
  } // end ELSE

  if (unlikely (normalize_in))
    for (typename std::vector<ValueType>::iterator iterator = result_a.begin ();
         iterator != result_a.end ();
         ++iterator)
      *iterator /= maxValue_;

  return result_a;
}

template <typename ValueType>
void
Common_Math_FFT_T<ValueType,
                  FFT_ALGORITHM_COOLEY_TUKEY>::ComputeMaxValue (int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::ComputeMaxValue"));

  // sanity check(s)
  ACE_ASSERT (X_);
  ACE_ASSERT (channel_in < static_cast<int> (channels_));

  ValueType temp = 0.0;

  if (likely (channel_in == -1))
  {
    for (unsigned int j = 0; j < channels_; ++j)
      for (unsigned int i = 1; i < halfSlots_; ++i)
      {
        ValueType magnitude = std::sqrt (std::norm (X_[j][i]));
        temp = std::max (temp, magnitude);
      } // end FOR
  } // end IF
  else
  {
    for (unsigned int i = 1; i < halfSlots_; ++i)
    {
      ValueType magnitude = std::sqrt (std::norm (X_[channel_in][i]));
      temp = std::max (temp, magnitude);
    } // end FOR
  } // end ELSE

  maxValue_ = temp;
  sqMaxValue_ = maxValue_ * maxValue_;
}

//template <typename ValueType>
//void
//Common_Math_FFT_T<ValueType,
//                  FFT_ALGORITHM_COOLEY_TUKEY>::fft (std::valarray<std::complex<ValueType> >& x_in)
//{
//  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::fft"));
//
//  const size_t N = x_in.size ();
//  if (unlikely (N <= 1))
//    return;
//
//  // divide
//  std::valarray<std::complex<ValueType> > even = x_in[std::slice (0, N / 2, 2)];
//  std::valarray<std::complex<ValueType> > odd = x_in[std::slice (1, N / 2, 2)];
//
//  // conquer
//  fft (even);
//  fft (odd);
//
//  // combine
//  for (size_t k = 0; k < N / 2; ++k)
//  {
//    std::complex<ValueType> t =
//      std::polar (static_cast<ValueType> (1.0), static_cast<ValueType> (-2.0 * M_PI) * k / N) * odd[k];
//    x_in[k] = even[k] + t;
//    x_in[k + N / 2] = even[k] - t;
//  } // end FOR
//}
