// The generated MyAppFrobber interface is designed so it is easy to implement it in a GObject subclass. 
// For example, to handle HelloWorld() method invocations, set the vfunc for handle_hello_hello_world() 
// in the MyAppFrobberIface structure. Similary, to handle the net.Corp.MyApp.Frobber:Verbose property 
// override the :verbose GObject property from the subclass. To emit a signal, use e.g. my_app_emit_signal() 
// or g_signal_emit_by_name().

// Instead of subclassing, it is often easier to use the generated MyAppFrobberSkeleton subclass. To 
// handle incoming method calls, use g_signal_connect() with the ::handle-* signals and instead of 
// overriding GObject's get_property() and set_property() vfuncs, use g_object_get() and g_object_set() 
// or the generated property getters and setters (the generated class has an internal property bag 
// implementation). 

#include <dbus/dbus-glib.h>
#include "myapp-generated.h"

#define MY_APP_BUS_NAME                         "net.corp.MyApp"
#define MY_APP_SERVICE1_PATH                    "/net/corp/MyApp"

#define MY_APP_ERROR                            10
#define MY_APP_ERROR_NO_WHINING                 100

#define MY_APP_LOG_POS()                        g_print("%s()[%d]\n", __FUNCTION__, __LINE__)
#define MY_APP_LOG_G_ERROR(gerror)              g_print("%s()[%d]: error=\"%s\"\n", __FUNCTION__, __LINE__, gerror->message)

static GMainLoop         						*gMainLoop = NULL;
static gboolean                                 gVerbose = TRUE;

static gboolean on_handle_hello_world(
    MyAppNetCorpMyAppFrobber *interface, 
    GDBusMethodInvocation *invocation, 
    const gchar *greeting, 
    gpointer user_data)
{
    if (g_strcmp0(greeting, "Boo") != 0) {
        gchar *response;
        response = g_strdup_printf("Word! You said `%s'.", greeting);
        my_app_net_corp_my_app_frobber_complete_hello_world(interface, invocation, response);       // NOTE: this function must be call on successfully.
        g_free(response);
        
        {
            const char * const messages[] = {
                "message 1",
                "message 2",
                "message 3",
                NULL
            };
            my_app_net_corp_my_app_frobber_emit_notification(interface, "icon1", 99, messages);    // emit a signal
        }
    } else {
        g_dbus_method_invocation_return_error(
            invocation,
            MY_APP_ERROR,
            MY_APP_ERROR_NO_WHINING,
            "Hey, %s, there will be no whining!",
            g_dbus_method_invocation_get_sender(invocation)
        );
    }
    return TRUE;
}

static void MyApp_bus_acquried_cb(GDBusConnection *connection, const gchar *bus_name, gpointer user_data)
{
    GError *error = NULL;
    MyAppNetCorpMyAppFrobber *pMyAppNetCorpMyAppFrobberInterface = NULL;
    pMyAppNetCorpMyAppFrobberInterface = my_app_net_corp_my_app_frobber_skeleton_new();
    
    my_app_net_corp_my_app_frobber_set_verbose(pMyAppNetCorpMyAppFrobberInterface, gVerbose);      // set a property.
    g_print("verbose=%d\n", gVerbose);
    
    g_signal_connect(
        pMyAppNetCorpMyAppFrobberInterface,
        "handle-hello-world",
        G_CALLBACK (on_handle_hello_world),
        NULL
    );
    if (error != NULL) {
        MY_APP_LOG_G_ERROR(error);
        g_error_free(error);
        error = NULL;
    }
    
    g_dbus_interface_skeleton_export(
        G_DBUS_INTERFACE_SKELETON(pMyAppNetCorpMyAppFrobberInterface),
        connection,
        MY_APP_SERVICE1_PATH,
        &error
    );
    if (error != NULL) {
        MY_APP_LOG_G_ERROR(error);
        g_error_free(error);
        error = NULL;
    }
}

static void MyApp_acquired_cb(GDBusConnection *connection, const gchar *bus_name, gpointer user_data)
{
	g_print("Acquired bus name: %s\n", bus_name);
}

static void MyApp_lost_cb(GDBusConnection *connection, const gchar *bus_name, gpointer user_data)
{  
	if (connection == NULL) {
		g_print("Error: Failed to connect to dbus.\n"); 
	} else {    
		g_print("Error: Failed to obtain bus name: %s\n", bus_name);
	}

	g_main_loop_quit(gMainLoop);
}

gint main (gint argc, gchar *argv[])
{
	guint major = 2;
	guint minor = 32;
	guint micro = 4;
	gchar *result = NULL;

	result = (gchar* )glib_check_version(major, minor, micro);
	g_print("GLib Version Compability: %s\n", result);

	g_type_init();
    
	g_bus_own_name(
#ifdef USE_SESSION_BUS
        G_BUS_TYPE_SESSION,
#else
        G_BUS_TYPE_SYSTEM,
#endif
        MY_APP_BUS_NAME,
        G_BUS_NAME_OWNER_FLAGS_NONE,
        &MyApp_bus_acquried_cb,
        &MyApp_acquired_cb,
        &MyApp_lost_cb,
        NULL,
        NULL
    );

	//g_timeout_add(1000, &timer, NULL);
    
    gMainLoop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(gMainLoop);
	g_main_loop_unref(gMainLoop);
    
    return 0;
}
