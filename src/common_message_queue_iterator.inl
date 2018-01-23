
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <ACE_SYNCH_DECL,
          class TIME_POLICY>
Common_MessageQueueIterator_T<ACE_SYNCH_USE,
                              TIME_POLICY>::Common_MessageQueueIterator_T (MESSAGE_QUEUE_T& queue_in)
 : currentMessage_ (NULL)
 , queue_ (queue_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueIterator_T::Common_MessageQueueIterator_T"));

}

template <ACE_SYNCH_DECL,
          class TIME_POLICY>
int
Common_MessageQueueIterator_T<ACE_SYNCH_USE,
                              TIME_POLICY>::next (ACE_Message_Block*& message_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueIterator_T::next"));

  if (likely (this->currentMessage_))
  {
    message_out = this->currentMessage_;
    return 1;
  } // end IF

  return 0;
}

template <ACE_SYNCH_DECL,
          class TIME_POLICY>
int
Common_MessageQueueIterator_T<ACE_SYNCH_USE, TIME_POLICY>::advance (void)
{
  COMMON_TRACE (ACE_TEXT ("Common_MessageQueueIterator_T::advance"));

  if (likely (this->currentMessage_))
    this->currentMessage_ = this->currentMessage_->next ();

  return (this->currentMessage_ ? 1 : 0);
}
