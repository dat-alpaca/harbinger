---@meta

--- * ---> Startup
---The main entry point for a project. This is normally where you specify the application's first scene and load its assets. 
function startup() end



--- * ---> Scenes
---@class scenes_t
scenes = {}

---Switches the current scene to the handle passed to the function
---@param handle integer The handle that describes the scene to switch to
function scenes.set_current_scene(handle) end



--- | ---> Assets
---@class assets_t
local assets = {}

---@enum asset_upload_options
asset_upload_options = {
    none = 0,
    upload_immediate = 1
}

--- Loads a texture from a filepath into the internal engine asset handler.
---@param filepath string The filepath to the texture
---@param options? { desired_channels?: integer, free_host_memory?: boolean } Load options
---@param asset_upload_options? asset_upload_options Whether to upload the texture to GPU immediately or defer it
---@return integer asset_handle The handle that describes the texture loaded
function assets.load_texture(filepath, options, asset_upload_options) end

--- Unloads a texture from the internal engine asset handler and its cache definitions
---@param filepath string The filepath to the texture
function assets.unload_texture(filepath) end

--- Unloads a texture from the internal engine asset handler and its cache definitions
---@param handle integer The asset_handle to unload
function assets.unload_texture(handle) end


--- Loads a scene from a filepath into the internal engine scene holder.
---@param filepath string The filepath to the scene
---@param setAsCurrent boolean Whether to immediately set the current scene to the newly created one
---@return integer scene_handle The handle that describes the scene loaded
function assets.load_scene(filepath, setAsCurrent) end

--- Loads the assets defined by a scene
---@param sceneHandle integer A scene_handle
function assets.load_scene_assets(sceneHandle) end



--- | ---> Engine
---@class engine
---@field assets assets_t
---@field scenes scenes_t
_engine = {}