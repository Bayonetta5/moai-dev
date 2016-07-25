// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISCRIPTNODE_H
#define	MOAISCRIPTNODE_H

#include <moai-sim/MOAINode.h>


//================================================================//
// MOAIScriptNode
//================================================================//
/**	@lua	MOAIScriptNode
	@text	User scriptable dependency node. User may specify Lua
			callback to handle node updating as well as custom floating
			point attributes.
*/
class MOAIScriptNode :
	public MOAINode {
private:

	MOAILuaMemberRef mOnUpdate;
	ZLLeanArray < float >	mAttributes;
	ZLLeanArray < cc8* >	mAttrNames;

	//----------------------------------------------------------------//
	static int		_reserveAttrs			( lua_State* L );
	static int		_setCallback			( lua_State* L );
	static int		_setAttrName			( lua_State* L );

protected:

	//----------------------------------------------------------------//
	void			OnDepNodeUpdate			();

public:
	
	DECL_LUA_FACTORY ( MOAIScriptNode )
	
	//----------------------------------------------------------------//
	bool			ApplyAttrOp				( u32 attrID, MOAIAttribute& attr, u32 op );
					MOAIScriptNode			();
					~MOAIScriptNode			();
	void			NamedAttrAdd			( u32 attrID, MOAIAttribute& attr );
	void			NamedAttrGet			( u32 attrID, MOAIAttribute& attr );
	void			NamedAttrSet			( u32 attrID, MOAIAttribute& attr );
	void			RegisterLuaClass		( MOAILuaState& state );
	void			RegisterLuaFuncs		( MOAILuaState& state );
};

#endif
