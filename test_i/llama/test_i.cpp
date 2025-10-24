#include "stdafx.h"

#include "llama.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

#define LLAMACPP_CONTEXT_SIZE_DEFAULT         2048
#define LLAMACPP_NUMBER_OF_GPU_LAYERS_DEFAULT 99

enum Test_I_ModeType
{
  TEST_I_MODE_DEFAULT = 0,
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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c : context size [")
            << LLAMACPP_CONTEXT_SIZE_DEFAULT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g : #GPU layers [")
            << LLAMACPP_NUMBER_OF_GPU_LAYERS_DEFAULT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m : model file path")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p : program mode [")
            << TEST_I_MODE_DEFAULT
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
                      int contextSize_out,
                      int numberOfGPULayers_out,
                      std::string& modelFilePath_out,
                      enum Test_I_ModeType& mode_out,
                      bool& traceInformation_out)
{
  std::string path_root =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
  contextSize_out = LLAMACPP_CONTEXT_SIZE_DEFAULT;
  numberOfGPULayers_out = LLAMACPP_NUMBER_OF_GPU_LAYERS_DEFAULT;
  modelFilePath_out.clear ();
  mode_out = TEST_I_MODE_DEFAULT;
  traceInformation_out = false;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT ("c:g:m:p:t"),
                               1,                         // skip command name
                               1,                         // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS, // ordering
                               0);                        // for now, don't use long options

  int option = 0;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        std::istringstream converter (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()),
                                      std::ios_base::in);
        converter >> contextSize_out;
        break;
      }
      case 'g':
      {
        std::istringstream converter (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()),
                                      std::ios_base::in);
        converter >> numberOfGPULayers_out;
        break;
      }
      case 'm':
      {
        modelFilePath_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 'p':
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

void
do_work (int argc_in,
         ACE_TCHAR* argv_in[],
         int contextSize_in,
         int numberOfGPULayers_in,
         const std::string& modelFilePath_in,
         enum Test_I_ModeType mode_in)
{
  // only print errors ?
  llama_log_set ([] (enum ggml_log_level level, const char * text, void * /* user_data */)
  {
#if defined (_DEBUG)
    if (level >= GGML_LOG_LEVEL_NONE)
#else
    if (level >= GGML_LOG_LEVEL_ERROR)
#endif // _DEBUG
      ACE_DEBUG (((level >= GGML_LOG_LEVEL_ERROR ? LM_ERROR : LM_DEBUG),
                  ACE_TEXT ("%s"),
                  ACE_TEXT (text)));
  },
  NULL);

  // load dynamic backends
  ggml_backend_load_all ();

  // initialize the model
  struct llama_model_params model_params = llama_model_default_params ();
  model_params.n_gpu_layers = numberOfGPULayers_in;
  llama_model* model =
    llama_model_load_from_file (modelFilePath_in.c_str (), model_params);
  if (!model)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to load model (was: \"%s\"), returning\n"),
                ACE_TEXT (modelFilePath_in.c_str ())));
    return;
  } // end IF
  const llama_vocab* vocab = llama_model_get_vocab (model);
  ACE_ASSERT (vocab);

  // initialize the context
  llama_context_params ctx_params = llama_context_default_params ();
  ctx_params.n_ctx = contextSize_in;
  ctx_params.n_batch = contextSize_in;
  llama_context* ctx = llama_init_from_model (model, ctx_params);
  if (!ctx)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to create model context, returning\n")));
    return;
  } // end IF

  // initialize the sampler
  llama_sampler* smpl =
    llama_sampler_chain_init (llama_sampler_chain_default_params ());
  ACE_ASSERT (smpl);
  llama_sampler_chain_add (smpl, llama_sampler_init_min_p (0.05f, 1));
  llama_sampler_chain_add (smpl, llama_sampler_init_temp (0.8f));
  llama_sampler_chain_add (smpl, llama_sampler_init_dist (LLAMA_DEFAULT_SEED));

  switch (mode_in)
  {
    case TEST_I_MODE_DEFAULT:
    {
      // helper function to evaluate a prompt and generate a response
      auto generate = [&] (const std::string& prompt)
      {
        std::string response;

        const bool is_first = llama_memory_seq_pos_max (llama_get_memory (ctx), 0) == -1;

        // tokenize the prompt
        const int n_prompt_tokens = -llama_tokenize (vocab, prompt.c_str (), prompt.size (), NULL, 0, is_first, true);
        std::vector<llama_token> prompt_tokens (n_prompt_tokens);
        if (llama_tokenize (vocab, prompt.c_str (), prompt.size (), prompt_tokens.data (), prompt_tokens.size (), is_first, true) < 0)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to tokenize the prompt (was: \"%s\"), returning\n"),
                      ACE_TEXT (prompt.c_str ())));
          return response;
        } // end IF

        // prepare a batch for the prompt
        llama_batch batch = llama_batch_get_one (prompt_tokens.data (), prompt_tokens.size ());
        llama_token new_token_id;
        while (true)
        {
          // check if we have enough space in the context to evaluate this batch
          int n_ctx = llama_n_ctx (ctx);
          int n_ctx_used = llama_memory_seq_pos_max (llama_get_memory (ctx), 0) + 1;
          if (n_ctx_used + batch.n_tokens > n_ctx)
          {
            printf ("\033[0m\n");
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("context size exceeded (was: %d, is: %d), returning\n"),
                        n_ctx_used + batch.n_tokens,
                        n_ctx));
            return response;
          } // end IF

          int ret = llama_decode (ctx, batch);
          if (ret != 0)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to decode the prompt (was: \"%s\"): %d, returning\n"),
                        ACE_TEXT (prompt.c_str ()),
                        ret));
            return response;
          } // end IF

          // sample the next token
          new_token_id = llama_sampler_sample (smpl, ctx, -1);

          // is it an end of generation?
          if (llama_vocab_is_eog (vocab, new_token_id))
            break;

          // convert the token to a string, print it and add it to the response
          char buf[256];
          int n = llama_token_to_piece (vocab, new_token_id, buf, sizeof (char[256]), 0, true);
          if (n < 0)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to convert token to piece (prompt was: \"%s\"): %d, returning\n"),
                        ACE_TEXT (prompt.c_str ()),
                        n));
            return response;
          } // end IF
          std::string piece(buf, n);
          printf ("%s", piece.c_str ());
          fflush (stdout);
          response += piece;

          // prepare the next batch with the sampled token
          batch = llama_batch_get_one(&new_token_id, 1);
        } // end WHILE

        return response;
      };

      std::vector<llama_chat_message> messages;
      std::vector<char> formatted (llama_n_ctx (ctx));
      int prev_len = 0;
      while (true)
      {
        // get user input
        printf ("\033[32m> \033[0m");
        std::string user;
        std::getline (std::cin, user);
        if (user.empty ())
          break;

        const char * tmpl = llama_model_chat_template (model, NULL);

        // add the user input to the message list and format it
        messages.push_back ({"user", strdup (user.c_str ())});
        int new_len = llama_chat_apply_template (tmpl, messages.data (), messages.size (), true, formatted.data (), formatted.size ());
        if (new_len > (int)formatted.size ())
        {
          formatted.resize (new_len);
          new_len = llama_chat_apply_template (tmpl, messages.data (), messages.size (), true, formatted.data (), formatted.size ());
        } // end IF
        if (new_len < 0)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to apply the chat template: %d, returning\n"),
                      new_len));
          break;
        } // end IF

        // remove previous messages to obtain the prompt to generate the response
        std::string prompt(formatted.begin() + prev_len, formatted.begin() + new_len);

        // generate a response
        printf("\033[33m");
        std::string response = generate(prompt);
        printf("\n\033[0m");

        // add the response to the messages
        messages.push_back({"assistant", strdup(response.c_str())});
        prev_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), false, nullptr, 0);
        if (prev_len < 0)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to apply the chat template: %d, returning\n"),
                      prev_len));
          break;
        } // end IF
      } // end WHILE

      // free resources
      for (struct llama_chat_message& msg: messages)
        free (const_cast<char*> (msg.content));

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

  // free resources
  llama_sampler_free (smpl);
  llama_free (ctx);
  llama_model_free (model);

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
  int context_size = LLAMACPP_CONTEXT_SIZE_DEFAULT;
  int number_of_GPU_layers = LLAMACPP_NUMBER_OF_GPU_LAYERS_DEFAULT;
  std::string model_path_string;
  enum Test_I_ModeType mode_type_e = TEST_I_MODE_DEFAULT;
  bool trace_information = false;
  std::string log_file_name;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
                             context_size,
                             number_of_GPU_layers,
                             model_path_string,
                             mode_type_e,
                             trace_information))
  {
    do_print_usage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
    goto clean;
  } // end IF

  if (!Common_File_Tools::isReadable (model_path_string))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(s), aborting\n")));
    do_print_usage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
    goto clean;
  } // end IF

  // step1c: initialize logging and/or tracing
  log_file_name =
    Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (Common_PACKAGE_NAME),
                                      ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));
  if (!Common_Log_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)), // program name
                                     log_file_name,              // log file name
                                     false,                      // log to syslog ?
                                     false,                      // trace messages ?
                                     trace_information,          // debug messages ?
                                     NULL))                      // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));
    goto clean;
  } // end IF

  timer.start ();
  // step2: do actual work
  do_work (argc_in,
           argv_in,
           context_size,
           number_of_GPU_layers,
           model_path_string,
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
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
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
  Common_Log_Tools::finalize ();
  // *PORTABILITY*: on Windows, finalize ACE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return result;
}
