//
//  AudioRecorderWrapper.h
//  Steganography
//
//  Created by Faizan Aziz on 28/04/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AQRecorder.h"

@interface AudioRecorderWrapper : NSObject {
	AQRecorder*	recorder;
	BOOL isRecording;
}
@property (readonly) AQRecorder	*recorder;
@property (readonly) BOOL isRecording;
- (void)startNewRecordingWithFileName:(NSString *)aFileName;
- (void)stopRecording;
@end
