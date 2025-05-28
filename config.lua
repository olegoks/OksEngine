
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
            StartWidth = 1920,
            StartHeight = 1080
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

ECSCallGraphEditor = {
    ecsFilesRootDirectory = './Sources/Engine/Sources'
}

scenesRootDirectory = './Resources/Scenes/'

Render = {
    shadersRootDirectory = './Resources/Shaders/',
    API = "Vulkan",
    Vulkan = {
        vertexShader = 'triangleVert.spv',
        fragmentShader = 'triangleFrag.spv'
    },
    Driver = {

    }
}
