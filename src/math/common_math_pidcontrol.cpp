#include "stdafx.h"

#include "common_math_pidcontrol.h"

#include <algorithm>

#include "ace/Log_Msg.h"

Common_PIDControl::Common_PIDControl (double kP_in,
                                      double kI_in,
                                      double kD_in,
                                      double uMax_in,
                                      double uMin_in)
 : kP_ (kP_in)
 , kI_ (kI_in)
 , kD_ (kD_in)
 , uMax_ (uMax_in)
 , uMin_ ((uMax_in != std::numeric_limits<double>::max () &&
           uMin_in == std::numeric_limits<double>::min ()) ? -uMax_in
                                                           :  uMin_in)
 , integratorActive_ (true)
 , errorIntegral_ (0.0)
 , errorPrevious_ (NAN)
 , timer_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_PIDControl::Common_PIDControl"));

}

// get actuator command from error, which is target minus actual control value
double
Common_PIDControl::operator() (double error_in,
                               double errorDerivate_in)
{
//  COMMON_TRACE (ACE_TEXT ("Common_PIDControl::operator()(double,double=)"));

  // handle first call specifically
  if (unlikely (!timer_.isRunning () ||
                std::isnan (errorPrevious_)))
  {
    // (re-)start timer
    timer_.tic ();

    // reset integral over previous errors
    errorIntegral_ = 0.0;

    // remember error for next time
    errorPrevious_ = error_in;

    // simple proportional control, obey actuator limits
    return std::max (uMin_, std::min (uMax_, kP_ * error_in));
  } // end IF

  // get time since last call, restart timer
  double seconds_last_call_d = timer_.tocTic () / 1000.0;

  // compute controller contributions
  double error_derivate_d = errorDerivate_in;
  if (likely (std::isnan (error_derivate_d)))
    error_derivate_d = (error_in - errorPrevious_) / seconds_last_call_d;
  double uP_d = kP_ * error_in,
         uI_d = kI_ * errorIntegral_,
         uD_d = kD_ * error_derivate_d;

  // full PID control, obey actuator limits
  double uUnlim_d = uP_d + uI_d + uD_d,
         uLim_d = std::max (uMin_, std::min (uMax_, uUnlim_d));

  // *NOTE*: anti-wind-up: only integrate if actuator is not saturated in the
  //         integral controllers' direction
  // *NOTE*: implemented according to
  //         www.isep.pw.edu.pl/ZakladNapedu/lab-ane/anti-windup.pdf
  if (integratorActive_  &&
      ((uUnlim_d == uLim_d)
//       || ( uI_d     * error_in ) < 0.0 ) ) { // dev. note: fig.  9 of the file above
       || (uUnlim_d * error_in) < 0.0)) { // dev. note: fig. 10 of the file above
    // linearly interpolate between previous and current errors
    errorIntegral_ += (errorPrevious_ + error_in) / 2.0 * seconds_last_call_d;
  } // end IF

  // remember error for next time
  errorPrevious_ = error_in;

  // finally...
  return uLim_d;
}
