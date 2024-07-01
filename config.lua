
ResourceSystem = {
    resourcesRootDirectory = './Resources/',
    scriptsRootDirectory = './Sources/Scripts/'
}

UI = {
    Window = {
        Size = {
            MinWidth = 1280,
            MinHeight = 720,
            MaxWidth = 1920,
            MaxHeight = 1080,
            StartWidth = 960,
            StartHeight = 540
        },
        FullscreenMode = false,
        Title = "Vulkan"
    },
    Subsystem = "GLFW"
}

Logging = {
    Disable = false
}

Scripts = {
    scriptsRootDirectory = './Sources/Scripts/'
}

Render = {
    API = "Vulkan",
    Vulkan = {
        vertexShader = 'triangleVert.spv',
        fragmentShader = 'triangleFrag.spv'
    },
    Driver = {

    }
}
