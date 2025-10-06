package platform
import "core:log"

init_logger :: proc() -> log.Logger {
    return log.create_console_logger()
}