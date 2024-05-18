//
// Created by cyolo on 5/4/24.
//

#include "xf_instruction_s.h"
#include <stdio.h>
#include <string.h>
#include <microhttpd.h>
#include <jansson.h>
#include <X11/Xlib.h>

#define POSTBUFFERSIZE 512
#define MAXNAMESIZE 20
#define MAXANSWERSIZE 512

#define GET 0
#define POST 1
#define PORT 8080

struct connection_info_struct
{
	int connectiontype;
	char* postdata;
	size_t postlen;
};

static int send_response(struct MHD_Connection* connection, unsigned int status_code,
                         const char* message)
{
	struct MHD_Response* response;
	int ret;

	response =
	    MHD_create_response_from_buffer(strlen(message), (void*)message, MHD_RESPMEM_PERSISTENT);
	if (!response)
		return MHD_NO;

	ret = MHD_queue_response(connection, status_code, response);
	MHD_destroy_response(response);

	return ret;
}

static enum MHD_Result answer_to_connection(void* cls, struct MHD_Connection* connection, const char* url,
                                const char* method, const char* version, const char* upload_data,
                                size_t* upload_data_size, void** con_cls)
{
	if (NULL == *con_cls)
	{
		struct connection_info_struct* con_info;

		con_info = malloc(sizeof(struct connection_info_struct));
		if (NULL == con_info)
			return MHD_NO;

		con_info->postlen = 0;
		con_info->postdata = malloc(POSTBUFFERSIZE);
		con_info->connectiontype = MHD_POSTDATA_KIND;
		*con_cls = (void*)con_info;

		return MHD_YES;
	}

	if (strcmp(method, "POST") == 0)
	{
		struct connection_info_struct* con_info = *con_cls;

		if (*upload_data_size != 0)
		{
			if ((con_info->postlen + *upload_data_size) > POSTBUFFERSIZE)
			{
				free(con_info->postdata);
				return MHD_NO;
			}
			memcpy(con_info->postdata + con_info->postlen, upload_data, *upload_data_size);
			con_info->postdata[con_info->postlen + *upload_data_size] = 0;
			con_info->postlen += *upload_data_size;
			*upload_data_size = 0;

			return MHD_YES;
		}

		if (con_info->postlen > 0)
		{
			unsigned int status_code;
			char* message;

			json_error_t error;
			json_t* root = json_loads(con_info->postdata, 0, &error);
			if (root)
			{
				json_t* scanCode = json_object_get(root, "scanCode");
				json_t* isDown = json_object_get(root, "isDown");

				if (!json_is_integer(scanCode))
				{
					status_code = MHD_HTTP_BAD_REQUEST;
					message = "'scanCode' not found in JSON body (or maybe its not an integer)";
				}
				else if (!json_is_boolean(isDown))
				{
					status_code = MHD_HTTP_BAD_REQUEST;
					message = "'isDown' not found in JSON body (or maybe its not a boolean)";
				}
				else
				{
					freerdp* instance = (freerdp*)cls;

					if (freerdp_input_send_keyboard_event_ex(instance->context->input,
					                                         json_boolean_value(isDown), FALSE,
					                                         json_integer_value(scanCode)))
					{
						status_code = MHD_HTTP_OK;
						message = "success";
					}
					else
					{
						status_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
						message = "freerdp_input_send_keyboard_event_ex did not succeed";
					}
				}

				json_decref(root);
			}
			else
			{
				status_code = MHD_HTTP_BAD_REQUEST;
				message = "body not properly formed JSON object";
			}

			free(con_info->postdata);
			return send_response(connection, status_code, message);
		}
	}

	return send_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED,
	                     "Invalid request method. Use POST method.");
}

static void request_completed(void* cls, struct MHD_Connection* connection, void** con_cls,
                              enum MHD_RequestTerminationCode toe)
{
	struct connection_info_struct* con_info = *con_cls;

	if (NULL == con_info)
		return;

	free(con_info);
}

void run_instruction_server(xfContext* xfc)
{
	struct MHD_Daemon* daemon;

	daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection,
	                          xfc->common.context.instance, MHD_OPTION_NOTIFY_COMPLETED,
	                          request_completed, NULL, MHD_OPTION_END);
	if (daemon == NULL)
	{
		fprintf(stderr, "Failed to start daemon\n");
		exit(1);
	}
}
