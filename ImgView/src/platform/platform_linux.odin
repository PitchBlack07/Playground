package platform

import "vendor:glfw"
import "core:log"

init :: proc() -> b32 { 
    if !glfw.Init() do return false
    
    if !glfw.VulkanSupported() {
        log.error("Vulkan is not supported on your machine.")        
    } else {
        log.debug("Vulkan supported!")
    }

    glfw.WindowHint(glfw.CLIENT_API, glfw.NO_API)
    glfw.WindowHint(glfw.RESIZABLE, glfw.FALSE)
    
    return true
}

deinit :: proc() {

}