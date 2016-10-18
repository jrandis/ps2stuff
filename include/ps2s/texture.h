/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America

       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef ps2s_texture_h
#define ps2s_texture_h

/********************************************
 * includes
 */

// PLIN
//  #include "eetypes.h" // need to include eetypes.h for eestruct.h
//  #include "eestruct.h"

#include "ps2s/packet.h"
#include "ps2s/imagepackets.h"
#include "ps2s/types.h"
#include "ps2s/gs.h"

class CImageUploadPkt;
class CClutUploadPkt;

namespace GS {

   /********************************************
    * typedefs
    */

   namespace TexWrapMode {
      typedef enum {
	 kRepeat,
	 kClamp
      } tTexWrapMode;
   }
   using TexWrapMode::tTexWrapMode;

   namespace TexMode {
      typedef enum {
	 kModulate,
	 kDecal,
	 kHighlight,
	 kHighlight2
      } tTexMode;
   }
   using TexMode::tTexMode;

   namespace MagMode {
      typedef enum {
	 kNearest,
	 kLinear
      } tMagMode;
   }
   using MagMode::tMagMode;

   namespace MinMode {
      typedef enum {
	 kNearest,
	 kLinear,
	 kNearestMipmapNearest,
	 kNearestMipmapLinear,
	 kLinearMipmapNearest,
	 kLinearMipmapLinear
      } tMinMode;
   }
   using MinMode::tMinMode;

   /********************************************
    * CTexEnv
    */

   class CTexEnv
   {
      public:
	 // if you must use the short constructor, be sure to call SetDimensions() and SetPSM()!!
	 CTexEnv( GS::tContext context = GS::kContext1 );
	 CTexEnv( GS::tContext context, tU32 width, tU32 height, GS::tPSM psm );
	 virtual ~CTexEnv( void ) {}

	 // accessors

	 inline tU32 GetImageGsAddr( void ) const { return gsrTex0.TBP0 * 64; }
	 inline tU32 GetClutGsAddr( void ) const { return gsrTex0.CBP * 64; }
	 inline GS::tContext GetContext( void ) const;
	 inline GS::tPSM GetPSM( void ) const { return (GS::tPSM)gsrTex0.PSM; }
	 inline tU32 GetW( void ) const { return uiTexPixelWidth; }
	 inline tU32 GetH( void ) const { return uiTexPixelHeight; }

	 // mutators

	 inline void ClearRegion( void );
	 inline void SetClutLoadConditions( int mode ) { gsrTex0.CLD = mode; }
	 virtual void SetClutGsAddr( tU32 gsMemWordAddress );
	 void SetContext( GS::tContext context );
	 virtual void SetDimensions( tU32 w, tU32 h );
	 inline void SetFlush( bool flush );
	 virtual void SetImageGsAddr( tU32 gsMemWordAddress );
	 inline void SetMagMode( tMagMode newMode ) { gsrTex1.MMAG = (tU64)newMode; }
	 inline void SetMinMode( tMinMode newMode ) { gsrTex1.MMIN = (tU64)newMode; }
	 void SetPSM( GS::tPSM newPSM );
	 void SetRegion( tU32 originU, tU32 originV, tU32 w, tU32 h );
	 inline void SetTexMode( tTexMode newMode ) { gsrTex0.TFX = (tU64)newMode; }
	 void SetWrapModeS( tTexWrapMode sMode );
	 void SetWrapModeT( tTexWrapMode tMode );
	 inline void SetWrapMode( tTexWrapMode sMode, tTexWrapMode tMode ) {
	    SetWrapModeS( sMode );
	    SetWrapModeT( tMode );
	 }
	 inline void SetUseTexAlpha( bool useTexAlpha );

	 // other

	 void SendSettings( bool waitForEnd = false, bool flushCache = true );
	 void SendSettings( CSCDmaPacket& packet );
	 void SendSettings( CVifSCDmaPacket& packet );

	 // assignment
	 CTexEnv( const CTexEnv& rhs );
	 CTexEnv& operator = ( const CTexEnv& rhs );

	 inline void* operator new( size_t size ) {
	    return Core::New16(size);
	 }

      protected:
	 // gs packet to setup texture environment
	 tSourceChainTag SettingsDmaTag;
	 tGifTag		SettingsGifTag;
	 GS::tTexflush	gsrTexflush;	tU64 TexflushAddr;
	 GS::tClamp	gsrClamp;	tU64 ClampAddr;
	 GS::tTex1	gsrTex1;	tU64 Tex1Addr;
	 GS::tTex0	gsrTex0;	tU64 Tex0Addr;
	 GS::tTexa	gsrTexA;	tU64 TexAAddr;

	 tU32		uiNumSettingsGSRegs;
	 tU32		uiTexPixelWidth, uiTexPixelHeight;

	 CSCDmaPacket	SettingsPacket;

      private:
	 void InitCommon( GS::tContext context );

   } __attribute__ (( aligned(16) ));


   /********************************************
    * CTexture
    */

   // this class is meant to be subclassed, so it cannot
   // be instanciated.

   // this needs a lot of work, but it's pretty useful so I'll leave it
   // in here.  In particular the *UploadPkts might not have been a good
   // idea...

   class CTexture : public CTexEnv
   {
      public:
	 // mutators

	 virtual void SetImageGsAddr( tU32 gsMemWordAddress );
	 virtual void SetClutGsAddr( tU32 gsMemWordAddress );

	 // other

	 void SendImage( bool waitForEnd = false, bool flushCache = true );
	 void SendImage( CSCDmaPacket& packet );
	 void SendImage( CVifSCDmaPacket& packet );

	 void SendClut( bool waitForEnd = false, bool flushCache = true );
	 void SendClut( CSCDmaPacket& packet );
	 void SendClut( CVifSCDmaPacket& packet );

      protected:
	 tU128			*pImage, *pClut;
	 tU32			uiGsAddr;

	 CImageUploadPkt*	pImageUploadPkt;
	 CClutUploadPkt* 	pClutUploadPkt;

	 // be sure to call SetImage, SetImageGsAddr, and SetClutGsAddr if applicable
	 CTexture( GS::tContext context );
	 virtual ~CTexture( void );

	 tU128* AllocMem( tU32 w, tU32 h, GS::tPSM psm );
	 virtual void SetImage( tU128* imagePtr, tU32 w, tU32 h, GS::tPSM psm, tU32* clutPtr = NULL );

	 void Reset();

      private:
	 bool			bFreeMemOnExit;

	 // thou shalt not assign CTextures
	 CTexture( const CTexture& rhs );
	 CTexture& operator = ( const CTexture& rhs );

	 void InitCommon( GS::tContext context );

   } __attribute__ (( aligned(16) ));

   /********************************************
    * CClut
    */

   class CClut {
	 CClutUploadPkt		*UploadPkt;
	 unsigned int		GsAddr;
      public:
	 CClut( const void *table, int numEntries );
	 ~CClut() { delete UploadPkt; }

	 void SetGsAddr( unsigned int wordAddr ) {
	    GsAddr = wordAddr;
	    UploadPkt->SetGsAddr( wordAddr );
	 }
	 unsigned int GetGsAddr() const { return GsAddr; }

	 void Send( bool waitForEnd = false, bool flushCache = true ) {
	    UploadPkt->Send(waitForEnd, flushCache);
	 }
	 void Send( CSCDmaPacket& packet ) { UploadPkt->Send(packet); }
	 void Send( CVifSCDmaPacket &packet ) { UploadPkt->Send(packet); }
   };

   /********************************************
    * CCheckTex
    */

   class CCheckTex : public CTexture
   {
      public:
	 CCheckTex( GS::tContext context,
		    tU32 width, tU32 height,
		    tU32 xCellSize, tU32 yCellSize,
		    tU32 color1, tU32 color2 );
	 virtual ~CCheckTex( void ) {}

      private:
	 void MakeCheckerboard( tU32 xCellSize, tU32 yCellSize, tU32 color1, tU32 color2 );
   };


   /********************************************
    * CTexEnv inlines
    */

   inline GS::tContext
   CTexEnv::GetContext( void ) const {
      if ( ClampAddr == (unsigned int)GS::RegAddrs::clamp_1 ) return GS::kContext1;
      else return GS::kContext2;
   }

   inline void
   CTexEnv::SetFlush( bool flush ) {
      if ( flush ) TexflushAddr = GS::RegAddrs::texflush;
      else TexflushAddr = GS::RegAddrs::nop;
    }

   inline void
   CTexEnv::SetUseTexAlpha( bool useTexAlpha ) {
      gsrTex0.TCC = useTexAlpha;
   }

   inline void
   CTexEnv::ClearRegion( void )
   {
      if ( gsrClamp.WMS & 2 )
	 gsrClamp.WMS = 3 - gsrClamp.WMS;
      if ( gsrClamp.WMT & 2 )
	 gsrClamp.WMT = 3 - gsrClamp.WMT;
   }

} // namespace GS

#endif // ps2s_texture_h
