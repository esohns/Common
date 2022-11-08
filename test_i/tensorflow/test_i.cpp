#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "tensorflow/c/c_api.h"
#include "tensorflow/c/tf_tstring.h"

#include "ace/config-lite.h"
#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Init_ACE.h"
#include "ace/OS.h"
#include "ace/Profile_Timer.h"
#include "ace/Time_Value.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_file_tools.h"
#include "common_string_tools.h"

#include "common_timer_tools.h"

#include "common_log_tools.h"

enum Test_I_ModeType
{
  TEST_I_MODE_DEFAULT = 0,
  TEST_I_MODE_GRAPH_INFERENCE,
  ////////////////////////////////////////
  TEST_I_MODE_MAX,
  TEST_I_MODE_INVALID
};

void
do_print_usage (const std::string& programName_in)
{
  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : program mode [")
            << TEST_I_MODE_DEFAULT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
                      enum Test_I_ModeType& mode_out,
                      bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  mode_out = TEST_I_MODE_DEFAULT;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("m:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'm':
      {
        std::istringstream converter (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()),
                                      std::ios_base::in);
        int i = 0;
        converter >> i;
        mode_out = static_cast<enum Test_I_ModeType> (i);
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argument_parser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.long_option ())));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

inline void free_buffer (void* data_in, size_t length_in) { delete [] data_in; }
inline void deallocator (void* data_in, size_t length_in, void* arg_in) { ::free ((void*)data_in); }

void
do_work (int argc_in,
         ACE_TCHAR* argv_in[],
         enum Test_I_ModeType mode_in)
{
  ::printf ("Hello from TensorFlow C library version %s\n", TF_Version ());

  switch (mode_in)
  {
    case TEST_I_MODE_DEFAULT:
    {
      TF_Graph* graph_p = TF_NewGraph ();
      TF_SessionOptions* options_p = TF_NewSessionOptions ();
      TF_Status* status_p = TF_NewStatus ();
      TF_Session* session_p = TF_NewSession (graph_p, options_p, status_p);
      char hello[] = "Hello TensorFlow!";
      struct TF_TString string_s;
      TF_StringInit (&string_s);
      TF_StringCopy (&string_s, hello, ACE_OS::strlen (hello));
      TF_Tensor* tensor_p = TF_AllocateTensor (TF_STRING, 0, 0, TF_StringGetSize (&string_s));
      TF_Tensor* tensor_output_p = NULL;
      TF_OperationDescription* operation_description_p = TF_NewOperation (graph_p, "Const", "hello");
      TF_Operation* operation_p = NULL;
      struct TF_Output output_s;

      ACE_OS::memcpy (TF_TensorData (tensor_p), &string_s, sizeof (struct TF_TString));
      TF_StringDealloc (&string_s);
      TF_SetAttrTensor (operation_description_p, "value", tensor_p, status_p);
      TF_SetAttrType (operation_description_p, "dtype", TF_TensorType (tensor_p));
      operation_p = TF_FinishOperation (operation_description_p, status_p);

      output_s.oper = operation_p;
      output_s.index = 0;

      TF_SessionRun (session_p, 0,
                     0, 0, 0,                        // Inputs
                     &output_s, &tensor_output_p, 1, // Outputs
                     &operation_p, 1,                // Operations
                     0, status_p);

      ::printf ("status code: %i\n", TF_GetCode (status_p));
      struct TF_TString* string_p =
        (struct TF_TString*)TF_TensorData (tensor_output_p);
      ::printf ("%s\n", TF_StringGetDataPointer (string_p));

      TF_CloseSession (session_p, status_p);
      TF_DeleteSession (session_p, status_p);
      TF_DeleteStatus (status_p);
      TF_DeleteSessionOptions (options_p);

      break;
    }
    case TEST_I_MODE_GRAPH_INFERENCE:
    {
      // load 'frozen' graph (aka 'model')
      uint8_t* data_p = NULL;
      ACE_UINT64 size_i = 0;
      if (!Common_File_Tools::load (ACE_TEXT_ALWAYS_CHAR ("slopemodel.pb"),
                                    data_p,
                                    size_i,
                                    0))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to load model (was: \"%s\"), returning\n"),
                    ACE_TEXT ("slopemodel.pb")));
        return;
      } // end IF
      ACE_ASSERT (data_p && size_i);
      TF_Buffer* buffer_p = TF_NewBuffer ();
      ACE_ASSERT (buffer_p);
      buffer_p->data = data_p;
      buffer_p->length = size_i;
      buffer_p->data_deallocator = free_buffer;

      TF_Graph* graph_p = TF_NewGraph ();
      ACE_ASSERT (graph_p);
      TF_Status* status_p = TF_NewStatus ();
      ACE_ASSERT (status_p);
      TF_ImportGraphDefOptions* options_p = TF_NewImportGraphDefOptions ();
      ACE_ASSERT (options_p);
      TF_GraphImportGraphDef (graph_p, buffer_p, options_p, status_p);
      TF_DeleteImportGraphDefOptions (options_p); options_p = NULL;
      if (TF_GetCode (status_p) != TF_OK)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to import model (was: \"%s\"), returning\n"),
                    ACE_TEXT ("slopemodel.pb")));
        TF_DeleteGraph (graph_p);
        TF_DeleteStatus (status_p);
        return;
      } // end IF

      // create session
      TF_SessionOptions* options_2 = TF_NewSessionOptions ();
      ACE_ASSERT (options_2);
      TF_Session* session_p = TF_NewSession (graph_p, options_2, status_p);
      TF_DeleteSessionOptions (options_2); options_2 = NULL;
      if (TF_GetCode (status_p) != TF_OK)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to create session, returning\n")));
        TF_DeleteSession (session_p, status_p);
        TF_DeleteGraph (graph_p);
        TF_DeleteStatus (status_p);
        return;
      } // end IF

      //// run restore
      //TF_Operation* checkpoint_operation_p = TF_GraphOperationByName (graph_p, "save/Const");
      //ACE_ASSERT (checkpoint_operation_p);
      //TF_Operation* restore_operation_p = TF_GraphOperationByName (graph_p, "save/restore_all");
      //ACE_ASSERT (restore_operation_p);
      //const char* checkpoint_path_str = "./exported/my_model";
      //size_t checkpoint_path_str_len = ACE_OS::strlen (checkpoint_path_str);
      //struct TF_TString string_s;
      //TF_StringInit (&string_s);
      //TF_StringCopy (&string_s, checkpoint_path_str, ACE_OS::strlen (checkpoint_path_str));
      //size_t encoded_size = TF_StringGetSize (&string_s);
      //// The format for TF_STRING tensors is:
      ////   start_offset: array[uint64]
      ////   data:         byte[...]
      ////size_t total_size = sizeof (uint64_t) + encoded_size;
      ////char* input_encoded = (char*)malloc (total_size);
      ////ACE_OS::memset (input_encoded, 0, total_size);
      ////TF_StringEncode (checkpoint_path_str, checkpoint_path_str_len,
      ////                 input_encoded + sizeof(int64_t),
      ////                 encoded_size,
      ////                 status);
      ////if (TF_GetCode(status) != TF_OK) {
      ////fprintf(stderr, "ERROR: something wrong with encoding: %s",
      ////TF_Message(status));
      ////return 1;
      ////}
      //TF_StringDealloc (&string_s);

      // First two samples, normalized values
      int SEQ_LENGTH = 21;
      float sampledata0[21] = { 1.f, 0.96713615f, 0.88262911f, 0.74178404f, 0.69953052f,
                                0.66666667f, 0.6713615f, 0.57746479f, 0.48826291f, 0.50234742f,
                                0.5399061f, 0.43661972f, 0.33333333f, 0.342723f, 0.25821596f,
                                0.24882629f, 0.19248826f, 0.16901408f, 0.07511737f, 0.f,
                                0.03286385f };
      float sampledata1[21] = { 0.f        , 0.0257732f, 0.01030928f, 0.1185567f, 0.20618557f,
                                0.28865979f, 0.30412371f, 0.33505155f, 0.36597938f, 0.42268041f,
                                0.43814433f, 0.55670103f, 0.73195876f, 0.69587629f, 0.77835052f,
                                0.85051546f, 0.8814433f, 0.81443299f, 0.92783505f, 0.98969072f,
                                1.f };

      // generate input
      TF_Operation* input_operation_p = TF_GraphOperationByName (graph_p, "dense_1_input");
      ACE_ASSERT (input_operation_p);
      ::printf ("input_op has %i inputs\n", TF_OperationNumOutputs (input_operation_p));
      float* raw_input_data = (float*)malloc (SEQ_LENGTH * sizeof (float));
      for (int i = 0; i < 21; i++)
        raw_input_data[i] = sampledata1[i];
      //raw_input_data[1] = 1.f;
      int64_t* raw_input_dims = (int64_t*)malloc (2 * sizeof (int64_t));
      raw_input_dims[0] = 1;
      raw_input_dims[1] = 21;

      // prepare inputs
      TF_Output* input_p = (TF_Output*)malloc (sizeof (TF_Output));
      ACE_ASSERT (input_p);
      input_p->oper = input_operation_p;
      input_p->index = 0;
      TF_Tensor* input_tensor_p = TF_NewTensor (TF_FLOAT, raw_input_dims, 2, raw_input_data,
                                                SEQ_LENGTH * sizeof (float),
                                                &deallocator,
                                                NULL);
      ACE_ASSERT (input_tensor_p);
      TF_Tensor** run_inputs_tensors_p = (TF_Tensor**)malloc (sizeof (TF_Tensor*));
      ACE_ASSERT (run_inputs_tensors_p);
      run_inputs_tensors_p[0] = input_tensor_p;

      // prepare outputs
      TF_Operation* output_operation_p = TF_GraphOperationByName (graph_p, "dense_2/Sigmoid");
      ACE_ASSERT (output_operation_p);
      TF_Output* output_p = (TF_Output*)malloc (sizeof (TF_Output));
      output_p->oper = output_operation_p;
      output_p->index = 0;

      float* raw_output_data = (float*)malloc (1 * sizeof (float));
      raw_output_data[0] = 1.f;
      int64_t* raw_output_dims = (int64_t*)malloc (1 * sizeof (int64_t));
      raw_output_dims[0] = 1;

      TF_Tensor* output_tensor_p = TF_NewTensor (TF_FLOAT, raw_output_dims, 1, raw_output_data,
                                                 1 * sizeof (float),
                                                 &deallocator,
                                                 NULL);
      ACE_ASSERT (output_tensor_p);
      TF_Tensor** run_output_tensors_p = (TF_Tensor**)malloc (sizeof (TF_Tensor*));
      ACE_ASSERT (run_output_tensors_p);
      run_output_tensors_p[0] = output_tensor_p;

      // run network
      TF_SessionRun (session_p,
      /* RunOptions */ NULL,
      /* Input tensors */ input_p, run_inputs_tensors_p, 1,
      /* Output tensors */ output_p, run_output_tensors_p, 1,
      /* Target operations */ NULL, 0,
      /* RunMetadata */ NULL,
      /* Output status */ status_p);
      if (TF_GetCode (status_p) != TF_OK)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to run session: \"%s\", returning\n"),
                    ACE_TEXT (TF_Message (status_p))));
        TF_DeleteSession (session_p, status_p);
        TF_DeleteGraph (graph_p);
        TF_DeleteStatus (status_p);
        TF_DeleteBuffer (buffer_p);
        return;
      } // end IF

      void* output_data_p = TF_TensorData (run_output_tensors_p[0]);
      float data1 = ((float*)output_data_p)[0];
      printf ("Prediction: %.4f\n", data1);

      TF_CloseSession (session_p, status_p);
      TF_DeleteSession (session_p, status_p);
      TF_DeleteGraph (graph_p);
      TF_DeleteStatus (status_p);
      TF_DeleteBuffer (buffer_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown mode (was: %d), returning\n"),
                  mode_in));
      break;
    }
  } // end SWITCH

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working\n")));
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  int result = EXIT_FAILURE, result_2 = -1;

  // step0: initialize
  // *PORTABILITY*: on Windows, initialize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result_2 = ACE::init ();
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
  ACE_Profile_Timer process_profile;
  process_profile.start ();

//  Common_Tools::initialize ();

  ACE_High_Res_Timer timer;
  ACE_Time_Value working_time;
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_Time_Value user_time, system_time;

  // step1a set defaults
  std::string path_root = Common_File_Tools::getWorkingDirectory ();
  enum Test_I_ModeType mode_type_e = TEST_I_MODE_DEFAULT;
  bool trace_information = false;
  std::string log_file_name;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             mode_type_e,
                             trace_information))
  {
    do_print_usage (ACE::basename (argv_in[0]));
    goto clean;
  } // end IF

  //if (!Common_File_Tools::isReadable (source_file_path))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("invalid argument(s), aborting\n")));
  //  do_print_usage (ACE::basename (argv_in[0]));
  //  goto clean;
  //} // end IF

  // step1c: initialize logging and/or tracing
  log_file_name =
    Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                      ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
  if (!Common_Log_Tools::initializeLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)), // program name
                                            log_file_name,              // log file name
                                            false,                      // log to syslog ?
                                            false,                      // trace messages ?
                                            trace_information,          // debug messages ?
                                            NULL))                      // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));
    goto clean;
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (argc_in,
           argv_in,
           mode_type_e);
  timer.stop ();

  // debug info
  timer.elapsed_time (working_time);
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"\n"),
              ACE_TEXT (Common_Timer_Tools::periodToString (working_time).c_str ())));

  // step3: shut down
  process_profile.stop ();

  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  result_2 = process_profile.elapsed_time (elapsed_time);
  if (result_2 == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));
    goto clean;
  } // end IF
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  user_time.set (elapsed_rusage.ru_utime);
  system_time.set (elapsed_rusage.ru_stime);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (Common_Timer_Tools::periodToString (user_time).c_str ()),
              ACE_TEXT (Common_Timer_Tools::periodToString (system_time).c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (Common_Timer_Tools::periodToString (user_time).c_str ()),
              ACE_TEXT (Common_Timer_Tools::periodToString (system_time).c_str ()),
              elapsed_rusage.ru_maxrss,
              elapsed_rusage.ru_ixrss,
              elapsed_rusage.ru_idrss,
              elapsed_rusage.ru_isrss,
              elapsed_rusage.ru_minflt,
              elapsed_rusage.ru_majflt,
              elapsed_rusage.ru_nswap,
              elapsed_rusage.ru_inblock,
              elapsed_rusage.ru_oublock,
              elapsed_rusage.ru_msgsnd,
              elapsed_rusage.ru_msgrcv,
              elapsed_rusage.ru_nsignals,
              elapsed_rusage.ru_nvcsw,
              elapsed_rusage.ru_nivcsw));
#endif // ACE_WIN32 || ACE_WIN64

  result = EXIT_SUCCESS;

clean:
  Common_Log_Tools::finalizeLogging ();
  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
