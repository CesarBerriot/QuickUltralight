#include "quick_ultralight.h"
#include <windows.h>

#define assert(...) do { if(!(__VA_ARGS__)) exit(EXIT_FAILURE); } while(0)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void initialize(char developer_name[], char application_name[], char window_title[], int window_width, int window_height, bool is_window_size_fixed);
static void create_application(char developer_name[], char application_name[]);
static void create_window(char window_title[], int window_width, int window_height, bool is_window_size_fixed);
static void create_overlay(void);
static void run(void);
static void window_closed_callback(void * user_data, ULWindow);
static void window_resized_callback(void * user_data, ULWindow, unsigned int width, unsigned int height);
static void document_object_model_ready_callback(void * user_data, ULView, unsigned long long frame_id, bool is_main_frame, ULString url);
static void cleanup(void);

static ULApp application;
static ULWindow window;
static ULOverlay overlay;
static ULView view;

static qu_callback callbacks[QU_CALLBACK_MAX] = { NULL };

void qu_bind_callback(enum qu_callback_id id, qu_callback callback) { callbacks[id] = callback; }

ULApp qu_get_application(void) { return application; }

ULWindow qu_get_window(void) { return window; }

ULOverlay qu_get_overlay(void) { return overlay; }

ULView qu_get_view(void) { return view; }

void qu_initialize(char developer_name[], char application_name[], char window_title[], int window_width, int window_height, bool is_window_size_fixed)
{	initialize(developer_name, application_name, window_title, window_width, window_height, is_window_size_fixed);
}

void qu_run(void) { run(); }

void qu_cleanup(void) { cleanup(); }

static void initialize(char developer_name[], char application_name[], char window_title[], int window_width, int window_height, bool is_window_size_fixed)
{	create_application(developer_name, application_name);
	create_window(window_title, window_width, window_height, is_window_size_fixed);
	create_overlay();
}

static void create_application(char developer_name[], char application_name[])
{	ULString developer_name_string = ulCreateString(developer_name);
	ULString application_name_string = ulCreateString(application_name);

	ULSettings settings = ulCreateSettings();
	ulSettingsSetDeveloperName(settings,  developer_name_string);
	ulSettingsSetAppName(settings, application_name_string);

	ULConfig config = ulCreateConfig();

	application = ulCreateApp(settings, config);

	ulDestroyString(developer_name_string);
	ulDestroyString(application_name_string);

	ulDestroySettings(settings);

	ulDestroyConfig(config);
}
static void create_window(char window_title[], int window_width, int window_height, bool is_window_size_fixed)
{	window = ulCreateWindow(ulAppGetMainMonitor(application), window_width, window_height, false, kWindowFlags_Titled | (!is_window_size_fixed * (kWindowFlags_Resizable | kWindowFlags_Maximizable)));
	// Ultralight's AppCore provides no solution to allow minimization without maximization
	if(is_window_size_fixed)
	{	HWND win32_window = GetActiveWindow();
		assert(window);
		LONG styles = GetWindowLongA(win32_window, GWL_STYLE);
		assert(styles);
		assert(SetWindowLongA(win32_window, GWL_STYLE, styles | WS_MINIMIZEBOX));
	}
	ulWindowSetTitle(window, window_title);
	ulWindowSetCloseCallback(window, window_closed_callback, NULL);
	ulWindowSetResizeCallback(window, window_resized_callback, NULL);
}

static void create_overlay(void)
{	overlay = ulCreateOverlay(window, 0, 0, 0, 0);
	window_resized_callback(NULL, window, ulWindowGetWidth(window), ulWindowGetHeight(window));
	view = ulOverlayGetView(overlay);
	ulViewSetDOMReadyCallback(view, document_object_model_ready_callback, NULL);
	ULString url = ulCreateString("file:///app.html");
	ulViewLoadURL(view, url);
	ulDestroyString(url);
}

static void run(void) { ulAppRun(application); }

static void window_closed_callback(void * user_data, ULWindow window) { ulAppQuit(application); }

static void window_resized_callback(void * user_data, ULWindow window, unsigned int width, unsigned int height) { ulOverlayResize(overlay, width, height); }

static void document_object_model_ready_callback(void * user_data, ULView view, unsigned long long frame_id, bool is_main_frame, ULString url)
{	if(callbacks[QU_CALLBACK_DOM_LOADED])
		callbacks[QU_CALLBACK_DOM_LOADED]();
}

static void cleanup(void)
{	ulDestroyOverlay(overlay);
	ulDestroyWindow(window);
	ulDestroyApp(application);
}

void qu_register_javascript_function(char function_name[], JSObjectCallAsFunctionCallback callback)
{	JSStringRef function_name_javascript_string = JSStringCreateWithUTF8CString(function_name);
	JSContextRef context = ulViewLockJSContext(view);
	JSObjectRef global_object = JSContextGetGlobalObject(context);
	JSObjectRef javascript_callback = JSObjectMakeFunctionWithCallback(context, function_name_javascript_string, callback);
	JSObjectSetProperty(context, global_object, function_name_javascript_string, javascript_callback, 0, NULL);
	ulViewUnlockJSContext(view);
}

char * qu_evaluate_script(char script[])
{	ULString script_string = ulCreateString(script);
	ULString result = ulViewEvaluateScript(view, script_string, NULL);
	ulDestroyString(script_string);
	return ulStringGetData(result);
}

#pragma GCC diagnostic pop
