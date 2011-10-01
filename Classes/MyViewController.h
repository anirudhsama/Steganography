#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioServices.h>
#import <MessageUI/MessageUI.h>

#import "OverlayViewController.h"
#include "BasicImageManipulator.h"
#import "AudioRecorderWrapper.h"

typedef enum{
	kSourceCamera,
	kSourePhotoLib,
	kSourceFile
}kSource;

typedef struct {
	int32_t filesz;
	int16_t creator1;
	int16_t creator2;
	int32_t bmp_offset;
}bmpfile_header;

typedef struct {
	int32_t header_sz;//40
	int32_t width;//width of the image
	int32_t height;//height of the image
	int16_t nplanes;//set to 1
	int16_t bitspp;
	int32_t compress_type;
	int32_t bmp_bytesz;
	int32_t hres;
	int32_t vres;
	int32_t ncolors;
	int32_t nimpcolors;
} BITMAPINFOHEADER;

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

@interface MyViewController : UIViewController <UIImagePickerControllerDelegate, UITextFieldDelegate,
                                                OverlayViewControllerDelegate,UITableViewDataSource,UITableViewDelegate,MFMailComposeViewControllerDelegate>
{
    UIToolbar *myToolbar;
    
    OverlayViewController *overlayViewController; // the camera custom overlay view
	
	UIImage *takenImage;
	NSArray *items;
	UIImageView *displayView;
	
	UILabel *lsbLabel;
	UITextField *passwordField;
	
	UInt8 *sourceBuffer;
	size_t sourceHeight, sourceWidth;
	NSString *hideDataFileName;
	NSString *coverFileName;
	
	BOOL coverImgae;
	BOOL isHide;
	kSource sourceType;
	
	NSArray *tableDataSource;
	UITableViewController *tableViewController;
	UIView *hideView,*lowerView;
	UIButton *cameraButton;
	
	AudioRecorderWrapper *audioRecorder;
	BOOL isMail;
	MFMailComposeViewController *mailController;
	
	IBOutlet UIActivityIndicatorView *recordingActivityIndicator;
	IBOutlet UILabel *recordingLable;
}

@property (nonatomic, retain) IBOutlet UIToolbar *myToolbar;
@property (nonatomic, retain) IBOutlet UILabel *lsbLabel;
@property (nonatomic, retain) IBOutlet UITextField *passwordField;
@property (nonatomic, retain) IBOutlet UIView *hideView,*lowerView;
@property (nonatomic, retain) IBOutlet UIButton *cameraButton;
@property (nonatomic, retain) IBOutlet UILabel *recordingLable;
@property (nonatomic, retain) IBOutlet UIActivityIndicatorView *recordingActivityIndicator;
@property (nonatomic, retain) IBOutlet UIBarButtonItem *mailButton;
@property (nonatomic, retain) OverlayViewController *overlayViewController;



// toolbar buttons
- (IBAction)getSourcePicture:(id)sender;
- (IBAction)getDataToHide:(id)sender;
- (IBAction)sliderMoved:(id)sender;
- (IBAction)sliderEnded:(id)sender;
- (IBAction)convert:(id)sender;
- (IBAction)segmentMoved:(id)sender;

- (IBAction)mail:(id)sender;

- (void)captureData:(NSString*)aSourceType;

- (void)sourceAccept;
- (void)hideAccept;

void filePath(char *fileName, char *tPath);
void NSFilePath(NSString *fileName, char *tPath);

@end

