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

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLU;
import android.opengl.GLUtils;
import android.os.SystemClock;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.opengles.GL10;

public class PipboyRenderer implements GLView.Renderer{

    ProgressCallback progressCallback;

    public PipboyRenderer(Context context,Progress cb) {
        mContext = context;
        progressCallback = cb;
        NativePipboy.load(progressCallback);
    }

    public int[] getConfigSpec() {
        // We don't need a depth buffer, and don't care about our
        // color depth.
        int[] configSpec = {
                EGL10.EGL_DEPTH_SIZE, 0,
                EGL10.EGL_ALPHA_SIZE, 8,
                EGL10.EGL_NONE
        };
        return configSpec;
    }

    public void surfaceCreated(GL10 gl) {
        NativePipboy.init();
    }

    public void drawFrame(GL10 gl) {
        NativePipboy.step();
    }

    public void sizeChanged(GL10 gl, int w, int h) {
        gl.glViewport(0, 0, w, h);

        /*
        * Set our projection matrix. This doesn't have to be done
        * each time we draw, but usually a new projection needs to
        * be set when the viewport is resized.
        */

        float ratio = (float) w / h;
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glLoadIdentity();
        if(w != h)
            GLU.gluOrtho2D(gl, 1, 0, 1, 0);
        //gl.glOrthof (0, 1, 0, 1, -1, 1);

        
        Log.w("nativepipboy", String.format("xy %d %d\n", w, h));
        
        //gl.glFrustumf(-ratio, ratio, -1, 1, 3, 7);

    }

    private Context mContext;
 
}
