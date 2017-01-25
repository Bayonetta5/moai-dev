// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICamera.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIFrameBufferTexture.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAILayer.h>
#include <moai-sim/MOAIMaterialStackMgr.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIPartitionResultMgr.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <moai-sim/MOAIViewProj.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
int MOAILayer::_draw ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAILayer, "U" )

	self->Render ();
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	getPropViewList
	@text	Return a list of props gathered and sorted by layer.
	
	@in		MOAILayer self
	@opt	number sortMode					Default is layer's current value.
	@opt	boolean sortInViewSpace			Default is layer's current value.
	@opt	number xSortScale				Default is layer's current value.
	@opt	number ySortScale				Default is layer's current value.
	@opt	number zSortScale				Default is layer's current value.
	@opt	number pSortScale				Priority sort scale. Default is layer's current value.
	@out	...								Gathered props.
*/
int	MOAILayer::_getPropViewList ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAILayer, "U" )
	
	MOAIPartition* partition = self->MOAIPartitionHolder::mPartition;
	
	if ( partition && self->mViewport ) {
		
		u32 interfaceMask = partition->GetInterfaceMask < MOAIGraphicsProp >();
		if ( !interfaceMask ) return 0;
		
		float sortScale [ 4 ];
		
		u32 sortMode			= state.GetValue < u32 >( 2, self->mSortMode );
		bool sortInViewSpace	= state.GetValue < bool >( 3, self->mSortInViewSpace );
		
		sortScale [ 0 ]			= state.GetValue < float >( 4, self->mSortScale [ 0 ]);
		sortScale [ 1 ]			= state.GetValue < float >( 5, self->mSortScale [ 1 ]);
		sortScale [ 2 ]			= state.GetValue < float >( 6, self->mSortScale [ 2 ]);
		sortScale [ 3 ]			= state.GetValue < float >( 7, self->mSortScale [ 3 ]);
		
		ZLMatrix4x4 viewMtx = MOAIViewProj::GetViewMtx ( self->mCamera, self->mParallax );
		ZLMatrix4x4 invViewProjMtx = viewMtx;
		invViewProjMtx.Append ( MOAIViewProj::GetProjectionMtx ( self->mViewport, self->mCamera ));
		invViewProjMtx.Inverse ();
	
		ZLFrustum viewVolume;
		viewVolume.Init ( invViewProjMtx );
		
		MOAIPartitionResultBuffer& buffer = MOAIPartitionResultMgr::Get ().GetBuffer ();
		
		u32 totalResults = 0;
		
		if ( self->mPartitionCull2D ) {
			totalResults = partition->GatherHulls ( buffer, 0, viewVolume.mAABB, interfaceMask );
		}
		else {
			totalResults = partition->GatherHulls ( buffer, 0, viewVolume, interfaceMask );
		}
		
		if ( !totalResults ) return 0;
		
		if ( sortInViewSpace ) {
			buffer.Transform ( viewMtx, false );
		}
		
		buffer.GenerateKeys (
			sortMode,
			sortScale [ 0 ],
			sortScale [ 1 ],
			sortScale [ 2 ],
			sortScale [ 3 ]
		);
		
		buffer.Sort ( self->mSortMode );
	
		buffer.PushHulls ( L );
		return totalResults;
	}
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	getSortMode
	@text	Get the sort mode for rendering.
	
	@in		MOAILayer self
	@out	number sortMode
*/
int MOAILayer::_getSortMode ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAILayer, "U" )
	
	lua_pushnumber ( state, self->mSortMode );
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	getSortScale
	@text	Return the scalar applied to axis sorts.
	
	@in		MOAILayer self
	@out	number x
	@out	number y
	@out	number priority
*/
int	MOAILayer::_getSortScale ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAILayer, "U" )

	lua_pushnumber ( state, self->mSortScale [ 0 ]);
	lua_pushnumber ( state, self->mSortScale [ 1 ]);
	lua_pushnumber ( state, self->mSortScale [ 3 ]);

	return 3;
}

//----------------------------------------------------------------//
/**	@lua	setPartitionCull2D
	@text	Enables 2D partition cull (projection of frustum AABB will
			be used instead of AABB or frustum).
	
	@in		MOAILayer self
	@in		boolean partitionCull2D		Default value is false.
	@out	nil
*/
int	MOAILayer::_setPartitionCull2D ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAILayer, "U" )

	self->mPartitionCull2D = state.GetValue < bool >( 2, false );

	return 0;
}

//----------------------------------------------------------------//
/**	@lua	setSortMode
	@text	Set the sort mode for rendering.
	
	@in		MOAILayer self
	@in		number sortMode				One of MOAILayer.SORT_NONE, MOAILayer.SORT_PRIORITY_ASCENDING,
										MOAILayer.SORT_PRIORITY_DESCENDING, MOAILayer.SORT_X_ASCENDING,
										MOAILayer.SORT_X_DESCENDING, MOAILayer.SORT_Y_ASCENDING,
										MOAILayer.SORT_Y_DESCENDING, MOAILayer.SORT_Z_ASCENDING,
										MOAILayer.SORT_Z_DESCENDING
	@in		boolean sortInViewSpace		Default value is 'false'.
	@out	nil
*/
int MOAILayer::_setSortMode ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAILayer, "U" )
	
	self->mSortMode			= state.GetValue < u32 >( 2, MOAIPartitionResultBuffer::SORT_PRIORITY_ASCENDING );
	self->mSortInViewSpace	= state.GetValue < bool >( 3, false );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	setSortScale
	@text	Set the scalar applied to axis sorts.
	
	@in		MOAILayer self
	@opt	number x			Default value is 0.
	@opt	number y			Default value is 0.
	@opt	number z			Default value is 0.
	@opt	number priority		Default value is 1.
	@out	nil
*/
int	MOAILayer::_setSortScale ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAILayer, "U" )

	self->mSortScale [ 0 ] = state.GetValue < float >( 2, 0.0f );
	self->mSortScale [ 1 ] = state.GetValue < float >( 3, 0.0f );
	self->mSortScale [ 2 ] = state.GetValue < float >( 4, 0.0f );
	self->mSortScale [ 3 ] = state.GetValue < float >( 5, 1.0f );

	return 0;
}

//================================================================//
// MOAILayer
//================================================================//

//----------------------------------------------------------------//
void MOAILayer::DrawPartition ( MOAIPartition& partition ) {

	MOAIGfxMgr& gfxMgr = MOAIGfxMgr::Get ();

	u32 interfaceMask = partition.GetInterfaceMask < MOAIDrawable >();
	if ( !interfaceMask ) return;
	
	MOAIPartitionResultBuffer& buffer = MOAIPartitionResultMgr::Get ().GetBuffer ();
	const ZLFrustum& viewVolume = gfxMgr.mGfxState.GetViewVolume ();
	
	u32 totalResults = 0;
	
	if ( this->mPartitionCull2D ) {
		totalResults = partition.GatherHulls ( buffer, 0, viewVolume.mAABB, interfaceMask );
	}
	else {
		totalResults = partition.GatherHulls ( buffer, 0, viewVolume, interfaceMask );
	}
	
	if ( !totalResults ) return;
	
	if ( this->mSortInViewSpace ) {
		buffer.Transform ( gfxMgr.mGfxState.GetMtx ( MOAIGfxGlobalsCache::WORLD_TO_VIEW_MTX ), false );
	}
	
	buffer.GenerateKeys (
		this->mSortMode,
		this->mSortScale [ 0 ],
		this->mSortScale [ 1 ],
		this->mSortScale [ 2 ],
		this->mSortScale [ 3 ]
	);
	
	buffer.Sort ( this->mSortMode );
	
	MOAIMaterialStackMgr& materialStack = MOAIMaterialStackMgr::Get ();
	materialStack.Push ( this->GetMaterial ());
	
	this->DrawProps ( buffer );
	
	materialStack.Pop ();
	
	if ( MOAIDebugLinesMgr::Get ().IsVisible () && this->mShowDebugLines ) {
		
		this->DrawPropsDebug ( buffer );
	}
}

//----------------------------------------------------------------//
void MOAILayer::DrawProps ( MOAIPartitionResultBuffer& buffer ) {

	u32 totalResults = buffer.GetTotalResults ();

	for ( u32 i = 0; i < totalResults; ++i ) {
		MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
		MOAIDrawable* drawable = result->AsType < MOAIDrawable >();
		drawable->Draw ( result->mSubPrimID );
	}
}

//----------------------------------------------------------------//
void MOAILayer::DrawPropsDebug ( MOAIPartitionResultBuffer& buffer ) {

	u32 totalResults = buffer.GetTotalResults ();

	for ( u32 i = 0; i < totalResults; ++i ) {
		MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
		MOAIDrawable* drawable = result->AsType < MOAIDrawable >();
		drawable->DrawDebug ( result->mSubPrimID );
	}
}

//----------------------------------------------------------------//
MOAILayer::MOAILayer () :
	mSortMode ( MOAIPartitionResultBuffer::SORT_PRIORITY_ASCENDING ),
	mSortInViewSpace ( false ),
	mPartitionCull2D ( true ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIPartitionHolder )
		RTTI_EXTEND ( MOAILayerBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAILayer::~MOAILayer () {
}

//----------------------------------------------------------------//
void MOAILayer::RegisterLuaClass ( MOAILuaState& state ) {

	MOAIPartitionHolder::RegisterLuaClass ( state );
	MOAILayerBase::RegisterLuaClass ( state );
	
	state.SetField ( -1, "SORT_NONE",						( u32 )MOAIPartitionResultBuffer::SORT_NONE );
	state.SetField ( -1, "SORT_ISO",						( u32 )MOAIPartitionResultBuffer::SORT_ISO );
	state.SetField ( -1, "SORT_PRIORITY_ASCENDING",			( u32 )MOAIPartitionResultBuffer::SORT_PRIORITY_ASCENDING );
	state.SetField ( -1, "SORT_PRIORITY_DESCENDING",		( u32 )MOAIPartitionResultBuffer::SORT_PRIORITY_DESCENDING );
	state.SetField ( -1, "SORT_X_ASCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_X_ASCENDING );
	state.SetField ( -1, "SORT_X_DESCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_X_DESCENDING );
	state.SetField ( -1, "SORT_Y_ASCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_Y_ASCENDING );
	state.SetField ( -1, "SORT_Y_DESCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_Y_DESCENDING );
	state.SetField ( -1, "SORT_Z_ASCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_Z_ASCENDING );
	state.SetField ( -1, "SORT_Z_DESCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_Z_DESCENDING );
	state.SetField ( -1, "SORT_VECTOR_ASCENDING",			( u32 )MOAIPartitionResultBuffer::SORT_VECTOR_ASCENDING );
	state.SetField ( -1, "SORT_VECTOR_DESCENDING",			( u32 )MOAIPartitionResultBuffer::SORT_VECTOR_DESCENDING );
	state.SetField ( -1, "SORT_DIST_SQUARED_ASCENDING",		( u32 )MOAIPartitionResultBuffer::SORT_DIST_SQUARED_ASCENDING );
	state.SetField ( -1, "SORT_DIST_SQUARED_DESCENDING",	( u32 )MOAIPartitionResultBuffer::SORT_DIST_SQUARED_DESCENDING );
}

//----------------------------------------------------------------//
void MOAILayer::RegisterLuaFuncs ( MOAILuaState& state ) {
	
	MOAIPartitionHolder::RegisterLuaFuncs ( state );
	MOAILayerBase::RegisterLuaFuncs ( state );
	
	luaL_Reg regTable [] = {
		{ "draw",					_draw },
		{ "getPropViewList",		_getPropViewList },
		{ "getSortMode",			_getSortMode },
		{ "getSortScale",			_getSortScale },
		{ "setPartitionCull2D",		_setPartitionCull2D },
		{ "setSortMode",			_setSortMode },
		{ "setSortScale",			_setSortScale },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAILayer::SerializeIn ( MOAILuaState& state, MOAIDeserializer& serializer ) {
	MOAIGraphicsProp::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAILayer::SerializeOut ( MOAILuaState& state, MOAISerializer& serializer ) {
	MOAIGraphicsProp::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAILayer::MOAILayerBase_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );
	
	if ( this->MOAIPartitionHolder::mPartition ) {
		this->DrawPartition ( *this->MOAIPartitionHolder::mPartition );
	}
}
