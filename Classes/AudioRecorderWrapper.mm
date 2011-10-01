//
//  AudioRecorderWrapper.m
//  Steganography
//
//  Created by Faizan Aziz on 28/04/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "AudioRecorderWrapper.h"


@implementation AudioRecorderWrapper
@synthesize recorder;
@synthesize isRecording;

#pragma mark AudioSession listeners
void interruptionListener(	void *	inClientData,
						  UInt32	inInterruptionState)
{
	AudioRecorderWrapper *THIS = (AudioRecorderWrapper*)inClientData;
	if (inInterruptionState == kAudioSessionBeginInterruption)
	{
		if (THIS->recorder->IsRunning()) {
			[THIS stopRecording];
		}
		
	}
}

void propListener(	void *                  inClientData,
				  AudioSessionPropertyID	inID,
				  UInt32                  inDataSize,
				  const void *            inData)
{
	AudioRecorderWrapper *THIS = (AudioRecorderWrapper*)inClientData;
	if (inID == kAudioSessionProperty_AudioRouteChange)
	{
		CFDictionaryRef routeDictionary = (CFDictionaryRef)inData;			
		//CFShow(routeDictionary);
		CFNumberRef reason = (CFNumberRef)CFDictionaryGetValue(routeDictionary, CFSTR(kAudioSession_AudioRouteChangeKey_Reason));
		SInt32 reasonVal;
		CFNumberGetValue(reason, kCFNumberSInt32Type, &reasonVal);
		if (reasonVal != kAudioSessionRouteChangeReason_CategoryChange)
		{
			/*CFStringRef oldRoute = (CFStringRef)CFDictionaryGetValue(routeDictionary, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
			 if (oldRoute)	
			 {
			 printf("old route:\n");
			 CFShow(oldRoute);
			 }
			 else 
			 printf("ERROR GETTING OLD AUDIO ROUTE!\n");
			 
			 CFStringRef newRoute;
			 UInt32 size; size = sizeof(CFStringRef);
			 OSStatus error = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &newRoute);
			 if (error) printf("ERROR GETTING NEW AUDIO ROUTE! %d\n", error);
			 else
			 {
			 printf("new route:\n");
			 CFShow(newRoute);
			 }*/
			
			
			// stop the queue if we had a non-policy route change
			if (THIS->recorder->IsRunning()) {
				[THIS stopRecording];
			}
		}	
	}
	else if (inID == kAudioSessionProperty_AudioInputAvailable)
		NSLog(@"Can't record");
}

-(id)init{
	if(self = [super init]){
		
		recorder = new AQRecorder();
		
		OSStatus error = AudioSessionInitialize(NULL, NULL, interruptionListener, self);
		if (error) printf("ERROR INITIALIZING AUDIO SESSION! %d\n", (int)error);
		else 
		{
			UInt32 category = kAudioSessionCategory_PlayAndRecord;	
			error = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
			if (error) printf("couldn't set audio category!");
			
			error = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, propListener, self);
			if (error) printf("ERROR ADDING AUDIO SESSION PROP LISTENER! %d\n",(int)error);
			UInt32 inputAvailable = 0;
			UInt32 size = sizeof(inputAvailable);
			
			// we do not want to allow recording if input is not available
			error = AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &size, &inputAvailable);
			if (error) printf("ERROR GETTING INPUT AVAILABILITY! %d\n", (int)error);
			
			// we also need to listen to see if input availability changes
			error = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioInputAvailable, propListener, self);
			if (error) printf("ERROR ADDING AUDIO SESSION PROP LISTENER! %d\n", (int)error);
			
			error = AudioSessionSetActive(true); 
			if (error) printf("AudioSessionSetActive (true) failed");
		}
		
		isRecording = NO;
	}
	return self;
}

- (void)startNewRecordingWithFileName:(NSString *)aFileName{
	// Start the recorder
	recorder->StartRecord((CFStringRef)aFileName);
	isRecording = YES;
	NSLog(@"Recording start");
	
}

- (void)stopRecording{
	
	recorder->StopRecord();
	isRecording = NO;
	NSLog(@"Recording stop");
}

- (void)dealloc
{
	delete recorder;
	[super dealloc];
}
@end
