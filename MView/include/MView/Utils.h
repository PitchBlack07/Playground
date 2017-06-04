#pragma once

template<typename T>
void SafeRelease(T*& value_)
{
    if (value_)
    {
        value_->Release();
        value_ = nullptr;
    }
}
