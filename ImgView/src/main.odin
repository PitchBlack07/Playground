package main
import "platform"
import "core:log"
import "core:os"

main :: proc() {
    context.logger = platform.init_logger()
    
    if !platform.init() {
        log.error("Failed to initialize platform layer")
        os.exit(-1)
    }
    defer platform.deinit()
}