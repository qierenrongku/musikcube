#pragma once

#include "stdafx.h"

class IScrollAdapter {
    public:
        virtual void SetDisplaySize(size_t width, size_t height) = 0;
        virtual size_t GetLineCount(size_t width) = 0;
        virtual size_t GetEntryCount() = 0;
        virtual void DrawPage(WINDOW* window, size_t index) = 0;
};

