/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* This is a JNI example where we use native methods to play sounds
 * using OpenSL ES. See the corresponding Java source file located at:
 *
 *   src/com/example/nativeaudio/NativeAudio/NativeAudio.java
 */

#include <assert.h>
#include <jni.h>
#include <string.h>

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
 #include <android/log.h>

// for native audio
#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

static const char hello[] =
#include "hello_clip.h"
;

// pre-recorded sound clips, both are 8 kHz mono 16-bit signed little endian
#define  LOG_TAG    "nativepipboy"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;

static SLObjectItf bqPlayerObject1 = NULL;
static SLPlayItf bqPlayerPlay1;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue1;
static SLEffectSendItf bqPlayerEffectSend1;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
    SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// URI player interfaces
static SLObjectItf uriPlayerObject = NULL;
static SLPlayItf uriPlayerPlay;
static SLSeekItf uriPlayerSeek;

// file descriptor player interfaces
static SLObjectItf fdPlayerObject = NULL;
static SLPlayItf fdPlayerPlay;
static SLSeekItf fdPlayerSeek;

// recorder interfaces
static SLObjectItf recorderObject = NULL;
static SLRecordItf recorderRecord;
static SLAndroidSimpleBufferQueueItf recorderBufferQueue;

// synthesized sawtooth clip
#define SAWTOOTH_FRAMES 8000
static short sawtoothBuffer[SAWTOOTH_FRAMES];

// 5 seconds of recorded audio at 16 kHz mono, 16-bit signed little endian
#define RECORDER_FRAMES (16000 * 5)
static short recorderBuffer[RECORDER_FRAMES];
static unsigned recorderSize = 0;
static SLmilliHertz recorderSR;

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

static short *nextBuffer1;
static unsigned nextSize1;
static int nextCount1;

SLVolumeItf volItf = NULL;


// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == bqPlayerBufferQueue);
    assert(NULL == context);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
        SLresult result;
        // enqueue another buffer
        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        assert(SL_RESULT_SUCCESS == result);
    }
}

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback1(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == bqPlayerBufferQueue1);
    assert(NULL == context);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (--nextCount1 > 0 && NULL != nextBuffer1 && 0 != nextSize1) {
        SLresult result;
        // enqueue another buffer
        result = (*bqPlayerBufferQueue1)->Enqueue(bqPlayerBufferQueue1, nextBuffer1, nextSize1);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        assert(SL_RESULT_SUCCESS == result);
    }
}



// create the engine and output mix objects
void Java_com_tompudding_pipboy_NativePipboy_createEngine(JNIEnv* env, jclass clazz)
{
    SLresult result;
    int trials = 0;

    LOGE("CreateEngine entry %d",result);

    // create engine
    while (trials < 10 && engineObject == NULL) {
        result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
        if(SL_RESULT_SUCCESS != result)
        {
            trials++;
            sleep(1);
            continue;
        }
        break;
    }
    if(trials >= 10)
    {
        LOGE("Error with slCreateEngine %d",result);
        return;
    }

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    if(SL_RESULT_SUCCESS != result)
    {
        LOGE("Error with engine realise %d",result);
        return;
    }

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    //result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_VOLUME, (void*)&volItf);
    //assert(SL_RESULT_SUCCESS == result);

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
            &outputMixEnvironmentalReverb);
    assert(SL_RESULT_SUCCESS == result);
    
    //result = (*volItf)->SetVolumeLevel( volItf, -300);
    //assert(SL_RESULT_SUCCESS == result);

    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example
}



void createPlayer(SLObjectItf *PlayerObject,
                  SLPlayItf   *PlayerPlay  ,
                  SLAndroidSimpleBufferQueueItf *PlayerBufferQueue,
                  SLEffectSendItf *PlayerEffectSend,
                  void (*PlayerCallback)(SLAndroidSimpleBufferQueueItf bq, void *context))
{
    SLresult result;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_22_05,
        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND};
    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, PlayerObject, &audioSrc, &audioSnk,
            2, ids, req);
    assert(SL_RESULT_SUCCESS == result);

    // realize the player
    result = (**PlayerObject)->Realize(*PlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the play interface
    result = (**PlayerObject)->GetInterface(*PlayerObject, SL_IID_PLAY, PlayerPlay);
    assert(SL_RESULT_SUCCESS == result);

    // get the buffer queue interface
    result = (**PlayerObject)->GetInterface(*PlayerObject, SL_IID_BUFFERQUEUE,
            PlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);

    // register callback on the buffer queue
    result = (**PlayerBufferQueue)->RegisterCallback(*PlayerBufferQueue, PlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);

    // get the effect send interface
    result = (**PlayerObject)->GetInterface(*PlayerObject, SL_IID_EFFECTSEND,
            PlayerEffectSend);
    assert(SL_RESULT_SUCCESS == result);

    

    // set the player's state to playing
    result = (**PlayerPlay)->SetPlayState(*PlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
}

// create buffer queue audio player
void Java_com_tompudding_pipboy_NativePipboy_createBufferQueueAudioPlayer(JNIEnv* env,
                                                                                         jclass clazz) {
    createPlayer(&bqPlayerObject,&bqPlayerPlay,&bqPlayerBufferQueue,&bqPlayerEffectSend,bqPlayerCallback);
    createPlayer(&bqPlayerObject1,&bqPlayerPlay1,&bqPlayerBufferQueue1,&bqPlayerEffectSend1,bqPlayerCallback1);
    
}

void PlayClip_c(short *buffer,size_t size,int queue,int q) {
    SLAndroidSimpleBufferQueueItf bqueue = q ? bqPlayerBufferQueue1 :  bqPlayerBufferQueue;

    if(queue == 0)
        (*bqueue)->Clear(bqueue);
    (*bqueue)->Enqueue(bqueue, buffer, size);
}

void StopClip() {
    (*bqPlayerBufferQueue1)->Clear(bqPlayerBufferQueue1);
}

uint32_t CurrentlyPlaying() {
    SLAndroidSimpleBufferQueueState state;
    (*bqPlayerBufferQueue1)->GetState(bqPlayerBufferQueue1,&state);
    return state.count;
}


// create asset audio player
/*
jboolean Java_com_tompudding_pipboy_NativePipboy_createAssetAudioPlayer(JNIEnv* env, jclass clazz,
        jobject assetManager, jstring filename)
{
    SLresult result;

    // convert Java string to UTF-8
    const jbyte *utf8 = (*env)->GetStringUTFChars(env, filename, NULL);
    assert(NULL != utf8);

    // use asset manager to open asset by filename
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    assert(NULL != mgr);
    AAsset* asset = AAssetManager_open(mgr, (const char *) utf8, AASSET_MODE_UNKNOWN);

    // release the Java string and UTF-8
    (*env)->ReleaseStringUTFChars(env, filename, utf8);

    // the asset might not be found
    if (NULL == asset) {
        return JNI_FALSE;
    }

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);
    AAsset_close(asset);

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[1] = {SL_IID_SEEK};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &fdPlayerObject, &audioSrc, &audioSnk,
            1, ids, req);
    assert(SL_RESULT_SUCCESS == result);

    // realize the player
    result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the play interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);

    // get the seek interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &fdPlayerSeek);
    assert(SL_RESULT_SUCCESS == result);

    // enable whole file looping
    result = (*fdPlayerSeek)->SetLoop(fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    assert(SL_RESULT_SUCCESS == result);

    return JNI_TRUE;
}


// set the playing state for the asset audio player
void Java_com_tompudding_pipboy_NativePipboy_setPlayingAssetAudioPlayer(JNIEnv* env,
        jclass clazz, jboolean isPlaying)
{
    SLresult result;

    // make sure the asset audio player was created
    if (NULL != fdPlayerPlay) {

        // set the player's state
        result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, isPlaying ?
            SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
        assert(SL_RESULT_SUCCESS == result);

    }

    }*/

// shut down the native audio system
void Java_com_tompudding_pipboy_NativePipboy_shutdown(JNIEnv* env, jclass clazz)
{

    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
    }

    // destroy file descriptor audio player object, and invalidate all associated interfaces
    if (fdPlayerObject != NULL) {
        (*fdPlayerObject)->Destroy(fdPlayerObject);
        fdPlayerObject = NULL;
        fdPlayerPlay = NULL;
        fdPlayerSeek = NULL;
    }

    // destroy URI audio player object, and invalidate all associated interfaces
    if (uriPlayerObject != NULL) {
        (*uriPlayerObject)->Destroy(uriPlayerObject);
        uriPlayerObject = NULL;
        uriPlayerPlay = NULL;
        uriPlayerSeek = NULL;
    }

    // destroy audio recorder object, and invalidate all associated interfaces
    if (recorderObject != NULL) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = NULL;
        recorderRecord = NULL;
        recorderBufferQueue = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

}
