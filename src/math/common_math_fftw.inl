
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"

//template <>
Common_Math_FFT_T<float,
                  FFT_ALGORITHM_FFTW>::Common_Math_FFT_T (unsigned int channels_in,
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
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Common_Math_FFT_T"));

  isInitialized_ = Initialize (channels_in,
                               slots_in,
                               sampleRate_in);
}

//template <>
Common_Math_FFT_T<float,
                  FFT_ALGORITHM_FFTW>::~Common_Math_FFT_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::~Common_Math_FFT_T"));

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

//template <>
bool
Common_Math_FFT_T<float,
                  FFT_ALGORITHM_FFTW>::Initialize (unsigned int channels_in,
                                                   unsigned int slots_in,
                                                   unsigned int sampleRate_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Initialize"));

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
                    float*[channels_in]);
  if (!buffer_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (buffer_[i],
                      float[slots_in]);
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
  sqrtSlots_ = std::sqrt (static_cast<float> (slots_in));

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

//template <>
void
Common_Math_FFT_T<float,
                  FFT_ALGORITHM_FFTW>::Setup (unsigned int channel_in)
{
  // COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Setup"));

  for (unsigned int j = 0; j < slots_; ++j)
  {
    X_[channel_in][j][0] = buffer_[channel_in][j];
    X_[channel_in][j][1] = 0.0f;
  } // end FOR
}

//template <>
void
Common_Math_FFT_T<float,
                  FFT_ALGORITHM_FFTW>::CopyIn (unsigned int channel_in,
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
                   (slots_ - samples_in) * sizeof (float));

  // copy the sample data to the tail end of the buffer
  int tail_slot = slots_ - samples_in;
  for (unsigned int i = 0; i < samples_in; ++i)
    buffer_[channel_in][tail_slot + i] = iterator_in.get (i, channel_in);
}

//template <>
std::vector<float>
Common_Math_FFT_T<float,
                  FFT_ALGORITHM_FFTW>::Spectrum (int channel_in,
                                                 bool normalize_in)
{
  // COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Spectrum"));

  std::vector<float> result_a;

  // sanity check(s)
  if (unlikely (!isInitialized_))
    return result_a;

  if (unlikely (normalize_in))
    ComputeMaxValue (channel_in);

  // *IMPORTANT NOTE*: - the first ('DC'-)slot does not contain frequency
  //                     information --> i = 1
  //                   - the slots N/2 - N are mirrored and do not contain
  //                     additional information
  //                     --> there are only N/2 - 1 meaningful values
  float real_f, imag_f, value_f;

  if (likely (channel_in == -1))
  {
    for (unsigned int i = 1; i < halfSlots_; ++i)
    {
      real_f = Y_[0][i][0];
      imag_f = Y_[0][i][1];
      value_f = std::sqrt (real_f * real_f + imag_f * imag_f);
      for (unsigned int j = 1; j < channels_; ++j)
      {
        real_f = Y_[j][i][0];
        imag_f = Y_[j][i][1];
        value_f += std::sqrt (real_f * real_f + imag_f * imag_f);
      } // end FOR
      value_f /= static_cast<float> (channels_);
      result_a.push_back (value_f);
    } // end FOR
  } // end IF
  else
  { ACE_ASSERT (channel_in < static_cast<int> (channels_));
    for (unsigned int i = 1; i < halfSlots_; ++i)
    {
      real_f = Y_[channel_in][i][0];
      imag_f = Y_[channel_in][i][1];
      value_f = std::sqrt (real_f * real_f + imag_f * imag_f);
      result_a.push_back (value_f);
    } // end FOR
  } // end ELSE

  if (unlikely (normalize_in))
    for (typename std::vector<float>::iterator iterator = result_a.begin ();
         iterator != result_a.end ();
         ++iterator)
      *iterator /= maxValue_;

  return result_a;
}

//template <>
void
Common_Math_FFT_T<float,
                  FFT_ALGORITHM_FFTW>::ComputeMaxValue (int channel_in)
{
  //COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::ComputeMaxValue"));

  // sanity check(s)
  ACE_ASSERT (Y_);
  ACE_ASSERT (channel_in < static_cast<int> (channels_));

  float temp = 0.0f;

  if (likely (channel_in == -1))
  {
    for (unsigned int j = 0; j < channels_; ++j)
      for (unsigned int i = 1; i < halfSlots_; ++i)
      {
        float real_f = Y_[j][i][0];
        float imag_f = Y_[j][i][1];
        float magnitude = std::sqrt (real_f * real_f + imag_f * imag_f);
        temp = std::max (temp, magnitude);
      } // end FOR
  } // end IF
  else
  {
    for (unsigned int i = 1; i < halfSlots_; ++i)
    {
      float real_f = Y_[channel_in][i][0];
      float imag_f = Y_[channel_in][i][1];
      float magnitude = std::sqrt (real_f * real_f + imag_f * imag_f);
      temp = std::max (temp, magnitude);
    } // end FOR
  } // end ELSE

  maxValue_ = temp;
  sqMaxValue_ = maxValue_ * maxValue_;
}
