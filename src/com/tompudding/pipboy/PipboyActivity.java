/*
 * Copyright (C) 2008 Google Inc.
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
 */

package com.tompudding.pipboy;

import javax.microedition.khronos.opengles.GL;

import android.app.Activity;
import android.opengl.GLDebugHelper;
import android.os.Bundle;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.View.OnClickListener;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.MenuItem;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.Display;
import android.view.WindowManager;
import android.content.Context;
import android.view.GestureDetector;
import android.util.Log;
import android.content.DialogInterface;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.os.Handler;

class MyGestureDetector extends SimpleOnGestureListener implements OnClickListener{
    public float x;
    public float y;
    private static final int SWIPE_MIN_DISTANCE = 120;
    private static final int SWIPE_MAX_OFF_PATH = 250;
    private static final int SWIPE_THRESHOLD_VELOCITY = 200;
    MyGestureDetector(float a,float b) {
        x = a;
        y = b;
    }
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        Log.i("nativepipboy",String.format("onfling"));
        try {
            if (Math.abs(e1.getY() - e2.getY()) > SWIPE_MAX_OFF_PATH)
                return false;
            // right to left swipe
            if(e1.getX() - e2.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                //Log.i("nativepipboy", "Left Swipe");
                NativePipboy.LeftSwipe();
            }  else if (e2.getX() - e1.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
                NativePipboy.RightSwipe();
                //Log.i("nativepipboy", "Right Swipe");
            }
        } catch (Exception e) {
            // nothing
        }
        return false;
    }
 
    public boolean onTouch(View v, MotionEvent event) {
        Log.i("nativepipboy",String.format("touch event 1a %d",event.getAction()));

        return false;
    };
    public boolean onSingleTapUp(MotionEvent event) {
        Log.i("nativepipboy","onsingletapup");
        NativePipboy.TouchEvent((x-event.getRawX())/x,event.getRawY()/y);
        return false;
    }
    public void onLongPress(MotionEvent event) {
        NativePipboy.LongPress();
        Log.i("nativepipboy","onlongpress");
    }
    public void onClick(View v) {
        //Log.i("nativepipboy","OnClick 2");
        //Filter f = (Filter) v.getTag();
        //FilterFullscreenActivity.show(this, input, f);
    }

}

interface ProgressCallback {
    void updateProgress(float progress);
    void fatalError(String message);
}

class Progress implements ProgressCallback {
    PipboyActivity root;
    public Progress(PipboyActivity r) {
        root = r;
    }
    public void updateProgress(float progress)  {
        Log.i("nativepipboy",String.format("progress update %f",progress));
        root.progressBar.setProgress((int)(progress*100));
        if(progress >= 1.0) {
            root.progressBar.dismiss();
        }
    }
    public void fatalError(String message) {
        Log.i("nativepipboy","Fatal error : " + message);
        
        AlertDialog alertDialog = new AlertDialog.Builder(root).create();
        alertDialog.setTitle("Fatal Error");
        alertDialog.setMessage(message);
        alertDialog.setButton("Quit", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                root.finish();
            }
        });
        alertDialog.show();
        //root.finish();
    }
}

public class PipboyActivity extends Activity implements OnClickListener{

    /** Set to true to enable checking of the OpenGL error code after every OpenGL call. Set to
     * false for faster code.
     *
     */
    private final static boolean DEBUG_CHECK_GL_ERROR = true;
    private GestureDetector gestureDetector;
    private final MyGestureDetector mgd = new MyGestureDetector(0,0);
    private Handler mHandler = new Handler();
    Progress jim = new Progress(this);
    ProgressDialog progressBar;
    PipboyRenderer bob;

    public void createbob() {
        Log.i("nativepipboy",String.format("creating bob"));
        bob = new PipboyRenderer(PipboyActivity.this,jim);
        Log.i("nativepipboy",String.format("created bob"));
        runOnUiThread(new Runnable() {
            public void run() {
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                Log.i("nativepipboy",String.format("Jimbo:%d:%d",getWindowManager().getDefaultDisplay().getHeight(),getWindowManager().getDefaultDisplay().getWidth()));
                setContentView(R.layout.main);
                mGLView = (GLView) findViewById(R.id.glview);
                float width  = getWindowManager().getDefaultDisplay().getWidth();
                float height = getWindowManager().getDefaultDisplay().getHeight();
                gestureDetector = new GestureDetector(mgd);
                mGLView.setOnTouchListener(new View.OnTouchListener() {
                    public boolean onTouch(View v, MotionEvent event) {
                        Log.i("nativepipboy",String.format("touch event 1b %d",event.getAction()));
                        mgd.x  = getWindowManager().getDefaultDisplay().getWidth();
                        mgd.y  = getWindowManager().getDefaultDisplay().getHeight();
                
                        if (gestureDetector.onTouchEvent(event)) {
                            return true;
                        }
                        return false;
                    }
                });


                mGLView.setOnClickListener(PipboyActivity.this); 
                
                if (DEBUG_CHECK_GL_ERROR) {
                    mGLView.setGLWrapper(new GLView.GLWrapper() {
                        public GL wrap(GL gl) {
                            return GLDebugHelper.wrap(gl, GLDebugHelper.CONFIG_CHECK_GL_ERROR, null);
                        }});
                }

                mGLView.setRenderer(bob);
                //setContentView(mGLView);
                mGLView.requestFocus();
            }
        });
    }       
            

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        progressBar = new ProgressDialog(this);
        progressBar.setCancelable(false);
        progressBar.setMessage("Loading");
        progressBar.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
        progressBar.setProgress(0);
        progressBar.setMax(100);
        progressBar.show();
        NativePipboy.createEngine();
        NativePipboy.createBufferQueueAudioPlayer();
        new Thread(new Runnable() {
            public void run() {
                createbob();
            }
        }).start();

        //


        
    }


    @Override
    protected void onPause() {
        super.onPause();
        if(null != mGLView) {
            mGLView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(null != mGLView) {
            mGLView.onResume();
        }
    }

    @Override  public void onConfigurationChanged(Configuration newConfig) {
     super.onConfigurationChanged(newConfig);
    }
 
    @Override public boolean onPrepareOptionsMenu (Menu menu){
        Log.i("nativepipboy","Menu1");
        NativePipboy.MenuButton();
        return false;
    }

    public void onClick(View v) {
         Log.i("nativepipboy","OnClick 1");
        //Filter f = (Filter) v.getTag();
        //FilterFullscreenActivity.show(this, input, f);
    }

    

    private GLView mGLView;
}
