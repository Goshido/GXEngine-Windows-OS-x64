//version 1.0
//No include guard allowed.
//Not include this explicitly! Use GXCommon/GXDisableWarning.h instead.


#define GX_DISABLE_WARNING(command) \
	_Pragma ( ##command )
