function startup()
    local mainScene = _engine.assets.load_scene("res/scenes/example_scene.json", true)
    _engine.assets.load_scene_assets(mainScene)

    --local cat = _engine.assets.load_texture("res/textures/cat.jpg", {
    --    free_host_memory = true,
    --    desired_channels = 0
    --}, asset_upload_options.upload_immediate)
end