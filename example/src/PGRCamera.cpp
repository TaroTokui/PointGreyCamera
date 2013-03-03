//
//  PGRCamera.cpp
//  ofxKinectExample
//
//  Created by tokui on 13/02/08.
//
//

#include "PGRCamera.h"

PGRCamera::PGRCamera(){
    
	/* get camera */
    dc1394camera_list_t * mCameraList;
    dc1394_t * d;
    
    d = dc1394_new ();
    chkErr(dc1394_camera_enumerate (d, &mCameraList));
    if (mCameraList->num == 0) {
        cout << "### no camera found!" << endl;
    }
    
    const int mCameraID = 0;
    mCamera = dc1394_camera_new (d, mCameraList->ids[mCameraID].guid);
    
    dc1394_iso_release_bandwidth(mCamera, INT_MAX);
	for (int channel = 0; channel < 64; channel++) {
		dc1394_iso_release_channel(mCamera, channel);
	}
    
    if (!mCamera) {
        cout << "### failed to initialize camera with guid %llx" << mCameraList->ids[mCameraID].guid << endl;
    } else {
        cout << "+++ camera (GUID): " << mCamera->vendor << "(" << mCamera->guid << ")" <<endl;
    }
    dc1394_camera_free_list (mCameraList);
    
}

PGRCamera::~PGRCamera(){}

void PGRCamera::setup(){
    
    /* setup camera */
    dc1394video_mode_t mVideoMode = DC1394_VIDEO_MODE_FORMAT7_3;
    unsigned int mMaxWidth;
    unsigned int mMaxHeight;
    
    chkErr(dc1394_video_set_operation_mode(mCamera, DC1394_OPERATION_MODE_1394B));
    chkErr(dc1394_format7_get_max_image_size(mCamera, mVideoMode, &mMaxWidth, &mMaxHeight));
    cout << "+++ maximum size for current Format7 mode is " << mMaxWidth << "x" << mMaxHeight << endl;
    
    // set gain
    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL);
    dc1394_feature_set_absolute_control(mCamera, DC1394_FEATURE_GAIN, DC1394_ON);
    dc1394_feature_set_power(mCamera, DC1394_FEATURE_GAIN, DC1394_ON );
    dc1394_feature_set_absolute_value(mCamera, DC1394_FEATURE_GAIN, 30.0);
    
    // set exposure
    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_EXPOSURE, DC1394_FEATURE_MODE_MANUAL);
    dc1394_feature_set_absolute_control(mCamera, DC1394_FEATURE_EXPOSURE, DC1394_ON);
    dc1394_feature_set_power(mCamera, DC1394_FEATURE_EXPOSURE, DC1394_ON );
    dc1394_feature_set_absolute_value(mCamera, DC1394_FEATURE_EXPOSURE, 0.1);
    
    chkErr(dc1394_format7_set_roi(mCamera,
                                       mVideoMode,
                                       DC1394_COLOR_CODING_MONO8,
                                       DC1394_USE_MAX_AVAIL,
                                       0, 0, mMaxWidth, mMaxHeight));
    
    // パケットサイズを設定 = format7ではフレームレートが決まる
    // capture_setupの前に呼び出すこと
    chkErr(dc1394_format7_set_packet_size(mCamera, mVideoMode, 1024));
    
    chkErr(dc1394_video_set_mode(mCamera, mVideoMode));
    const int NUMBER_DMA_BUFFERS = 2;
    chkErr(dc1394_capture_setup(mCamera,
                                     NUMBER_DMA_BUFFERS,
                                     DC1394_CAPTURE_FLAGS_DEFAULT));
    
    /* grab first frame and dump info */
    chkErr(dc1394_video_set_transmission(mCamera, DC1394_ON));
    dc1394_video_set_framerate(mCamera, DC1394_FRAMERATE_60);
    
    cout << "+++ capture first frame ..." << endl;
    dc1394video_frame_t* mFrame = NULL;
    chkErr(dc1394_capture_dequeue(mCamera, DC1394_CAPTURE_POLICY_WAIT, &mFrame));
    if (mFrame) {
        cout << "+++ dimensions        : " << mFrame->size[0] << ", " << mFrame->size[1] << endl;
        cout << "+++ color coding      : " << mFrame->color_coding << endl;
        cout << "+++ data depth        : " << mFrame->data_depth << endl;
        cout << "+++ stride            : " << mFrame->stride << endl;
        cout << "+++ video_mode        : " << mFrame->video_mode << endl;
        cout << "+++ total_bytes       : " << mFrame->total_bytes << endl;
        cout << "+++ packet_size       : " << mFrame->packet_size << endl;
        cout << "+++ packets_per_frame : " << mFrame->packets_per_frame << endl;
        chkErr(dc1394_capture_enqueue(mCamera, mFrame));
    }
    
    // メモリ確保
	mCameraTexture 		= new unsigned char [mMaxWidth*mMaxHeight*3];
    
    // 画像領域を確保
    srcImage.allocate(mMaxWidth, mMaxHeight);
    grayImage.allocate(mMaxWidth, mMaxHeight);
}

void PGRCamera::update(){
    
    dc1394video_frame_t * mFrame = NULL;
    chkErr(dc1394_capture_dequeue(mCamera, DC1394_CAPTURE_POLICY_POLL, &mFrame));
    if (mFrame) {
        unsigned int RGB_CHANNELS = 3;
        unsigned int mRGBImageSize = mFrame->size[0] * mFrame->size[1] * RGB_CHANNELS;
        chkErr(dc1394_bayer_decoding_8bit(mFrame->image,
                                               mCameraTexture,
                                               mFrame->size[0], mFrame->size[1],
                                               DC1394_COLOR_FILTER_RGGB,
                                               DC1394_BAYER_METHOD_BILINEAR));
        chkErr(dc1394_capture_enqueue(mCamera, mFrame));
        
        srcImage.setFromPixels(mCameraTexture, mFrame->size[0], mFrame->size[1]);
    }
    srcImage.flagImageChanged();
    
}

void PGRCamera::draw(){
    srcImage.draw(0, 0, 320, 240);
}

ofxCvGrayscaleImage PGRCamera::getGrayImage(){
    grayImage = srcImage;
    return grayImage;
}

void PGRCamera::chkErr(dc1394error_t pError) {
    if (pError) {
        cout << "### ERROR #" << pError << " occured." << endl;
    }
}

void PGRCamera::exposure(float value){
    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_EXPOSURE, DC1394_FEATURE_MODE_MANUAL);
    dc1394_feature_set_absolute_control(mCamera, DC1394_FEATURE_EXPOSURE, DC1394_ON);
    dc1394_feature_set_absolute_value(mCamera, DC1394_FEATURE_EXPOSURE, value);
    dc1394_video_set_framerate(mCamera, DC1394_FRAMERATE_30);
}
