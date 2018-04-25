#include "api-surface-bl.h"
#include "ilm/ilm_client.h"
#include "ilm/ilm_control.h"
#include <map>
#include <cstring>
#include <sstream>
#include "logger.h"

using namespace std;

/*
* Implementation for Weston with GENIVI wayland-ivi-extension (http://projects.genivi.org/wayland-ivi-extension/)
*
* The current implementation assumes a simple 1-to-1 mapping of surfaces to layers
* All changes are applied to the layer (e.g. setSize), not to the surface.
* By doing this we don't have to take care if the surface is really available or not.
*/

using namespace api_data::surface;
namespace api_bl
{
    namespace surface
    {
        typedef enum screens{
            MAIN_SCREEN_ID = 0
        }eScreenID;

        typedef enum {
            MAIN_LAYER_ID,
            CARPLAY_LAYER_ID,
            NAVIGATION_LAYER_ID,
            ANDROIDAUTO_LAYER_ID,
            CARLIFE_LAYER_ID,
            NUMBER_OF_LAYERS
        }eLayerID;

        typedef enum {
            CARPLAY_SURFACE_ID = 44,
            ANDROIDAUTO_SURFACE_ID = 46,
            CARLIFE_SURFACE_ID = 48,
            NAVIGATION_SURFACE_ID = 50,
            MAIN_SURFACE_ID = 100,
            INVALID_SURFACE
        }eSurfaceID;    

        typedef struct{
            eLayerID layerID;
            t_ilm_uint zPosition;
            t_ilm_uint x;
            t_ilm_uint y;
            t_ilm_uint width;
            t_ilm_uint height;
        }surfaceInformation;

        std::map<eSurfaceID, surfaceInformation> layerSurfaceMapping = {{MAIN_SURFACE_ID, {MAIN_LAYER_ID, 1, 0, 0, 1200, 720}},
                                                                        {CARPLAY_SURFACE_ID, {CARPLAY_LAYER_ID, 3, 0, 0, 1200, 720}},
                                                                        {NAVIGATION_SURFACE_ID, {NAVIGATION_LAYER_ID, 4, 0, 0, 1200, 720}},
                                                                        {ANDROIDAUTO_SURFACE_ID, {ANDROIDAUTO_LAYER_ID, 5, 0, 0, 1200, 720}},
                                                                        {CARLIFE_SURFACE_ID, {CARLIFE_LAYER_ID, 6, 0, 0, 1200, 720}}};

        bool is_numeric (const std::string& str, eSurfaceID &value)
        {
            std::istringstream ss(str);
            int val;
            ss >> val;      // try to read the number
            ss >> std::ws;  // eat whitespace after number

            if (!ss.fail() && ss.eof())
            {
                value = static_cast<eSurfaceID>(val);
                return true;  // is-a-number
            }
            else
            {
                return false; // not-a-number
            }
        }

        void surfaceNotificationHandler(ilmObjectType object, t_ilm_uint id, t_ilm_bool created, void* user_data)
        {
            LOG_INFO("SUR|", "surfaceNotificationHandler: object: ",  object , ", id: " , id , ", created: " , created);
            (void)user_data;

            if(object == ILM_SURFACE)
            {
                if(created)
                {
                    ilmErrorTypes callResult;
                    t_ilm_surface surfaceID = (t_ilm_surface)id;

                    if (layerSurfaceMapping.find(static_cast<eSurfaceID>(surfaceID)) != layerSurfaceMapping.end())
                    {
                        t_ilm_layer layerID = layerSurfaceMapping[static_cast<eSurfaceID>(surfaceID)].layerID;
                        callResult = ilm_layerSetRenderOrder((t_ilm_layer)layerID, &surfaceID, 1);
                        if (ILM_SUCCESS != callResult)
                        {
                            LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                            LOG_ERROR("Failed to add surface with ID " , surfaceID , " to layer " , layerID);
                        }

                        callResult = ilm_surfaceSetVisibility(surfaceID, 1);
                        if (ILM_SUCCESS != callResult)
                        {
                            LOG_ERROR("LayerManagerService returned: ", ILM_ERROR_STRING(callResult));
                            LOG_ERROR("Failed to set visibility for surface with ID " , surfaceID);
                        }
                        commit(NULL);
                    }
                }
                else
                {
                    //TODO: Handle destruction of surface
                }
            }
        }

        Manager *init()
        {
            //ILM
            ilmScreenProperties screenProperties;
            ilmErrorTypes callResult;
            unsigned int* array = NULL;
            int count = 0;

            ilm_init();

            callResult = ilm_getPropertiesOfScreen(MAIN_SCREEN_ID, &screenProperties);
            if (ILM_SUCCESS != callResult)
            {
                LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                LOG_ERROR("Failed to get properties of screen with ID " , MAIN_SCREEN_ID , " found");
            }


            // Create layers if they don't exist
            callResult = ilm_getLayerIDs(&count, &array);
            if (ILM_SUCCESS != callResult)
            {
                LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                LOG_ERROR("Failed to get layer IDs");
            }

            for (auto& kv : layerSurfaceMapping)
            {
                bool layerExists = false;

                for (int i = 0; i < count; ++i)
                {
                    if (array[i] == kv.second.layerID)
                    {
                        layerExists = true;
                        break;
                    }
                }

                if(!layerExists)
                {
                    t_ilm_layer layerID = kv.second.layerID;
                    callResult = ilm_layerCreateWithDimension(&layerID, screenProperties.screenWidth, screenProperties.screenHeight);
                    if (ILM_SUCCESS != callResult)
                    {
                        LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                        LOG_ERROR("Failed to create layer with ID " , kv.second.layerID);
                    }
                }
            }

            free(array);

            //Set default layer render order (doesn't work with wayland-ivi-extension < 1.5)
            {
                t_ilm_layer layerRenderOrder[] = {MAIN_LAYER_ID, CARPLAY_LAYER_ID, NAVIGATION_LAYER_ID, ANDROIDAUTO_LAYER_ID, CARLIFE_LAYER_ID};
                callResult = ilm_displaySetRenderOrder(MAIN_SCREEN_ID, layerRenderOrder, NUMBER_OF_LAYERS);
                if (ILM_SUCCESS != callResult)
                {
                    LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                    LOG_ERROR("Failed to set render order for screen with ID " , MAIN_SCREEN_ID);
                }
            }

            //Default visibility
            for (auto& kv : layerSurfaceMapping)
            {
                callResult = ilm_layerSetVisibility(kv.second.layerID, 0);
                if (ILM_SUCCESS != callResult)
                {
                    LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                    LOG_ERROR("Failed to set visibility for layer with ID " , kv.second.layerID);
                }
            }
            callResult = ilm_layerSetVisibility(MAIN_LAYER_ID, 1);
            if (ILM_SUCCESS != callResult)
            {
                LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                LOG_ERROR("Failed to set visibility 1 for layer with ID " , MAIN_LAYER_ID);
            }

            // Register notification callback
            callResult = ilm_registerNotification(surfaceNotificationHandler, NULL);
            if (ILM_SUCCESS != callResult)
            {
               LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                LOG_ERROR("Failed to set visibility 0 for layer with ID " , ANDROIDAUTO_LAYER_ID);
            }

            //TODO: Check for existing surfaces

            return nullptr;
        }

        void uninit(Manager *)
        {
            ilmErrorTypes callResult;

            for (auto& kv : layerSurfaceMapping)
            {
                callResult = ilm_layerRemove(kv.second.layerID);
                if (ILM_SUCCESS != callResult)
                {
                    LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                    LOG_ERROR("Failed to remove layer with ID " , MAIN_LAYER_ID);
                }
            }
        }

        void checkout(Manager *)
        {
        }

        void commit(Manager *)
        {
            ilmErrorTypes callResult = ilm_commitChanges();
            if (ILM_SUCCESS != callResult)
            {
                LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                LOG_ERROR("Commit failed");
            }
        }

        void setPosition(Manager *, const Name &name, const Position &pos)
        {
            eSurfaceID surfaceID = INVALID_SURFACE;

            if(is_numeric(name,surfaceID))
            {                
                if (layerSurfaceMapping.find(surfaceID) != layerSurfaceMapping.end())
                {
                    ilmErrorTypes callResult;

                    layerSurfaceMapping[surfaceID].x = pos.first;
                    layerSurfaceMapping[surfaceID].y = pos.second;

                    callResult = ilm_layerSetDestinationRectangle(layerSurfaceMapping[surfaceID].layerID,
                                                                  layerSurfaceMapping[surfaceID].x,
                                                                  layerSurfaceMapping[surfaceID].y,
                                                                  layerSurfaceMapping[surfaceID].width,
                                                                  layerSurfaceMapping[surfaceID].height);
                    if (ILM_SUCCESS != callResult)
                    {
                        LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                        LOG_ERROR("Failed to set destination rectangle for surface with ID " , surfaceID);
                    }
                }
            }
        }

        void setSize(Manager *, const Name &name, const Size &size)
        {
            eSurfaceID surfaceID = INVALID_SURFACE;

            if(is_numeric(name,surfaceID))
            {
                if (layerSurfaceMapping.find(surfaceID) != layerSurfaceMapping.end())
                {
                    ilmErrorTypes callResult;

                    layerSurfaceMapping[surfaceID].width = size.first;
                    layerSurfaceMapping[surfaceID].height = size.second;

                    callResult = ilm_layerSetDestinationRectangle(layerSurfaceMapping[surfaceID].layerID,
                                                                  layerSurfaceMapping[surfaceID].x,
                                                                  layerSurfaceMapping[surfaceID].y,
                                                                  layerSurfaceMapping[surfaceID].width,
                                                                  layerSurfaceMapping[surfaceID].height);
                    if (ILM_SUCCESS != callResult)
                    {
                        LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                        LOG_ERROR("Failed to set destination rectangle for surface with ID " , surfaceID);
                    }
                }
            }
        }

        void setOpacity(Manager *, const Name & windowTitle, Ramp ramp)
        {
            eSurfaceID surfaceID = INVALID_SURFACE;

            if(is_numeric(windowTitle, surfaceID))
            {
                if (layerSurfaceMapping.find(surfaceID) != layerSurfaceMapping.end())
                {
                    ilmErrorTypes callResult;
                    callResult = ilm_layerSetVisibility(layerSurfaceMapping[surfaceID].layerID, (ramp > 0) ? 1 : 0);
                    if (ILM_SUCCESS != callResult)
                    {
                        LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                        LOG_ERROR("Failed to set visibility 1 for surface with ID " , surfaceID);
                    }
                }
            }
        }

        void setCloseness(Manager *, const Name & windowTitle, Ramp ramp)
        {
            eSurfaceID surfaceID = INVALID_SURFACE;

            if(is_numeric(windowTitle, surfaceID))
            {
                if (layerSurfaceMapping.find(surfaceID) != layerSurfaceMapping.end())
                {
                    ilmErrorTypes callResult;

                    layerSurfaceMapping[surfaceID].zPosition = ramp;

                    t_ilm_layer layerRenderOrder[NUMBER_OF_LAYERS];

                    std::map<t_ilm_uint, eLayerID> layerOrder;
                    for (auto& kv : layerSurfaceMapping)
                    {
                        layerOrder[kv.second.zPosition] = kv.second.layerID;
                    }

                    for (auto& kv : layerOrder)
                    {
                        int i = 0;
                        if(i < NUMBER_OF_LAYERS)
                            layerRenderOrder[i] = kv.second;
                        i++;
                    }

                    //Set default layer render order (doesn't work with wayland-ivi-extension < 1.5)
                    callResult = ilm_displaySetRenderOrder(MAIN_SCREEN_ID, layerRenderOrder, NUMBER_OF_LAYERS);
                    if (ILM_SUCCESS != callResult)
                    {
                        LOG_ERROR("LayerManagerService returned: " , ILM_ERROR_STRING(callResult));
                        LOG_ERROR("Failed to set render order for screen with ID " , MAIN_SCREEN_ID);
                    }
                }
            }
        }
    }
}
