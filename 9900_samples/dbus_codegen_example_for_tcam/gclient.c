// Instead of using the generic GDBusProxy facilities, one can use the generated methods such as 
// my_app_frobber_call_hello_world() to invoke the net.Corp.MyApp.Frobber.HelloWorld() D-Bus method, 
// connect to the the ::notification GObject signal to receive the net.Corp.MyApp.Frobber::Notication 
// D-Bus signal and get/set the net.Corp.MyApp.Frobber:Verbose D-Bus Property using either the GObject 
// property :verbose or the my_app_get_verbose() and my_app_set_verbose() methods. Use the standard 
// "notify" signal to listen to property changes.

// Note that all property access is via GDBusProxy's property cache so no I/O is ever done when reading 
// properties. Also note that setting a property will cause the org.freedesktop.DBus.Properties.Set 
// method to be called on the remote object. This call, however, is asynchronous so setting a property 
// won't block. Further, the change is delayed and no error checking is possible. 

#include <dbus/dbus-glib.h>
#include "myapp-generated.h"

#define MY_APP_BUS_NAME                         "net.corp.MyApp"
#define MY_APP_SERVICE1_PATH                    "/net/corp/MyApp"

#define MY_APP_LOG_POS()                        g_print("%s()[%d]\n", __FUNCTION__, __LINE__)
#define MY_APP_LOG_G_ERROR(gerror)              g_print("%s()[%d]: error=\"%s\"\n", __FUNCTION__, __LINE__, gerror->message)

static void MyApp_call_hello_world_callback(
    GObject *source_object,
    GAsyncResult *res,
    gpointer user_data)
{
    gchar *out_response = NULL;
    GError *error = NULL;
    
    MY_APP_LOG_POS();
    my_app_net_corp_my_app_frobber_call_hello_world_finish(                                         // NOTE: this function must be called on finish the call.
        (MyAppNetCorpMyAppFrobber *)source_object, &out_response, res, &error);
    if (error != NULL) {
        MY_APP_LOG_G_ERROR(error);
        g_error_free(error);  
        error = NULL;
    }
    if (out_response != NULL) {
        g_print("call_hello_world_async()=\"%s\"\n", out_response);
        g_free(out_response);
    }
}

static gboolean MyApp_on_notification(
    MyAppNetCorpMyAppFrobber *object,
    const gchar *arg_icon_blob,
    gint arg_height,
    const gchar *const *arg_messages,
    gpointer user_data)
{
    const gchar *const * p= NULL;
    
    MY_APP_LOG_POS();
    
    g_print("%s(): arg_icon_blob=%s, arg_height=%d, arg_messages=[", __FUNCTION__, arg_icon_blob, arg_height);
    for (p = arg_messages; p != NULL && *p != NULL; p++) {
        if (p != arg_messages) {
            g_print(", ");
        }
        g_print("\"%s\"", *p);
    }
    g_print("]\n");
    return TRUE;
}

static void MyApp_on_g_properties_changed(
    GDBusProxy *proxy,
    GVariant *changed_properties,
    GStrv invalidated_properties,
    gpointer user_data)
{
    gboolean verbose;
    GVariantIter iter;
    GVariant *child;
    gchar *s = NULL;
    GVariant *v = NULL;

    MY_APP_LOG_POS();
    
    g_variant_iter_init (&iter, changed_properties);
    while ((child = g_variant_iter_next_value (&iter))) {
        g_variant_get(child, "{sv}", &s, &v);
        
        if (g_strcmp0(s, "Verbose") == 0) {
            verbose = my_app_net_corp_my_app_frobber_get_verbose((MyAppNetCorpMyAppFrobber *)proxy);
            g_print("verbose=%d\n", verbose);
        }

        g_free(s);
        g_variant_unref(v);
        g_variant_unref(child);
    }
}

static void MyApp_on_g_signal(
    GDBusProxy *proxy,
    gchar *sender_name,
    gchar *signal_name,
    GVariant *parameters,
    gpointer user_data)
{
    MY_APP_LOG_POS();
        
    if (g_strcmp0(signal_name, "Notification") == 0) {
        g_print("signal received: Notification\n");
    }
}

static void call_test_case(MyAppNetCorpMyAppFrobber *proxy)
{
    GError *error = NULL;
    
    MY_APP_LOG_POS();
    
    {
        MY_APP_LOG_POS();
        g_signal_connect(proxy, "notification", G_CALLBACK(MyApp_on_notification), NULL);                  // app signal from generated code
    }
    
    {
        MY_APP_LOG_POS();
        g_signal_connect(proxy, "g-properties-changed", G_CALLBACK(MyApp_on_g_properties_changed), NULL);  // general signal for properties changed
        g_signal_connect(proxy, "g-signal", G_CALLBACK(MyApp_on_g_signal), NULL);                          // general signal for signals
    }
    
    {
        MY_APP_LOG_POS();
        my_app_net_corp_my_app_frobber_call_hello_world(proxy, "John", NULL, &MyApp_call_hello_world_callback, NULL); // asynchronous call
    }
    
    {
        MY_APP_LOG_POS();
        gchar *out_response = NULL;
        my_app_net_corp_my_app_frobber_call_hello_world_sync(proxy, "Mary", &out_response, NULL, &error);             // synchronous call
        if (error != NULL) {
            MY_APP_LOG_G_ERROR(error);
            g_error_free(error);
            error = NULL;
        }
        if (out_response != NULL) {
            g_print("call_hello_world_sync()=\"%s\"\n", out_response);
            g_free(out_response);
            out_response = NULL;
        }
    }
    
    {
        MY_APP_LOG_POS();
        gboolean verbose = FALSE;
        
        verbose = my_app_net_corp_my_app_frobber_get_verbose(proxy);                                // get property
        g_print("verbose=%d\n", verbose);
        my_app_net_corp_my_app_frobber_set_verbose(proxy, !verbose);                                // set property; NOTE: asynchronous call.
        verbose = my_app_net_corp_my_app_frobber_get_verbose(proxy);                                // get property; NOTE: the value may be wrong at this time.
        g_print("verbose=%d\n", verbose);
    }
    
    {
        MY_APP_LOG_POS();
        GVariant *result = NULL;
        GVariant *value = NULL;
        gboolean verbose = FALSE;
        
        result = g_dbus_proxy_get_cached_property((GDBusProxy *)proxy, "Verbose");
        if (result != NULL) {
            verbose = g_variant_get_boolean(result);
            g_print("verbose=%d\n", verbose);
            g_variant_unref(result);
            result = NULL;
        }
        
        verbose = !verbose;
        value = g_variant_new_boolean(verbose);
        if (value != NULL) {
            g_dbus_proxy_set_cached_property((GDBusProxy *)proxy, "Verbose", value);
            value = NULL;                                                                           // can't call g_variant_unref() on this object.
        }
        
        result = g_dbus_proxy_get_cached_property((GDBusProxy *)proxy, "Verbose");
        if (result != NULL) {
            verbose = g_variant_get_boolean(result);
            g_print("verbose=%d\n", verbose);
            g_variant_unref(result);
            result = NULL;
        }
    }
}

gint main (gint argc, gchar *argv[])
{
    GMainLoop *loop = NULL;
    GError *error = NULL;
    MyAppNetCorpMyAppFrobber *proxy = NULL;
    
    MY_APP_LOG_POS();

    g_type_init();

    proxy = my_app_net_corp_my_app_frobber_proxy_new_for_bus_sync (
#ifdef USE_SESSION_BUS
        G_BUS_TYPE_SESSION,
#else
        G_BUS_TYPE_SYSTEM,
#endif
        G_DBUS_PROXY_FLAGS_NONE,
        MY_APP_BUS_NAME,                /* bus name */
        MY_APP_SERVICE1_PATH,           /* object */
        NULL,                          /* GCancellable* */
        &error
    );
    if (proxy != NULL) {
        call_test_case(proxy);
    }
    if (error != NULL) {
        MY_APP_LOG_G_ERROR(error);
        g_error_free(error);  
        error = NULL;
    }

    MY_APP_LOG_POS();
    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run(loop);
    
    MY_APP_LOG_POS();
    g_main_loop_unref (loop);
    g_object_unref (proxy);

    return 0;
}
