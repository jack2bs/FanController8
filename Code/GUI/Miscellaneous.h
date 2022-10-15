#pragma once

#define TESTSTYLE \
            (wxSYSTEM_MENU | \
             wxRESIZE_BORDER | \
             wxMINIMIZE_BOX | \
             wxMAXIMIZE_BOX | \
             wxCLOSE_BOX | \
             wxCAPTION | \
             wxCLIP_CHILDREN)

#define TEXTBOXSTYLE_1 \
			(wxTE_MULTILINE | \
			 wxTE_READONLY | \
			 wxTE_LEFT | \
			 wxTE_WORDWRAP)