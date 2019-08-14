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

#define MY_APP_BUS_NAME                         "net.corp.exapmle"
#define MY_APP_SERVER_NAME                      "/net/corp/exapmle"
#define MY_APP_SERVICE1_PATH                    "/net/corp/exapmle/Manager"

#define MY_APP_ERROR                            10
#define MY_APP_ERROR_NO_WHINING                 100

#define MY_APP_LOG_POS()                        g_print("%s()[%d]\n", __FUNCTION__, __LINE__)
#define MY_APP_LOG_G_ERROR(gerror)              g_print("%s()[%d]: error=\"%s\"\n", __FUNCTION__, __LINE__, gerror->message)

static GMainLoop         						*gMainLoop = NULL;
static gboolean                                 gVerbose = TRUE;

static gboolean on_handle_say_hello_world(
    exapmleManager *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_greeting)
{
    g_print("-------------------------------------------------------\n");
    
    if (g_strcmp0(arg_greeting, "Boo") != 0) {
        gchar *response;
        response = g_strdup_printf("Word! You said `%s'.", arg_greeting);
        exapmle_manager_complete_say_hello_world(object, invocation, response);          // NOTE: this function must be call on successful.
        g_free(response);
        
        {
            const char * const messages[] = {
                "message 1",
                "message 2",
                "message 3",
                NULL
            };
            exapmle_manager_emit_notify_info_changed(object, "icon1", 99, messages);     // emit a signal
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
    exapmleObjectSkeleton *object = NULL;
    GDBusObjectManagerServer *managerServer = NULL;
    exapmleManager *manager = NULL;
    
    MY_APP_LOG_POS();
    
    managerServer = g_dbus_object_manager_server_new(MY_APP_SERVER_NAME);
    MY_APP_LOG_POS();
    object = exapmle_object_skeleton_new(MY_APP_SERVICE1_PATH);
    MY_APP_LOG_POS();
    manager = exapmle_manager_skeleton_new();
    MY_APP_LOG_POS();
    exapmle_object_skeleton_set_manager(object, manager);
    MY_APP_LOG_POS();
    
    g_object_unref(manager);
    MY_APP_LOG_POS();
    
    exapmle_manager_set_verbose(manager, gVerbose);      // set a property.
    MY_APP_LOG_POS();
    g_print("verbose=%d\n", gVerbose);
    
    MY_APP_LOG_POS();
    g_signal_connect(
        manager,
        "handle-say-hello-world",
        G_CALLBACK (on_handle_say_hello_world),
        NULL
    );
    MY_APP_LOG_POS();
    if (error != NULL) {
        MY_APP_LOG_POS();
        MY_APP_LOG_G_ERROR(error);
        g_error_free(error);
        error = NULL;
    }
    
    MY_APP_LOG_POS();
    g_dbus_object_manager_server_export(managerServer, G_DBUS_OBJECT_SKELETON(object));
    MY_APP_LOG_POS();
    g_object_unref(object);
    
    MY_APP_LOG_POS();
    g_dbus_object_manager_server_set_connection(managerServer, connection);
}

static void MyApp_name_acquired_cb(GDBusConnection *connection, const gchar *bus_name, gpointer user_data)
{
    MY_APP_LOG_POS();
	g_print("Acquired bus name: %s\n", bus_name);
}

static void MyApp_lost_cb(GDBusConnection *connection, const gchar *bus_name, gpointer user_data)
{  
    MY_APP_LOG_POS();
	if (connection == NULL) {
        MY_APP_LOG_POS();
		g_print("Error: Failed to connect to dbus.\n"); 
	} else {
        MY_APP_LOG_POS();
		g_print("Error: Failed to obtain bus name: %s\n", bus_name);
	}

    MY_APP_LOG_POS();
	g_main_loop_quit(gMainLoop);
    MY_APP_LOG_POS();
}

gint main (gint argc, gchar *argv[])
{
	guint major = 2;
	guint minor = 32;
	guint micro = 4;
	gchar *result = NULL;
    MY_APP_LOG_POS();

	result = (gchar* )glib_check_version(major, minor, micro);
    MY_APP_LOG_POS();
	g_print("GLib Version Compability: %s\n", result);

    MY_APP_LOG_POS();
	g_type_init();
    
    MY_APP_LOG_POS();
	g_bus_own_name(
#ifdef USE_SESSION_BUS
        G_BUS_TYPE_SESSION,
#else
        G_BUS_TYPE_SYSTEM,
#endif
        MY_APP_BUS_NAME,
        G_BUS_NAME_OWNER_FLAGS_NONE,
        &MyApp_bus_acquried_cb,
        &MyApp_name_acquired_cb,
        &MyApp_lost_cb,
        NULL,
        NULL
    );
    MY_APP_LOG_POS();
    
    MY_APP_LOG_POS();
    gMainLoop = g_main_loop_new(NULL, FALSE);
    MY_APP_LOG_POS();
	g_main_loop_run(gMainLoop);
    MY_APP_LOG_POS();
	g_main_loop_unref(gMainLoop);
    MY_APP_LOG_POS();
    
    return 0;
}
