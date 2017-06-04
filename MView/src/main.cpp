#include <MView/MView.h>
#include <MView/App.h>

INT WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance_, LPSTR lpCmdLine_, INT iCmdShow_)
{
    if (!MViewInit(hInstance_)) {
        return -1;
    }

    const INT retval = MViewRun(800, 480);

    MViewDeinit();

    return retval;
}