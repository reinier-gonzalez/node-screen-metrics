#include <node.h>
#include <node_version.h>
#include "Windows.h"

using namespace v8;

// TODO: Handle Exceptions!

BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{
    Handle<Array> * monitors = (Handle<Array>*)dwData;
    RECT * monitorCoordinates = (RECT*)lprcMonitor;

    Handle<Array> monitorData = Array::New();

    monitorData->Set( 0, Integer::New( (*monitorCoordinates).left ) );
    monitorData->Set( 1, Integer::New( (*monitorCoordinates).top ) );
    monitorData->Set( 2, Integer::New( (*monitorCoordinates).right - (*monitorCoordinates).left ) );
    monitorData->Set( 3, Integer::New( (*monitorCoordinates).bottom - (*monitorCoordinates).top  ) );

    (*monitors)->Set( (*monitors)->Length(), monitorData );

    return TRUE;
}

Handle<Array> displayMetrics()
{
  Handle<Array> monitors = Array::New();

  EnumDisplayMonitors( NULL, NULL, MonitorEnumProc, ( LPARAM ) &monitors );

  return monitors;
}

Handle<Array> totalResolution()
{
  Handle<Array> dimensions = Array::New( 2 );

  dimensions->Set( 0, Integer::New( GetSystemMetrics( SM_CXVIRTUALSCREEN ) ) );
  dimensions->Set( 1, Integer::New( GetSystemMetrics( SM_CYVIRTUALSCREEN ) ) );

  return dimensions;
}

Handle<Integer> monitorsCount()
{
	return Integer::New( GetSystemMetrics( SM_CMONITORS ) );
}

#if NODE_VERSION_AT_LEAST(0, 11, 3)

	void getDisplayMetrics( const v8::FunctionCallbackInfo<Value> &args )  { args.GetReturnValue().Set( displayMetrics() ); }
	void getTotalResolution( const v8::FunctionCallbackInfo<Value> &args ) { args.GetReturnValue().Set( totalResolution() ); }
	void getMonitorsCount( const v8::FunctionCallbackInfo<Value> &args )   { args.GetReturnValue().Set( monitorsCount() ); }
		
#else

	Handle<Value> getDisplayMetrics( const Arguments& args )  { HandleScope scope; return scope.Close( displayMetrics() ); }
	Handle<Value> getTotalResolution( const Arguments& args ) { HandleScope scope; return scope.Close( totalResolution() ); }
	Handle<Value> getMonitorsCount( const Arguments& args )   { HandleScope scope; return scope.Close( monitorsCount() ); }
	
#endif // NODE_VERSION_AT_LEAST


void RegisterModule( Handle<Object> exports )
{
	/* getDisplayMetrics() ===============================================================================================
	 Returns an array in the form [[x1,y1,w1,h1], [x2,y2,w2,h2], ..., [xN,yN,wN,hN]] 
	 with the resolutions and virtual positions of each monitor in the system.
	*/
  exports->Set( String::NewSymbol( "getDisplayMetrics" ),  FunctionTemplate::New( getDisplayMetrics )->GetFunction() );
  	
  /* getTotalResolution() ==============================================================================================
  	Returns an array in the form [width, height] with the total width and height of 
  	the virtual screen including all display monitors.
	*/
  exports->Set( String::NewSymbol( "getTotalResolution" ), FunctionTemplate::New( getTotalResolution )->GetFunction() );
  	
  /* getMonitorsCount() ================================================================================================
  	Returns the number of monitors in the system.
  */	
  exports->Set( String::NewSymbol( "getMonitorsCount" ),   FunctionTemplate::New( getMonitorsCount )->GetFunction() );
}

NODE_MODULE( screen_metrics, RegisterModule )