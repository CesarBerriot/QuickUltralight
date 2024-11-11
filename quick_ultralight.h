#pragma once

#include <Ultralight/CAPI.h>
#include <AppCore/CAPI.h>
#include <JavaScriptCore/JavaScript.h>

typedef void(*qu_callback)(void);

enum qu_callback_id
{	QU_CALLBACK_DOM_LOADED,
	QU_CALLBACK_MAX
};

void qu_bind_callback(enum qu_callback_id, qu_callback);

ULApp qu_get_application(void);
ULWindow qu_get_window(void);
ULOverlay qu_get_overlay(void);
ULView qu_get_view(void);

void qu_initialize(char developer_name[], char application_name[], char window_title[], int window_width, int window_height, bool is_window_size_fixed);
void qu_run(void);
void qu_cleanup(void);

void qu_register_javascript_function(char function_name[], JSObjectCallAsFunctionCallback);
char * qu_evaluate_script(char script[]);
