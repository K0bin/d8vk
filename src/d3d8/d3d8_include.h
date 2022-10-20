#pragma once

#ifndef _MSC_VER
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0A00
#endif

#include <stdint.h>

/*
// Undefine D3D8 macros //
#undef DIRECT3D_VERSION
#undef D3D_SDK_VERSION

#undef D3DCS_ALL            // parentheses added in DX9
#undef D3DFVF_POSITION_MASK // changed from 0x00E to 0x400E in DX9
#undef D3DFVF_RESERVED2     // reduced from 4 to 2 in DX9

#undef D3DSP_REGNUM_MASK    // changed from 0x00000FFF to 0x000007FF in DX9
#undef D3DVECTOR_DEFINED*/

#include <d3d9.h>

namespace d3d8 {
  #include <d3d8.h>
}

#include "../util/com/com_guid.h"
#include "../util/com/com_object.h"
#include "../util/com/com_pointer.h"

#include "../util/log/log.h"
#include "../util/log/log_debug.h"

#include "../util/rc/util_rc.h"
#include "../util/rc/util_rc_ptr.h"

#include "../util/sync/sync_recursive.h"

#include "../util/util_env.h"
#include "../util/util_enum.h"
#include "../util/util_error.h"
#include "../util/util_flags.h"
#include "../util/util_likely.h"
#include "../util/util_math.h"
#include "../util/util_misc.h"
#include "../util/util_string.h"
