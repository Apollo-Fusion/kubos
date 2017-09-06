/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "evented-control/ecp.h"
#include <dbus/dbus.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This is the endpoint used by nanomsg for pub/sub broadcast messages.
** Eventually we'll want to read this from a configuration file, but
** hardcoding it is fine for the near term.
*/
char * broadcast_endpoint = "tcp://127.0.0.1:25901";

/**
 * Initialize an ECP Context
 *
*/

tECP_Error ECP_Init(tECP_Context * context, const char * name, tECP_Callback callback)
{
    tECP_Error err = ECP_E_NOERR;
    DBusError  error;
    int        i = 0;

    /* Initialize context to known state */
    context->talk       = -1;
    context->listen     = -1;
    context->talk_id    = -1;
    context->listen_id  = -1;
    context->callbacks  = NULL;
    context->connection = NULL;

    do
    {
        dbus_error_init(&error);
        context->connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
        if (NULL == context->connection)
        {
            err = ECP_E_GENERIC;
            break;
        }

        if (0 > dbus_bus_request_name(context->connection, name, 0, &error))
        {
            err = ECP_E_GENERIC;
            break;
        }

        if (!dbus_connection_add_filter(context->connection, callback, NULL, NULL))
        {
            err = ECP_E_GENERIC;
            break;
        }
    } while (0);

    dbus_error_free(&error);

    return (err);
}

tECP_Error ECP_Listen(tECP_Context * context, const char * channel)
{
    tECP_Error err = ECP_E_NOERR;
    DBusError  error;
    char       sig_match_str[100];

    sprintf(sig_match_str, "type='signal',interface='%s'", channel);

    do
    {
        dbus_error_init(&error);
        dbus_bus_add_match(context->connection, sig_match_str, &error);

        if (dbus_error_is_set(&error))
        {
            fprintf(stderr, "Name Error (%s)\n", error.message);
            dbus_error_free(&error);
            err = ECP_E_GENERIC;
            break;
        }

        dbus_connection_flush(context->connection);
    } while (0);

    return err;
}

/**
 ** Block until a message is received or the timeout (in microseconds)
 expires
 */
tECP_Error ECP_Loop( tECP_Context * context, unsigned int timeout ) {
    tECP_Error err = ECP_E_NOERR;

    dbus_connection_read_write_dispatch(context->connection, timeout);

    return err;
}

/**
 ** Release resources allocated by ECP_Init()
 */
tECP_Error ECP_Destroy( tECP_Context * context ) {
  tECP_Error err = ECP_E_NOERR;

  /** Need to figure out what d-bus wants us to clean up...
    * It looks like dbus_connection_close isn't needed since
    * we are using dbus_bus_get
    */

  return( err );
}

/**
 ** Send a broadcast message on a pub-sub channel. Note: messages are
 ** broadcast immediately and don't wait for a call to ECP_Loop().
 */

#define BUFFER_SIZE ( sizeof( uint16_t ) + sizeof( tECP_Message ) )

tECP_Error ECP_Broadcast( tECP_Context * context, const char * signal_interface,
                          const char * signal_path, const char * signal_member,
                          const char * message ) {
  tECP_Error err = ECP_E_NOERR;
  DBusMessage * msg;
  DBusMessageIter args;
  dbus_uint32_t serial = 0;

  msg = dbus_message_new_signal(signal_path, signal_interface, signal_member);
  
  dbus_message_iter_init_append(msg, &args);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &message);
  if (!dbus_connection_send(context->connection, msg, &serial))
  {
      err = ECP_E_GENERIC;
  }

  return( err );
}