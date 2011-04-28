#import "MyViewController.h"

@implementation MyViewController

@synthesize myToolbar, overlayViewController, lsbLabel,passwordField, hideView,lowerView, cameraButton;


#pragma mark -
#pragma mark View Controller

void NSFilePath(NSString *fileName, char *tPath){
	int i;
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	
	for(i = 0 ; i<[documentsDirectory length]; i++)
		tPath[i] = [documentsDirectory characterAtIndex:i];
	tPath[i] = '/';
	i++;
	for(int j= 0 ; j<[fileName length]; j++,i++)
		tPath[i] = [fileName characterAtIndex:j];
	tPath[i] = '\0';
}

void filePath(char *fileName, char *tPath){
	int i;
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	
	for(i = 0 ; i<[documentsDirectory length]; i++)
		tPath[i] = [documentsDirectory characterAtIndex:i];
	tPath[i] = '/';
	i++;
	for(int j= 0 ; j<strlen(fileName); j++,i++)
		tPath[i] = fileName[j];
	tPath[i] = '\0';
}

void fskipw(FILE *fp, int num_bytes){
	int i;
	for (i=0; i<num_bytes; i++)
		fputc(0,fp);
}

void printHeaderDate(bmpfile_header *header1, BITMAPINFOHEADER *header2){
	printf("----------------------header data--------------------------\n");
	printf("BMP size=%d\n",header1->filesz);
	printf("creator 1=%d\n",header1->creator1);
	printf("creator 2=%d\n",header1->creator2);
	printf("Offset =%d\n",header1->bmp_offset);
	
	printf("header size=%d\n",header2->header_sz);
	printf("Width=%d\n",header2->width);
	printf("Height=%d\n",header2->height);
	printf("n planes=%d\n",header2->nplanes);
	printf("bits per pixel=%d\n",header2->bitspp);
	printf("Comp method=%d\n",header2->compress_type);
	printf("Image Size=%d\n",header2->bmp_bytesz);
	printf("H res=%d\n",header2->hres);
	printf("V res=%d\n",header2->vres);	
	printf("Num of colors=%d\n",header2->ncolors);	
	printf("I colors=%d\n",header2->nimpcolors);
	printf("----------------------header data--------------------------\n");
}

void write32bit(FILE *fp, int32_t aData){
	int16_t tempData1;
	int32_t tempData;
	
	//Store this first
	tempData = aData&65535;
	tempData1 = tempData;
	fwrite(&tempData, sizeof(word), 1, fp);
	
	//Store this later
	tempData = aData>>16;
	tempData = tempData&65535;
	tempData1 = tempData;
	fwrite(&tempData, sizeof(word), 1, fp);
}

void writeData(char *fileName,bmpfile_header *header1, BITMAPINFOHEADER *header2,int16_t *cPallete,UInt8 *imgData){
	FILE *fwrite1;
	fwrite1 = fopen(fileName, "wb");
	
	printHeaderDate(header1, header2);
	
	fputc('B', fwrite1);
	fputc('M', fwrite1);
	
	write32bit(fwrite1, header1->filesz);					//BMP file size
	fwrite(&header1->creator1, sizeof(word),1, fwrite1);	//Creator 1
	fwrite(&header1->creator2, sizeof(word),1, fwrite1);	//Creator 2
	write32bit(fwrite1, header1->bmp_offset);				//BMP Offset
	
	write32bit(fwrite1, header2->header_sz);				//Header size
	write32bit(fwrite1, header2->width);					//Width
	write32bit(fwrite1, header2->height);					//Heigth
	fwrite(&header2->nplanes, sizeof(word),1, fwrite1);		//No of planes
	fwrite(&header2->bitspp, sizeof(word),1, fwrite1);		//bits per pixel
	write32bit(fwrite1, header2->compress_type);			//Comp method
	write32bit(fwrite1, header2->bmp_bytesz);				//Image size
	write32bit(fwrite1, header2->hres);						//H res
	write32bit(fwrite1, header2->vres);						//V res
	write32bit(fwrite1, header2->ncolors);					//num of colors
	write32bit(fwrite1, header2->nimpcolors);				//I colors
	
	if(header1->bmp_offset != 54){
		for (int16_t i=0; i<header1->bmp_offset-54; i++)
			fputc(cPallete[i],fwrite1);
	}
	
	int64_t index;
	int ctr = 0;
	
	for(index= header2->width *(header2->height-1)*3;index>=0;index-=3*header2->width)
	{
		for(int x=0;x<header2->width*3;x+=3)
		{ 
			fputc(imgData[index+x+2], fwrite1);
			ctr++;
			fputc(imgData[index+x+1], fwrite1);
			ctr++;
			fputc(imgData[index+x], fwrite1);
			ctr++;
		}
	 
		if(header2->width%4!=0){
			ctr+=4 - header2->width%4;
			fskipw(fwrite1, 4 - header2->width%4);
		}
	 }
	
	fclose(fwrite1);
}


- (void)viewDidLoad
{
    self.overlayViewController =
        [[[OverlayViewController alloc] initWithNibName:@"OverlayViewController" bundle:nil] autorelease];

    // as a delegate we will be notified when pictures are taken and when to dismiss the image picker
    self.overlayViewController.delegate = self;
	isHide = YES;
	
	audioRecorder = [[AudioRecorderWrapper alloc] init];
}

- (void)viewDidUnload
{
    self.myToolbar = nil;
    self.overlayViewController = nil;
}

- (void)dealloc{	
	[cameraButton release];
	[myToolbar release];
    [hideView release];
	[lsbLabel release];
	[passwordField release];
    [overlayViewController release];
    [audioRecorder release];
	[super dealloc];
}


#pragma mark -
#pragma mark Toolbar Actions

- (void)showImagePicker:(UIImagePickerControllerSourceType)aSourceType
{	
    if ([UIImagePickerController isSourceTypeAvailable:aSourceType])
    {
        [self.overlayViewController setupImagePicker:aSourceType];
        [self presentModalViewController:self.overlayViewController.imagePickerController animated:YES];
    }
}

#pragma mark -
#pragma mark OverlayViewControllerDelegate

// as a delegate we are being told a picture was taken
- (void)didTakePicture:(UIImage *)picture
{
	//ManipulateImagePixelData([picture CGImage]);
	takenImage = [picture retain];
	
	displayView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 320, 480 - 20 - 44 - 44)];
	displayView.backgroundColor = [UIColor redColor];
	[displayView setImage:takenImage];
	[self.view addSubview:displayView];
	
	items = [[myToolbar items] retain];	
	UIBarButtonItem *cancelButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(cancelPhoto)];
	UIBarButtonItem *accept = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(accept)];
	[myToolbar setItems:[NSArray arrayWithObjects:cancelButton,accept,nil]];
	[cancelButton release];
	[accept release];
	
	[displayView release];
}

// as a delegate we are told to finished with the camera
- (void)didFinishWithCamera{
    [self dismissModalViewControllerAnimated:YES];
}

- (void)cancelPhoto{
	[displayView removeFromSuperview];
	[takenImage release];
	[myToolbar setItems:items];
	[items release];
}

- (void)accept{
	[displayView removeFromSuperview];
	[myToolbar setItems:items];
	[items release];
	
	if(coverImgae)
		[self sourceAccept];
	else
		[self hideAccept];
}

- (void)hideAccept{
	[hideDataFileName release];
	hideDataFileName = [[NSString alloc] initWithString:@"imgToHide.bmp"];
	
	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider([takenImage CGImage]));
	size_t width = CGImageGetWidth([takenImage CGImage]);
	size_t height = CGImageGetHeight([takenImage CGImage]);
	size_t bpp = CGImageGetBitsPerPixel([takenImage CGImage]);
	CFIndex length = CFDataGetLength(data);
	
	int ctr = bpp/8;
	//UInt8 *buffer = malloc(length*24/bpp);
	const UInt8 *tBuf = CFDataGetBytePtr(data);
	int j =0;
	for(int i = 0 ; i < length; i+=ctr){
		//buffer[j] = tBuf[i];
		j++;
		//buffer[j] = tBuf[i+1];
		j++;
		//buffer[j] = tBuf[i+2];
		j++;
	}
	[takenImage release];
	
	bmpfile_header header1;
	header1.creator1 = 0;
	header1.creator2 = 0;
	header1.bmp_offset = 54;
	header1.filesz = 54 + length;

	BITMAPINFOHEADER header2;
	header2.bitspp = 24;
	header2.bmp_bytesz = length;
	header2.compress_type = 0;
	header2.header_sz = 40;
	header2.height = height;
	header2.hres = 0;
	header2.ncolors = 0;
	header2.nimpcolors = 0;
	header2.nplanes = 1;
	header2.vres = 0;
	header2.width = width;
	 
	char fileName[200];
	//filePath("imgToHide.bmp",fileName);
	
	//writeData(fileName, &header1, &header2, nil, buffer);
	//free(buffer);
}

- (void)sourceAccept{
	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider([takenImage CGImage]));
	size_t width = CGImageGetWidth([takenImage CGImage]);
	size_t height = CGImageGetHeight([takenImage CGImage]);
	size_t bpp = CGImageGetBitsPerPixel([takenImage CGImage]);
	CFIndex length = CFDataGetLength(data);
	
	int ctr = bpp/8;
	//sourceBuffer = malloc(length*24/bpp);
	const UInt8 *tBuf = CFDataGetBytePtr(data);
	int j =0;
	for(int i = 0 ; i < length; i+=ctr){
		//sourceBuffer[j] = tBuf[i];
		j++;
		//sourceBuffer[j] = tBuf[i+1];
		j++;
		//sourceBuffer[j] = tBuf[i+2];
		j++;
	}
	[takenImage release];
	
	bmpfile_header header1;
	header1.creator1 = 0;
	header1.creator2 = 0;
	header1.bmp_offset = 54;
	header1.filesz = 54 + length;
	 
	BITMAPINFOHEADER header2;
	header2.bitspp = 24;
	header2.bmp_bytesz = length;
	header2.compress_type = 0;
	header2.header_sz = 40;
	header2.height = height;
	header2.hres = 0;
	header2.ncolors = 0;
	header2.nimpcolors = 0;
	header2.nplanes = 1;
	header2.vres = 0;
	header2.width = width;
	 
	char fileName[200];
	//filePath("write.bmp", fileName);

	//writeData(fileName, &header1, &header2, nil, sourceBuffer);
	//free(sourceBuffer);
}

- (IBAction)sliderMoved:(id)sender{
	UISlider *tSlider = (UISlider*)sender;
	int sliderVal = (int)[tSlider value];
	[lsbLabel setText:[NSString stringWithFormat:@"%d",sliderVal]];
	[tSlider setValue:sliderVal];
}

- (IBAction)sliderEnded:(id)sender{
	UISlider *tSlider = (UISlider*)sender;
	int sliderVal = (int)[tSlider value];
	[lsbLabel setText:[NSString stringWithFormat:@"%d",sliderVal]];
	[tSlider setValue:sliderVal animated:YES];
}

- (IBAction)convert:(id)sender{
	/*int i;
	if(isHide){
		char hideName[200], saveName[200],fileName[200], password[100];
		
		NSFilePath(hideDataFileName, hideName);
		filePath("output.bmp", saveName);
		filePath("write.bmp", fileName);
		for(i = 0 ; i < [passwordField.text length]; i++)
			password[i] = [passwordField.text characterAtIndex:i];
		password[i] = '\0';
		
		hide(hideName, fileName, password, [lsbLabel.text intValue], saveName);
	}
	else {
		char hideName[200], password[100], saveName[200];
		
		NSFilePath(hideDataFileName, hideName);
		for(i = 0 ; i < [passwordField.text length]; i++)
			password[i] = [passwordField.text characterAtIndex:i];
		password[i] = '\0';
		
		filePath("final.", saveName);
		show(hideName, password, saveName, [lsbLabel.text intValue]);
	}*/
	
	if([audioRecorder isRecording])
	   [audioRecorder stopRecording];
	else
	   [audioRecorder startNewRecordingWithFileName:@"test.mp3"];
}


- (IBAction)getSourcePicture:(id)sender{
	
	
	
	
	
	MFMailComposeViewController *controller = [[MFMailComposeViewController alloc] init];
	controller.mailComposeDelegate = self;
	
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	
	NSString *tAttachmentFileName;
	tAttachmentFileName = [[NSString alloc] initWithString:@"imgToHide.bmp"];
	
	NSString *tAttachmentPath = [documentsDirectory stringByAppendingPathComponent:tAttachmentFileName];
	NSData *attachment = [[NSData alloc] initWithContentsOfFile:tAttachmentPath]; 
	[controller addAttachmentData:attachment mimeType:@"image/bmp" fileName:tAttachmentFileName];
	[self presentModalViewController:controller animated:YES];
	[controller release];
	[attachment release];
	[tAttachmentFileName release];
	
	
	
	
	
	
	
	//coverImgae = YES;
	//[self captureData:[[sender titleLabel] text]];
}

- (IBAction)getHidePicture:(id)sender{
	coverImgae = NO;
	[self captureData:[[sender titleLabel] text]];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField{
	return [textField resignFirstResponder];
}

- (void)captureData:(NSString*)aSourceType{
	if([aSourceType isEqualToString:@"Camera"]){
		[self showImagePicker:UIImagePickerControllerSourceTypeCamera];
	}
	else if([aSourceType isEqualToString:@"Photo Library"]){
		[self showImagePicker:UIImagePickerControllerSourceTypePhotoLibrary];
	}
	else if([aSourceType isEqualToString:@"File"]){
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex:0];
		
		NSFileManager *defaultManager = [NSFileManager defaultManager];
		NSArray *fileList = [defaultManager contentsOfDirectoryAtPath:documentsDirectory error:nil];
		
		tableDataSource = [fileList retain];
		
		tableViewController = [[UITableViewController alloc] initWithStyle:UITableViewStylePlain];
		tableViewController.tableView.dataSource = self;
		tableViewController.tableView.delegate = self;
		[self presentModalViewController:tableViewController animated:YES];
		[tableViewController release];
	}
	
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
	static NSString *CellIdentifier = @"FileManagerCell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
	//Get the children of the present item.
    cell.textLabel.text = [tableDataSource objectAtIndex:indexPath.row];
    cell.showsReorderControl = YES;
    
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
	return [tableDataSource count];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
	[tableViewController dismissModalViewControllerAnimated:YES];
	
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	
	if(isHide){
		if(coverImgae){
			
			UIImage *newImage = [UIImage imageWithContentsOfFile:[documentsDirectory stringByAppendingPathComponent:[tableDataSource objectAtIndex:indexPath.row]]];
			[tableDataSource release];
			[self didTakePicture:newImage];
		}
		else {
			[hideDataFileName release];
			hideDataFileName = [[NSString alloc] initWithString:[tableDataSource objectAtIndex:indexPath.row]];
			[tableDataSource release];
		}
	}
	else {
		[hideDataFileName release];
		hideDataFileName = [[NSString alloc] initWithString:[tableDataSource objectAtIndex:indexPath.row]];
		[tableDataSource release];
	}

}

- (IBAction)segmentMoved:(id)sender{
	/*UISegmentedControl *seg = (UISegmentedControl*)sender;
	switch ([seg selectedSegmentIndex]) {
		case 1:
			isHide = NO;
			[UIView animateWithDuration:.3 animations:^{
				[hideView setAlpha:0];
				[lowerView setFrame:CGRectMake(hideView.frame.origin.x , hideView.frame.origin.y, lowerView.frame.size.width, lowerView.frame.size.height)];
				[cameraButton setAlpha:0];
			}];
			break;
		case 0:
			isHide = YES;
			[UIView animateWithDuration:.3 animations:^{
				[hideView setAlpha:1];
				[lowerView setFrame:CGRectMake(18 , 195, lowerView.frame.size.width, lowerView.frame.size.height)];
				[cameraButton setAlpha:1];
			}];
			break;
		default:
			break;
	}*/
}


#pragma mark mailComposeDelegate
- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error {
	NSLog(@"%@", error);
	NSLog(@"%@", result);
	[self becomeFirstResponder];
	[self dismissModalViewControllerAnimated:YES];
}


@end