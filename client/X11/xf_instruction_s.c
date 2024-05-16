//
// Created by cyolo on 5/4/24.
//

#include "xf_instruction_s.h"
#include <stdio.h>
#include <string.h>
#include <microhttpd.h>
#include <jansson.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define PORT 8080
//
// static int handle_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method,
//                   const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
//     if (strcmp(method, "POST") == 0) {
//         // Read the POST data
//         char *post_data = NULL;
//         size_t post_data_size = *upload_data_size;
//         if (post_data_size > 0) {
//             post_data = malloc(post_data_size + 1);
//             strncpy(post_data, upload_data, post_data_size);
//             post_data[post_data_size] = '\0';
//         }
//
//         // Parse the JSON data
//         json_t *root = json_loads(post_data, 0, NULL);
//         if (!root) {
//             // Error parsing JSON
//             free(post_data);
//             return MHD_NO;
//         }
//
//         // Access JSON values
//         const char *key = "glossary";
//         json_t *value = json_object_get(root, key);
//         if (json_is_string(value)) {
//             printf("Received JSON object with key '%s' and value '%s'\n", key, json_string_value(value));
//         }
//
//         // Free memory
//         json_decref(root);
//         free(post_data);
//     }
//
//     return MHD_NO; // Return a response
// }

void simulateKeyPress(Display* display, Window* window, KeySym keysym) {
	XEvent event;

	event.xkey.type = KeyPress;
	event.xkey.keycode = XKeysymToKeycode(display, keysym);
	event.xkey.serial = 0;
	event.xkey.display = display;
	event.xkey.window = &window;
	event.xkey.root = DefaultRootWindow(display);
	event.xkey.subwindow = None;
	event.xkey.time = CurrentTime;
	event.xkey.x = 1;
	event.xkey.y = 1;
	event.xkey.x_root = 1;
	event.xkey.y_root = 1;
	event.xkey.state = 0;
	event.xkey.same_screen = True;

	XSendEvent(display, window, True, KeyPressMask, &event);
	XFlush(display);
}

static int handle_request(void *cls, struct MHD_Connection *connection,
                          const char *url, const char *method,
                          const char *version, const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {

	const char *page = "<html><body>Hello, World!</body></html>";

	struct MHD_Response *response;
	int ret;

//https://github.com/FreeRDP/FreeRDP/discussions/7973
	freerdp* instance = (freerdp*)cls;

	//simulateKeyPress(xfc->display, xfc->window->xfwin, 0x01c5);
	// UINT16 key_code = freerdp_keyboard_get_rdp_scancode_from_virtual_key_code(key);
	//instance->context->input->KeyboardEvent(instance->input, KBD_FLAGS_DOWN, key_code);
	// instance->input->KeyboardEvent(instance->input, KBD_FLAGS_DOWN, key_code);
	// instance->input->KeyboardEvent(instance->input, KBD_FLAGS_RELEASE, key_code);

	freerdp_input_send_keyboard_event_ex(instance->context->input, TRUE, FALSE, 30);
	freerdp_input_send_keyboard_event_ex(instance->context->input, FALSE, FALSE, 30);


	response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
	ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;
}



void run_instruction_server(xfContext* xfc) {
	struct MHD_Daemon *daemon;

	daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &handle_request, xfc->common.context.instance, MHD_OPTION_END);
	if (daemon == NULL) {
		fprintf(stderr, "Failed to start daemon\n");
		return;
	}

	// printf("Server running on port %d\n", PORT);
	// getchar();
	//
	// MHD_stop_daemon(daemon);
}


